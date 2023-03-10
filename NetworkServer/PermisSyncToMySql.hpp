#pragma once

#include "MySqlThreadPool.hpp"
#include "LuckTool.hpp"
#include "LuckPermis.hpp"
/// <summary>
/// 权限同步事件的事件数据
/// </summary>
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
	/// <summary>
	/// 空事件
	/// </summary>
	PermissionSyncEvent() : mode(ModeEmpty) {}
	/// <summary>
	/// 
	/// </summary>
	/// <param name="_group">权限组</param>
	/// <param name="_str">权限</param>
	/// <param name="_mode">进行的操作</param>
	PermissionSyncEvent(const std::string& _group, const std::string& _str, SyncMode _mode) : group(_group), msg(_str), mode(_mode) {}

	SyncMode mode;
	std::string group;
	std::string msg;
};
/// <summary>
/// 负责将权限和数据库同步，单例。
/// </summary>
class PermissionSyncManager
{
public:
	friend class Singleton<PermissionSyncManager>;
	/// <summary>
	/// 将权限与数据库同步的实例
	/// </summary>
	/// <param name="_sql_con">线程池的数据库拓展对象</param>
	/// <param name="_sql_event">权限同步事件</param>
	static void SyncToMySql(luck::sql::MySqlThreadPool::MySqlConnectionPointer _sql_con, PermissionSyncEvent _sql_event);
	/// <summary>
	/// 向 _group 权限组插入 _permis 权限
	/// </summary>
	/// <param name="_group">权限组</param>
	/// <param name="_permis">权限</param>
	/// <returns>如果权限组不存在则失败</returns>
	bool InsertPermission(const std::string& _group, const std::string& _permis);
	/// <summary>
	/// 向 _group 权限组删除 _permis 权限
	/// </summary>
	/// <param name="_group">权限组</param>
	/// <param name="_permis">权限</param>
	/// <returns>如果权限组不存在则失败</returns>
	bool DeletePermission(const std::string& _group, const std::string& _permis);
	/// <summary>
	/// 创建一个临时权限组，若要永久话还需对临时权限组写入权限。
	/// </summary>
	/// <param name="_group">权限组</param>
	/// <returns>权限组若存在则失败</returns>
	bool CreatePermissionGroup(const std::string& _group);
	/// <summary>
	/// 根据给定参数链接数据库读取数据进行权限数据加载。
	/// </summary>
	/// <param name="_attri">数据库参数</param>
	void GainFromDatabase(const luck::sql::MySqlAttribute& _attri);
	/// <summary>
	/// 设置线程池数据库拓展
	/// </summary>
	/// <param name="_th_pool">线程池数据库拓展</param>
	void SetMySqlThPool(const luck::sql::MySqlThreadPool::SelfShare& _th_pool);
private:
	PermissionSyncManager();

	luck::permis::PermissionGroup::StrMapSelf instance;

	luck::sql::MySqlThreadPool::SelfShare th_pool;
};
