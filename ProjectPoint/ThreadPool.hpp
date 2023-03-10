#pragma once
#ifndef __LP_THREAD_POOL_HPP__
#define __LP_THREAD_POOL_HPP__

#include <thread>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <future>
#include <vector>
#include <memory>
#include <functional>
#include <queue>

namespace LuckSpace
{
	class ThreadPool
	{
	public:
		ThreadPool(const size_t& _tc = std::thread::hardware_concurrency()) :
			run_flag(true)
		{
			for (size_t i = 0; i < _tc; ++i) threads.emplace_back(
				[this]
				{
					++active_thread;
					while (run_flag)
					{
						{
							std::unique_lock<std::mutex> cv_lock(cv_mutex);
							cv.wait(cv_lock);
						}
						while (true)
						{
							std::function<void()> tk_one{ nullptr };
							{
								std::lock_guard<std::mutex> tk_lock(tk_mutex);
								if (tasks.empty()) break;
								tk_one = std::move(tasks.front());
								tasks.pop();
							}
							tk_one();
						}
					}
					--active_thread;
				});
			while (active_thread != _tc) {}
		}

		~ThreadPool()
		{
			Stop();
			cv.notify_all();

			for (auto& i : threads) i.join();
		}

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

		inline void Stop() { run_flag = false; }
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

#endif
