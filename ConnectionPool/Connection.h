#pragma once
#include<iostream>
#include <string>
#include<ctime>
#include <mysql.h>
#include "public.h"
using namespace std;

// ���ݿ������
class Connection
{
public:
	// ��ʼ�����ݿ�����
	Connection();

	// �ͷ����ݿ�������Դ
	~Connection();

	// �������ݿ�
	bool connect(string ip, unsigned short port, string user, string password,
		string dbname);

	// ���²��� insert��delete��update
	bool update(string sql);

	// ��ѯ���� select
	MYSQL_RES* query(string sql);

	//ˢ��һ�����ӵ���ʼ�Ŀ���ʱ���
	void refreshAliveTime();

	//���ش���ʱ��
	clock_t getAliveTime() const;
private:
	MYSQL* _conn; // ��ʾ��MySQL Server��һ������
	clock_t _alivetime;//�ڶ����еĿ���ʱ��
};