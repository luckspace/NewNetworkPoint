#pragma once

#include "LuckThreadPool.hpp"

#include <mysql/jdbc.h>

namespace luck::sql
{
	/// <summary>
	/// 为 mysql 提供基本的参数
	/// </summary>
	class MySqlAttribute
	{
	public:
		/// <summary>
		/// 为 mysql 提供基本的参数
		/// </summary>
		/// <param name="_addr">数据库地址</param>
		/// <param name="_name">用户名</param>
		/// <param name="_pswd">密码</param>
		MySqlAttribute(const std::string& _addr, const std::string& _name, const std::string& _pswd) :
			addr{ _addr }, name{ _name }, pswd{ _pswd }
		{}
		/// <summary>
		/// 为 mysql 提供基本的参数
		/// </summary>
		/// <param name="_addr">数据库地址</param>
		/// <param name="_name">用户名</param>
		/// <param name="_pswd">密码</param>
		/// <param name="_db">数据库</param>
		MySqlAttribute(const std::string& _addr, const std::string& _name, const std::string& _pswd, const std::string& _db) :
			addr{ _addr }, name{ _name }, pswd{ _pswd }, db{ _db }
		{}

		std::string addr;
		std::string name;
		std::string pswd;
		/// <summary>
		/// permis sync mysql 并入基础线程池时添加
		/// </summary>
		std::string db;
	};

	class MySqlThreadPool
	{
	public:
		using Self = MySqlThreadPool;

		using SelfShare = std::shared_ptr<Self>;

		using MySqlConnection = ::sql::Connection;
		using MySqlConnectionPtr = std::unique_ptr<MySqlConnection>;
		using MySqlConnectionShare = std::shared_ptr<MySqlConnection>;
		using MySqlConnectionPointer = MySqlConnection*;

		using MySqlConnectionPtrVec = std::vector<MySqlConnectionPtr>;

		MySqlThreadPool(const MySqlThreadPool&)	= delete;
		MySqlThreadPool(MySqlThreadPool&&)		= delete;

		MySqlThreadPool(const ThreadPool::SelfShare& _th_pool, const MySqlAttribute& _con_data, const uint64_t& _sql_con_max = std::string::npos) :
			th_pool{ _th_pool }, notwait_pos{ 0 }
		{
			active_connection = th_pool->ActiveThread() > _sql_con_max ? _sql_con_max : th_pool->ActiveThread();

			for (size_t i = 0; i < active_connection; ++i)
			{
				mysql_connection.emplace_back(::sql::mysql::get_mysql_driver_instance()->connect(_con_data.addr, _con_data.name, _con_data.pswd));
				mysql_connection[i]->setSchema(_con_data.db);
			}
			mysql_connection_mutex = std::vector<std::mutex>(active_connection);
		}

		~MySqlThreadPool()
		{

		}

		template<typename Do, typename... Args>
		std::invoke_result_t<Do, MySqlConnectionPointer, Args...>
			Submit(Do&& _do, Args&&... _ag)
		{
			auto bind_temp = std::bind(std::forward<Do>(_do), std::placeholders::_1, std::forward<Args>(_ag)...);
			th_pool->SubmitIgnoreReturn(
				[this, bind_temp]()
				{
					size_t mutex_pos = GainWaitMutex();
					{
						bind_temp(mysql_connection[mutex_pos].get());
					}
					mysql_connection_mutex[mutex_pos].unlock();
				}
			);
		}

		template<typename Do, typename Nt, typename... Args>
		std::invoke_result_t<Do, Nt, MySqlConnectionPointer, Args...>
			Submit(Do&& _do, Nt&& _nt, Args&&... _ag)
		{
			auto bind_temp = std::bind(std::forward<Do>(_do), std::forward<Nt>(_nt), std::placeholders::_1, std::forward<Args>(_ag)...);
			th_pool->SubmitIgnoreReturn(
				[this, bind_temp]()
				{
					size_t mutex_pos = GainWaitMutex();
					{
						bind_temp(mysql_connection[mutex_pos].get());
					}
					mysql_connection_mutex[mutex_pos].unlock();
				}
			);
		}
	private:
		size_t GainWaitMutex()
		{
			for (size_t i = 0; i < active_connection; ++i)
			{
				if (mysql_connection_mutex[i].try_lock())
					return i;
			}
			size_t pos = ++notwait_pos;
			pos %= active_connection;
			mysql_connection_mutex[pos].lock();
			return pos;
		}
	private:
		ThreadPool::SelfShare th_pool;

		MySqlConnectionPtrVec mysql_connection;
		std::vector<std::mutex> mysql_connection_mutex;
		size_t active_connection;
		std::atomic_size_t notwait_pos;
	};

	using MySqlSubmitFirst = MySqlThreadPool::MySqlConnectionPointer;
}
