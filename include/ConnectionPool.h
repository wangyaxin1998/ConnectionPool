#pragma once
#include <string>
#include <queue>
#include <mutex>
#include <unordered_map >
#include <atomic>
#include <thread>
#include <condition_variable>
#include <memory>
#include <functional>
using namespace std;
#include "Connection.h"

/*
* 单例模式
* 实现连接池功能模块
*/
class ConnectionPool
{
public:
	static ConnectionPool *getConnectionPool();
	/*给外部提供接口，从连接池中获取一个可用的空闲连接*/
	shared_ptr<Connection> getConnection();

private:
	ConnectionPool();
	bool loadConfigFile();		  //加载文件配置
	void produceConnectionTask(); //单独线程，专门负责生产新连接
	void scannerConnectionTask(); // 扫描超过maxIdleTime时间的空闲连接，将这些连接回收

	string _ip;				//mysql的ip地址
	unsigned short _port;	//mysql的端口号
	string _username;		//mysql登录用户名
	string _password;		//mysql登录密码
	string _dbname;			//连接的数据库名称
	int _initSize;			//连接池的初始连接量
	int _maxSize;			//连接池最大连接量
	int _maxIdleTime;		//连接池最大空闲时间
	int _connectionTimeOut; //连接池获取连接的超时时间

	queue<Connection *> _connectionQue; //存储mysql连接的队列，等于正在使用的连接和队列里面的连接之和
	mutex _queueMutex;					//维护线程队列的线程安全互斥锁
	atomic<int> _conntectionCnt;		//记录连接锁创建的connection连接的总数量
	condition_variable cv;				//设置条件变量，用于连接生产线程和消费线程的通信
};