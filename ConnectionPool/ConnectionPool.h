#pragma once
#include <string>
#include<queue>
#include<mutex>
#include < unordered_map >
#include<atomic>
#include<thread>
#include<condition_variable>
#include<memory>
#include<functional>
using namespace std;
#include "Connection.h"


/*
* ����ģʽ
* ʵ�����ӳع���ģ��
*/
class	 ConnectionPool {
public:
	static ConnectionPool* getConnectionPool();
	/*���ⲿ�ṩ�ӿڣ������ӳ��л�ȡһ�����õĿ�������*/
	shared_ptr<Connection> getConnection();
private:
	ConnectionPool();
	bool loadConfigFile();//�����ļ�����
	void produceConnectionTask();//�����̣߳�ר�Ÿ�������������
	void scannerConnectionTask();// ɨ�賬��maxIdleTimeʱ��Ŀ������ӣ�����Щ���ӻ���

	string _ip;//mysql��ip��ַ
	unsigned short _port;//mysql�Ķ˿ں�
	string _username;//mysql��¼�û���
	string _password;//mysql��¼����
	string _dbname;//���ӵ����ݿ�����
	int _initSize;//���ӳصĳ�ʼ������
	int _maxSize;//���ӳ����������
	int _maxIdleTime;//���ӳ�������ʱ��
	int _connectionTimeOut;//���ӳػ�ȡ���ӵĳ�ʱʱ��

	queue<Connection*> _connectionQue;//�洢mysql���ӵĶ��У���������ʹ�õ����ӺͶ������������֮��
	mutex _queueMutex;//ά���̶߳��е��̰߳�ȫ������
	atomic<int> _conntectionCnt;//��¼������������connection���ӵ�������
	condition_variable cv;//���������������������������̺߳������̵߳�ͨ��
};