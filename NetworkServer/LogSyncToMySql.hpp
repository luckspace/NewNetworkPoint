#pragma once

#include "MySqlThreadPool.hpp"
#include "LuckTool.hpp"

/// <summary>
/// 同步事件的事件数据
/// </summary>
class LogSyncEvent
{
public:
	/// <summary>
	/// 并入时遗留，不清楚给谁用的。
	/// </summary>
	LogSyncEvent() :
		code{ 0 }, timepoint{ 0 }
	{}
	/// <summary>
	/// 自动获取时间戳。
	/// </summary>
	/// <param name="_type">与 mysql `type` 对应</param>
	/// <param name="_code">与 mysql `code` 对应</param>
	/// <param name="_status">与 mysql `status` 对应</param>
	/// <param name="_msg">与 mysql `msg` 对应</param>
	LogSyncEvent(const std::string& _type, const uint64_t& _code, const std::string& _status, const std::string& _msg) :
		type{ _type }, code{ _code }, status{ _status }, msg{ _msg }, timepoint(std::chrono::system_clock::now().time_since_epoch().count())
	{}
	/// <summary>
	/// 插入数据库的所有数据
	/// </summary>
	/// <param name="_type">与 mysql `type` 对应</param>
	/// <param name="_code">与 mysql `code` 对应</param>
	/// <param name="_status">与 mysql `status` 对应</param>
	/// <param name="_msg">与 mysql `msg` 对应</param>
	/// <param name="_timepoint">与 mysql `timepoint` 对应</param>
	LogSyncEvent(const std::string& _type, const uint64_t& _code, const std::string& _status, const std::string& _msg, const uint64_t& _timepoint) :
	type{ _type }, code{ _code }, status{ _status }, msg{ _msg }, timepoint{ _timepoint }
	{}

	std::string type;
	uint64_t	code;
	std::string status;
	std::string msg;
	uint64_t	timepoint;
};
/// <summary>
/// 负责与数据库同步，单例。
/// </summary>
class LogSyncManager
{
public:
	using Self = LogSyncManager;

	using SelfShare = std::shared_ptr<LogSyncManager>;

	friend class Singleton<LogSyncManager>;
	/// <summary>
	/// 将日志写入数据库的实例
	/// </summary>
	/// <param name="_sql_con">数据库的链接对象</param>
	/// <param name="_event">日志事件</param>
	static void SyncToMySql(luck::sql::MySqlSubmitFirst _sql_con, const LogSyncEvent& _event);
	/// <summary>
	/// 提交给线程池的数据库拓展
	/// </summary>
	/// <param name="_event">日志事件</param>
	void Upload(const LogSyncEvent& _event);
	/// <summary>
	/// 设置线程池的数据库拓展
	/// </summary>
	/// <param name="_th_pool">线程池的数据库拓展</param>
	void SetMySqlPool(const luck::sql::MySqlThreadPool::SelfShare& _th_pool);
private:
	LogSyncManager();

	luck::sql::MySqlThreadPool::SelfShare th_pool;
};
