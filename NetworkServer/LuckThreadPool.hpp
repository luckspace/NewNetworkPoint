#pragma once

#include <vector>
#include <queue>
#include <functional>
#include <memory>
//	thread
#include <thread>
#include <mutex>
#include <condition_variable>

#include <atomic>
#include <future>

#include <iostream>

namespace luck
{
	/// <summary>
	/// 基础线程池，支持提交函数、静态函数、成员函数、匿名函数。会忽略所有捕获到的异常，请在任务中捕获。
	/// </summary>
	class ThreadPool
	{
	public:
		using Self = ThreadPool;
		using SelfShare = std::shared_ptr<Self>;

		/// <summary>
		/// 运行指定数目的线程。
		/// </summary>
		/// <param name="_tc">默认为 cpu 的核心数。建议不要超过此数。</param>
		ThreadPool(const size_t& _tc = std::thread::hardware_concurrency()) :
			run_flag(true)
		{
			for (size_t i = 0; i < _tc; ++i) threads.emplace_back(
				[this]
				{
					++active_thread;
					while (run_flag)
					{
						//	条件变量控制
						{
							std::unique_lock<std::mutex> cv_lock(cv_mutex);
							cv.wait(cv_lock);
						}
						//	无任务再结束
						while (true)
						{
							std::function<void()> tk_one{ nullptr };
							{
								std::lock_guard<std::mutex> tk_lock(tk_mutex);
								if (tasks.empty()) break;
								tk_one = std::move(tasks.front());
								tasks.pop();
							}
							//	忽略所有异常
							try { tk_one(); } catch(...) {}
						}
					}
					--active_thread;
				});
			//	确保线程都运行再结束
			while (active_thread != _tc) {}
		}
		/// <summary>
		/// 保证运行完任务并使所有线程退出
		/// </summary>
		~ThreadPool()
		{
			Stop();
			cv.notify_all();

			for (auto& i : threads) i.join();
		}
		/// <summary>
		/// 提交一个无返回值的任务
		/// </summary>
		/// <typeparam name="Do">任务类型</typeparam>
		/// <typeparam name="...Args">任务的参数的类型</typeparam>
		/// <param name="_do">任务地址</param>
		/// <param name="..._ag">任务参数</param>
		/// <returns>无返回值</returns>
		template<typename Do, typename... Args>
		std::enable_if_t<std::is_void_v<std::invoke_result_t<Do, Args...>>, void>
			Submit(Do&& _do, Args&&... _ag)
		{
			auto temp_do = std::bind(std::forward<Do>(_do), std::forward<Args>(_ag)...);
			{
				std::lock_guard<std::mutex> tk_lock(tk_mutex);
				tasks.emplace([temp_do] { temp_do(); });
			}
			cv.notify_one();
		}
		/// <summary>
		/// 提交一个含返回值的任务
		/// </summary>
		/// <typeparam name="Do">任务类型</typeparam>
		/// <typeparam name="...Args">任务的参数的类型</typeparam>
		/// <param name="_do">任务地址</param>
		/// <param name="..._ag">任务参数</param>
		/// <returns>返回值由 future 包装</returns>
		template<typename Do, typename... Args>
		std::future<std::enable_if_t<!std::is_void_v<std::invoke_result_t<Do, Args...>>, std::invoke_result_t<Do, Args...>>>
			Submit(Do&& _do, Args&&... _ag)
		{
			auto package_shared = std::make_shared<std::packaged_task<std::invoke_result_t<Do, Args...>()>>(std::bind(std::forward<Do>(_do), std::forward<Args>(_ag)...));
			{
				std::lock_guard<std::mutex> tk_lock(tk_mutex);
				tasks.emplace([package_shared] { (*package_shared)(); });
			}

			cv.notify_one();
			return package_shared->get_future();
		}
		/// <summary>
		/// 提交一个任务且忽略它的返回值
		/// </summary>
		/// <typeparam name="Do">任务类型</typeparam>
		/// <typeparam name="...Args">任务的参数的类型</typeparam>
		/// <param name="_do">任务地址</param>
		/// <param name="..._ag">任务参数</param>
		template<typename Do, typename... Args>
		void SubmitIgnoreReturn(Do&& _do, Args&&... _ag)
		{
			auto temp_do = std::bind(std::forward<Do>(_do), std::forward<Args>(_ag)...);
			{
				std::lock_guard<std::mutex> tk_lock(tk_mutex);
				tasks.emplace([temp_do] { temp_do(); });
			}
			cv.notify_one();
		}
		/// <summary>
		/// 仅设置 flag 需要再运行全部线程。
		/// </summary>
		inline void Stop() { run_flag = false; }
		/// <summary>
		/// 线程池线程数量
		/// </summary>
		/// <returns>线程数量，注意 unsigned 类型。</returns>
		inline size_t ActiveThread() const { return active_thread; }
	private:
		std::vector<std::thread> threads;
		std::atomic_size_t active_thread;

		std::condition_variable cv;
		std::mutex cv_mutex;

		std::queue<std::function<void()>> tasks;
		std::mutex tk_mutex;

		bool run_flag;
	};
}
