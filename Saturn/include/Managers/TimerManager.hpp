#pragma once

#include <mutex>
#include <future>
#include <thread>
#include <chrono>
#include <memory>
#include <functional>

#include "MemoryManager.hpp"

class TimerManager {
public:
	using Function = std::function<bool(void)>;

	static std::thread::id set_timeout(const Function&& f, double timeout);
	static std::thread::id set_interval(const Function&& f, double time);

	static void set_running(const std::thread::id& id, bool running = false);
	static void set_terminating(const std::thread::id& id, bool terminating = true);

	static bool is_running(const std::thread::id& id);
	static bool is_terminating(const std::thread::id& id);

	static void lock();
	static void unlock();
private:

	TimerManager() = delete;
	TimerManager(const TimerManager&) = delete;
	TimerManager(const TimerManager&&) = delete;

	TimerManager& operator=(const TimerManager&) = delete;
	TimerManager& operator=(const TimerManager&&) = delete;

	static std::mutex _mutex;

	struct Thread {
		std::thread t;

		bool run = true;
		bool terminate = false;
	};

	static std::vector<Thread> _threads;
};

using TM = TimerManager;