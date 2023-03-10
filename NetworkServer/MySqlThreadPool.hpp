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
	/// <summary>
	/// 以基础线程池为基础进行拓展。可以提交 mysql 相关的任务，任务的首个参数请务必是 ::sql::Connection，或使用 ::luck::sql::MySqlSubmitFirst。
	/// </summary>
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
		/// <summary>
		/// 根据 MySqlAttribute 初始化 mysql connection，并创建对应 ::std::mutex。
		/// </summary>
		/// <param name="_th_pool">对基础线程池拓展</param>
		/// <param name="_con_data">用于链接 mysql 及选择数据库。</param>
		/// <param name="_sql_con_max">链接 mysql 的最大值。链接数默认是线程池线程数，不会超过最大值。</param>
		MySqlThreadPool(const ThreadPool::SelfShare& _th_pool, const MySqlAttribute& _con_data, const uint64_t& _sql_con_max = std::string::npos) :
			th_pool{ _th_pool }, notwait_pos{ 0 }
		{
			//	确保不会超过最大值
			active_connection = th_pool->ActiveThread() > _sql_con_max ? _sql_con_max : th_pool->ActiveThread();
			//	链接数据库并选择数据库
			for (size_t i = 0; i < active_connection; ++i)
			{
				mysql_connection.emplace_back(::sql::mysql::get_mysql_driver_instance()->connect(_con_data.addr, _con_data.name, _con_data.pswd));
				mysql_connection[i]->setSchema(_con_data.db);
			}
			//	与数据库链接对应的锁
			mysql_connection_mutex = std::vector<std::mutex>(active_connection);
		}
		/// <summary>
		/// 提交一个 mysql 任务
		/// </summary>
		/// <typeparam name="Do">任务类型</typeparam>
		/// <typeparam name="...Args">任务的参数的类型</typeparam>
		/// <param name="_do">任务地址</param>
		/// <param name="..._ag">任务的参数</param>
		/// <returns>无返回值</returns>
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
		/// <summary>
		/// 提交一个 mysql 任务，成员函数特例。
		/// </summary>
		/// <typeparam name="Do">成员函数</typeparam>
		/// <typeparam name="Nt">成员类型</typeparam>
		/// <typeparam name="...Args">成员函数的参数的类型</typeparam>
		/// <param name="_do">成员函数的地址</param>
		/// <param name="_nt">成员的地址</param>
		/// <param name="..._ag">成员函数的参数</param>
		/// <returns>无返回值</returns>
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
		/// <summary>
		/// 将要使用的数据库链接对象的锁上锁，直到获取到锁。请确保在线程池中运行这段代码，！！！请确保结束时释放对应的锁！！！。
		/// </summary>
		/// <returns>对象位置</returns>
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
