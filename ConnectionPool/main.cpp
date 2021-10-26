using namespace std;
#include "ConnectionPool.h"
int main() {
	/*
	Connection conn;
	char sql[1024] = { 0 };
	sprintf(sql, "insert into user(name,age,sex) values('%s','%d','%s')","Jay",40,"male");
	conn.connect("127.0.0.1", 3306, "root", "980926", "chat");
	conn.update(sql);
	*/
	ConnectionPool* cp= ConnectionPool::getConnectionPool();
	clock_t begin = clock();
	int size = 1000;
	clock_t end = clock();
	vector<thread> t(4);
	for (int i = 0; i < t.size(); ++i)
	{

	}
	
	cout << (end - begin) << "ms" << endl;
	return 0;
}