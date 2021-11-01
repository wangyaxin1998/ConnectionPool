#include "ConnectionPool.h"

ConnectionPool *ConnectionPool::getConnectionPool()
{
	static ConnectionPool pool;
	return &pool;
}

shared_ptr<Connection> ConnectionPool::getConnection()
{
	unique_lock<mutex> lock(_queueMutex);
	while (_connectionQue.empty())
	{
		//如果有生产者线程在_connectionTimeOut时间之内生产好，会通知消费者，在超时时间内被唤醒
		//如果是超时时间内还没有被唤醒，而且超时了
		if (cv_status::timeout == cv.wait_for(lock, chrono::microseconds(_connectionTimeOut)))
		{
			if (_connectionQue.empty())
			{
				LOG("获取空闲连接超时了...获取连接失败！");
				return nullptr;
			}
		}
	}
	/*
	* 正常情况下shared_ptr使用完会自动析构对象
	* 但我们不需要析构对象，再次把指针加入队尾即可
	* std::shared_ptr<T> s(a, [](T* p){ delete[] p; });
	* 默认的shared_ptr删除器
	* 这里自己写删除器，注意获取外部变量[&]
	*/
	shared_ptr<Connection> sp(_connectionQue.front(), [&](Connection *c)
							  {
								  unique_lock<mutex> lock(_queueMutex);
								  c->refreshAliveTime();
								  _connectionQue.push(c);
							  });

	_connectionQue.pop();
	/*队列中最后一个connection，通知生产者开始生产*/
	if (_connectionQue.empty())
		cv.notify_all();
	return sp;
}

ConnectionPool::ConnectionPool()
{
	if (!loadConfigFile())
		return;
	/*创建初始数量的连接*/
	for (int i = 0; i < _initSize; ++i)
	{
		Connection *p = new Connection();
		p->connect(_ip, _port, _username, _password, _dbname);
		p->refreshAliveTime();
		_connectionQue.push(p);
		_conntectionCnt++;
	}

	/*启动一个新的线程作为连接生产者*/
	thread produce(std::bind(&ConnectionPool::produceConnectionTask, this));
	produce.detach();

	/*启动一个新的定时线程，扫描超过maxIdleTime时间的空闲连接，将这些连接回收*/
	thread scanner(std::bind(&ConnectionPool::scannerConnectionTask, this));
	scanner.detach();
}

bool ConnectionPool::loadConfigFile()
{
	FILE *pf = fopen("../example/mysql.ini", "r");
	if (pf == nullptr)
	{
		LOG("mysql.ini file is not exist!");
		return false;
	}
	unordered_map<string, string> map;
	//如果文件未到末尾
	while (!feof(pf))
	{
		char line[1024] = {0};
		fgets(line, 1024, pf);
		string str = line;
		int idx = str.find('=', 0);
		if (idx == -1)
			continue;
		int endidx = str.find('\n', idx);
		string key = str.substr(0, idx);
		string value = str.substr(idx + 1, endidx - idx - 1);
		map[key] = value;
	}
	_ip = map["ip"];
	_port = atoi(map["port"].c_str());
	_username = map["username"];
	_password = map["password"];
	_dbname = map["dbname"];
	_initSize = atoi(map["initSize"].c_str());
	_maxSize = atoi(map["maxSize"].c_str());
	_maxIdleTime = atoi(map["maxIdleTime"].c_str());
	_connectionTimeOut = atoi(map["connectionTimeOut"].c_str());

	return true;
}

void ConnectionPool::produceConnectionTask()
{
	while (true)
	{
		unique_lock<mutex> lock(_queueMutex);
		while (!_connectionQue.empty())
		{
			cv.wait(lock); //队列不空，此处生产线程进入等待状态，等待就会放锁
		}
		if (_conntectionCnt < _maxSize)
		{
			Connection *p = new Connection();
			p->connect(_ip, _port, _username, _password, _dbname);
			p->refreshAliveTime();
			_connectionQue.push(p);
			_conntectionCnt++;
		}
		//通知消费者线程，可以消费连接了，消费者都从阻塞态->就绪态
		cv.notify_all();
		//等出本while循环才会解锁，然后各种就绪态的开始抢cpu资源
	}
}

void ConnectionPool::scannerConnectionTask()
{
	while (true)
	{ //通过sleep模拟定时效果
		this_thread::sleep_for(chrono::seconds(_maxIdleTime));
		//扫描整个队列，释放多余连接
		unique_lock<mutex> lock(_queueMutex);
		while (_conntectionCnt > _initSize)
		{
			Connection *p = _connectionQue.front();
			if (p->getAliveTime() >= (_maxIdleTime * 1000))
			{
				_connectionQue.pop();
				_conntectionCnt--;
				delete p;
			}
			else
			{
				//对头都没有超过_maxIdleTime，后面的连接更不可能超过_maxIdleTime
				break;
			}
		}
	}
}
