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
	int size = 10000;
	vector<thread> t(4);
	for (int i = 0; i < t.size(); ++i)
	{
		t[i] = thread([&]()
			{
				for (int j = 0; j < size / t.size(); ++j)
				{
					auto conn = cp->getConnection();
					char sql[1024] = { 0 };
					sprintf(sql, "insert into user(name,age,sex) values('%s','%d','%s')", "Jay", 40, "male");
					conn->update(sql);
				}
			});
		t[i].join();
	}
	clock_t end = clock();
	cout << (end - begin) << "ms" << endl;
	return 0;
}