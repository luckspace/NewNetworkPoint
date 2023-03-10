#include <qapplication.h>

#include "ConsoleWidget.hpp"

#include "LuckTool.hpp"
//	luck thread pool
#include "LuckThreadPool.hpp"
#include "MySqlThreadPool.hpp"
//	mysql
#include "PermisSyncToMySql.hpp"

void init()
{
	Singleton<PermissionSyncManager>::Instance()->GainFromDatabase({ "127.0.0.1", "root", "356231064", "localpoint" });

	auto permis_sync = Singleton<PermissionSyncManager>::Instance();

	permis_sync->InsertPermission("default", "default.base");
}

class A
{
public:
	void test(int a)
	{

	}
};

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	luck::sql::MySqlAttribute mysql_root("127.0.0.1:3306", "root", "356231064");

	luck::ThreadPool::SelfShare th_base = std::make_shared<luck::ThreadPool>();
	luck::sql::MySqlThreadPool::SelfShare th_mysql = std::make_shared<luck::sql::MySqlThreadPool>(th_base, mysql_root);

	Singleton<PermissionSyncManager>::Instance()->SetMySqlThPool(th_mysql);

	init();

	ConsoleWidget w;
	w.show();

	return app.exec();
}
