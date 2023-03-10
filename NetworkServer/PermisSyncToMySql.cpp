#include "PermisSyncToMySql.hpp"
#include "LogSyncToMySql.hpp"

#include <sstream>

void PermissionSyncManager::SyncToMySql(luck::sql::MySqlThreadPool::MySqlConnectionPointer _sql_con, PermissionSyncEvent _sql_event)
{
	std::string this_thread_id;
	{
		std::stringstream sin;
		sin << std::this_thread::get_id();
		this_thread_id = sin.str();
	}

	std::unique_ptr<::sql::PreparedStatement> sql_smt_del(_sql_con->prepareStatement(R"(delete from `permis` where `group` = ? and `permis` like ?)"));
	std::unique_ptr<::sql::PreparedStatement> sql_smt_ins(_sql_con->prepareStatement(R"(insert into `permis` (`group`, `permis`, `operator`, `status`, `time`) values (?, ?, ?, ?, ?))"));

	if (_sql_event.mode & _sql_event.ModeDeleteMask)
	{
		sql_smt_del->setString(1, _sql_event.group);
		sql_smt_del->setString(2, _sql_event.msg + "%");
		try
		{
			sql_smt_del->execute();

			Singleton<LogSyncManager>::Instance()->Upload(LogSyncEvent(
				"permis changed",
				0,
				std::string() + "operator:local th_pool " + this_thread_id + ";mode:del;status:ok;",
				_sql_event.msg
			));
		}
		catch (sql::SQLException& _e)
		{
			Singleton<LogSyncManager>::Instance()->Upload(LogSyncEvent(
				"permis changed",
				_e.getErrorCode(),
				std::string() + "operator:local th_pool " + this_thread_id + ";mode:del;status:ok;msg:" + _sql_event.msg + ";",
				_e.what()
			));
		}
	}
	if (_sql_event.mode & _sql_event.ModeInsertMask)
	{
		sql_smt_ins->setString(1, _sql_event.group);
		sql_smt_ins->setString(2, _sql_event.msg);
		sql_smt_ins->setString(3, std::string() + "local server");
		sql_smt_ins->setString(4, std::string() + "th_pool " + this_thread_id);
		sql_smt_ins->setUInt64(5, std::chrono::system_clock::now().time_since_epoch().count());
		try
		{
			sql_smt_ins->execute();
			Singleton<LogSyncManager>::Instance()->Upload(LogSyncEvent(
				"permis changed",
				0,
				std::string() + "operator:local th_pool " + this_thread_id + ";mode:ins;status:ok;",
				_sql_event.msg
			));
		}
		catch (sql::SQLException& _e)
		{
			sql_smt_ins->execute();
			Singleton<LogSyncManager>::Instance()->Upload(LogSyncEvent(
				"permis exception",
				_e.getErrorCode(),
				std::string() + "operator:local th_pool " + this_thread_id + ";mode:ins;status:ok;msg:" + _sql_event.msg + ";",
				_e.what()
			));
		}
	}
}

bool PermissionSyncManager::InsertPermission(const std::string& _group, const std::string& _permis)
{
	auto item = instance.find(_group);
	if (item != instance.end())
	{
		//	插入权限
		item->second->InsertPermission(_permis);
		//	同步权限
		PermissionSyncEvent sync_event(_group, _permis, PermissionSyncEvent::ModeInsert);
		th_pool->Submit(SyncToMySql, sync_event);
	}
	return false;
}

bool PermissionSyncManager::DeletePermission(const std::string& _group, const std::string& _permis)
{
	auto item = instance.find(_group);
	if (item != instance.end())
	{
		//	删除权限
		item->second->DeletePermission(_permis);
		//	同步权限
		PermissionSyncEvent sync_event(_group, _permis, PermissionSyncEvent::ModeDelete);
		th_pool->Submit(SyncToMySql, sync_event);
		return true;
	}
	return false;
}

bool PermissionSyncManager::CreatePermissionGroup(const std::string& _group)
{
	//	临时权限组
	return instance.insert({ _group, std::make_shared<luck::permis::PermissionGroup>(_group) }).second;
}

void PermissionSyncManager::GainFromDatabase(const luck::sql::MySqlAttribute& _attri)
{
	std::unique_ptr<sql::Connection> sql_con(sql::mysql::get_mysql_driver_instance()->connect(_attri.addr, _attri.name, _attri.pswd));
	sql_con->setSchema(_attri.db);
	{
		std::unique_ptr<sql::PreparedStatement> sql_smt_group(sql_con->prepareStatement(R"(select `group`, `permis` from `permis` where `link` is null)"));
		std::unique_ptr<sql::ResultSet> sql_rst(sql_smt_group->executeQuery());

		while (sql_rst->next())
		{
			std::string permis_group = sql_rst->getString(1);
			std::string permis_permis = sql_rst->getString(2);

			auto item = instance.find(permis_group);
			if (item == instance.end())
				item = instance.insert({ permis_group, std::make_shared<luck::permis::PermissionGroup>(permis_group) }).first;

			item->second->InsertPermission(permis_permis);
		}
	}
	{
		std::unique_ptr<sql::PreparedStatement> sql_smt_link(sql_con->prepareStatement(R"(select `group`, `link` from `permis` where `link` is not null)"));
		std::unique_ptr<sql::ResultSet> sql_rst(sql_smt_link->executeQuery());

		while (sql_rst->next())
		{
			std::string permis_group = sql_rst->getString(1);
			std::string permis_link = sql_rst->getString(2);

			auto item = instance.find(permis_group);
			auto item_share = instance.find(permis_link);

			if (item == instance.end() || item_share == instance.end())
				continue;
			item->second->InsertLink(item_share->second);
		}
	}
}

void PermissionSyncManager::SetMySqlThPool(const luck::sql::MySqlThreadPool::SelfShare& _th_pool)
{
	th_pool = _th_pool;
}

PermissionSyncManager::PermissionSyncManager() {}
