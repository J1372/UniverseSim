#pragma once
#include <vector>
#include <thread>
#include <functional>
#include <future>
#include <algorithm>
#include <queue>

// A basic thread pool class for easy parallelization.
// 
// Currently unused:
// When compiled with MSVC, execution policies use a thread pool
// and seem to be about 27% faster than this thread pool.
class ThreadPool
{

	std::vector<std::thread> threads;
	std::queue<std::function<void()>> tasks;
	std::mutex task_mutex;
	std::condition_variable cv_task;
	std::condition_variable cv_done;

	int working = 0;
	bool shutdown = false;

	void thread_loop()
	{
		while (true)
		{
			std::function<void()> f;
			// get task
			{
				std::unique_lock lock{ task_mutex };
				cv_task.wait(lock, [this]() { return !tasks.empty() or shutdown; });
				if (!tasks.empty())
				{
					++working;
					f = std::move(tasks.front());
					tasks.pop();
				}
				else if (shutdown)
				{
					return;
				}
			}

			// do task.
			f();

			// end task.
			{
				std::lock_guard lock{ task_mutex };
				--working;
			}

			cv_done.notify_one();
		}
	}

	void wait()
	{
		std::unique_lock lock{ task_mutex };
		cv_done.wait(lock, [this]() { return tasks.empty() and working == 0; });
	}

public:

	ThreadPool(unsigned int size)
	{
		threads.resize(size);
		for (int i = 0 ; i < size; ++i)
		{
			threads[i] = std::thread{ &ThreadPool::thread_loop, this };
		}
	}

	template<typename F, typename InputIterator>
	void for_each(InputIterator a, InputIterator b, F&& f)
	{
		// Make sure given func can actually be called with an object pointed to by iterators.
		static_assert(std::is_invocable_v<F, decltype(*a)>,
			"Provided function can not be called on deref of iterators.");

		auto dist = static_cast<std::size_t>(std::distance(a, b));

		if (dist == 0)
		{
			return;
		}

		auto threads_to_use = std::min(threads.size(), dist);
		auto equal_work = dist / threads_to_use;
		auto remaining = dist % threads_to_use;

		auto it = a;
		for (int i = 0; i < threads_to_use; ++i)
		{
			auto thread_work = equal_work + (i < remaining);
			auto next_it = it + thread_work;
			auto thread_func = [it, next_it, &f]()
			{
				for (auto thread_it = it; thread_it != next_it; ++thread_it)
				{
					std::invoke(f, *thread_it);
				}
			};

			it = next_it;

			{
				std::lock_guard lock{ task_mutex };
				tasks.emplace(std::move(thread_func));
			}

		}
		// tested and was faster than notify_one after creating task.
		cv_task.notify_all();

		wait();
	}

	~ThreadPool()
	{
		{
			std::lock_guard lock{ task_mutex };
			shutdown = true;
		}

		cv_task.notify_all();

		for (std::thread& thread : threads)
		{
			thread.join();
		}
	}

};
