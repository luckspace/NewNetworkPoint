#include "LogSyncToMySql.hpp"

void LogSyncManager::SyncToMySql(luck::sql::MySqlSubmitFirst _sql_con, const LogSyncEvent& _event)
{
	std::unique_ptr<sql::PreparedStatement> sql_smt(_sql_con->prepareStatement(R"(insert into `aerfa_log` (`type`, `code`, `status`, `msg`, `timepoint`) values (?, ?, ?, ?, ?))"));
	sql_smt->setString(1, _event.type);
	sql_smt->setUInt64(2, _event.code);
	sql_smt->setString(3, _event.status);
	sql_smt->setString(4, _event.msg);
	sql_smt->setUInt64(5, _event.timepoint);
	sql_smt->execute();
}

void LogSyncManager::Upload(const LogSyncEvent& _event)
{
	th_pool->Submit(LogSyncManager::SyncToMySql, _event);
}

void LogSyncManager::SetMySqlPool(const luck::sql::MySqlThreadPool::SelfShare& _th_pool)
{
	th_pool = _th_pool;
}

LogSyncManager::LogSyncManager() {}
