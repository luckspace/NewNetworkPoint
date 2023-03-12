#include <qapplication.h>

#include "ConsoleWidget.hpp"

#include "LogSyncToMySql.hpp"
#include "PermisSyncToMySql.hpp"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

	luck::sql::MySqlAttribute mysql_root("127.0.0.1:3306", "root", "356231064", "localpoint");
	
	//	base thread pool
	luck::ThreadPool::SelfShare th_base = std::make_shared<luck::ThreadPool>();
	//	depen pool
	luck::sql::MySqlThreadPool::SelfShare th_mysql = std::make_shared<luck::sql::MySqlThreadPool>(th_base, mysql_root);

	Singleton<LogSyncManager>::Instance()->SetMySqlPool(th_mysql);
	Singleton<PermissionSyncManager>::Instance()->SetMySqlThPool(th_mysql);
	Singleton<PermissionSyncManager>::Instance()->GainFromDatabase(mysql_root);

	ConsoleWidget w;
	w.show();

	{
		auto log_instance = Singleton<LogSyncManager>::Instance();

		LogSyncEvent log_event("log", 100, "application run ok;", std::string() + __FILE__ + "=>" + std::to_string(__LINE__));
		log_instance->Upload(log_event);
	}
	{
		auto permis_instance = Singleton<PermissionSyncManager>::Instance();

		permis_instance->InsertPermission("default", "luck.luck.luck.luck.luck.base");
	}

	return app.exec();
}
