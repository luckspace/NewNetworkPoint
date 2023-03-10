#pragma once

#include "MySqlThreadPool.hpp"
#include "LuckTool.hpp"

class LogSyncEvent
{
public:

	LogSyncEvent() :
		code{ 0 }, timepoint{ 0 }
	{}

	LogSyncEvent(const std::string& _type, const uint64_t& _code, const std::string& _status, const std::string& _msg) :
		type{ _type }, code{ _code }, status{ _status }, msg{ _msg }, timepoint(std::chrono::system_clock::now().time_since_epoch().count())
	{}

	LogSyncEvent(const std::string& _type, const uint64_t& _code, const std::string& _status, const std::string& _msg, const uint64_t& _timepoint) :
	type{ _type }, code{ _code }, status{ _status }, msg{ _msg }, timepoint{ timepoint }
	{}

	std::string type;
	uint64_t	code;
	std::string status;
	std::string msg;
	uint64_t	timepoint;
};

class LogSyncManager
{
public:
	friend class Singleton<LogSyncManager>;

	static void SyncToMySql(luck::sql::MySqlSubmitFirst _sql_con, const LogSyncEvent& _event);

	void Upload(const LogSyncEvent& _event);
private:
	LogSyncManager();

	luck::sql::MySqlThreadPool::SelfShare th_pool;
};
