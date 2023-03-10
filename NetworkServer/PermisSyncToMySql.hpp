#pragma once

#include "MySqlThreadPool.hpp"
#include "LuckTool.hpp"
#include "LuckPermis.hpp"

class PermissionSyncEvent
{
public:
	enum SyncMode
	{
		ModeEmpty = 0,
		ModeDelete = 1,
		ModeInsert = 3,

		ModeDeleteMask = 1,
		ModeInsertMask = 2,
	};

	PermissionSyncEvent() : mode(ModeEmpty) {}

	PermissionSyncEvent(const std::string& _group, const std::string& _str, SyncMode _mode) : group(_group), msg(_str), mode(_mode) {}

	SyncMode mode;
	std::string group;
	std::string msg;
};

class PermissionSyncManager
{
public:
	friend class Singleton<PermissionSyncManager>;

	static void SyncToMySql(luck::sql::MySqlThreadPool::MySqlConnectionPointer _sql_con, PermissionSyncEvent _sql_event);

	bool InsertPermission(const std::string& _group, const std::string& _permis);

	bool DeletePermission(const std::string& _group, const std::string& _permis);

	bool CreatePermissionGroup(const std::string& _group);

	void GainFromDatabase(const luck::sql::MySqlAttribute& _attri);

	void SetMySqlThPool(const luck::sql::MySqlThreadPool::SelfShare& _th_pool);
	
	void Point()
	{
		th_pool->Submit(&PermissionSyncManager::Submit, this, 100);
	}

	void Submit(luck::sql::MySqlSubmitFirst _sql_con, int a)
	{

	}

private:
	PermissionSyncManager();

	luck::permis::PermissionGroup::StrMapSelf instance;

	luck::sql::MySqlThreadPool::SelfShare th_pool;
};
