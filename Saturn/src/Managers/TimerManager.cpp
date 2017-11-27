#include "Managers/TimerManager.hpp"

std::vector<TM::Thread> TimerManager::_threads;
std::mutex TimerManager::_mutex;

std::thread::id TimerManager::set_timeout(
	const Function&& f, double timeout
) {
	Thread t;
	t.t = std::thread([f, timeout]() -> void {
		std::this_thread::sleep_for(std::chrono::duration<double>(timeout));

		TM::lock();
		if (TM::is_running(std::this_thread::get_id())) {
			TM::unlock();
			f();
		}
		else {
			TM::unlock();
		}
	});
	t.t.detach();
	_threads.emplace_back(std::move(t));
	return t.t.get_id();
}

std::thread::id TimerManager::set_interval(
	const Function&& f, double time
) {
	_threads.emplace_back();
	_threads.back().t = std::thread([f, time]() -> void {
		bool terminating = false;
		do {
			std::this_thread::sleep_for(std::chrono::duration<double>(time));

			TM::lock();
			terminating = TM::is_terminating(std::this_thread::get_id());
			if (TM::is_running(std::this_thread::get_id())) {
				TM::unlock();
				f();
			}
			else {
				TM::unlock();
			}

		} while (!terminating);
	});
	_threads.back().t.detach();
	return _threads.back().t.get_id();
}

void TimerManager::set_running(const std::thread::id& id, bool running) {
	const auto& it = std::find_if(_threads.begin(), _threads.end(), 
		[id](const Thread& A) -> bool {
			return A.t.get_id() == id;
		}
	);
	it->run = running;
}	
void TimerManager::set_terminating(const std::thread::id& id, bool terminating) {
	const auto& it = std::find_if(_threads.begin(), _threads.end(),
		[id](const Thread& A) -> bool {
			return A.t.get_id() == id;
		}
	);
	it->terminate = terminating;
}

bool TimerManager::is_running(const std::thread::id& id) {
	const auto& it = std::find_if(_threads.begin(), _threads.end(),
		[id](const Thread& A) -> bool {
			return A.t.get_id() == id;
		}
	);
	return it->run;
}
bool TimerManager::is_terminating(const std::thread::id& id) {
	const auto& it = std::find_if(_threads.begin(), _threads.end(),
		[id](const Thread& A) -> bool {
			return A.t.get_id() == id;
		}
	);
	return it->terminate;
}

void TimerManager::lock() {
	_mutex.lock();
}
void TimerManager::unlock() {
	_mutex.unlock();
}