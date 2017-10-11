#pragma once
#include <chrono>

struct Clock {
private:
	using time_point = decltype(std::chrono::system_clock::now());

	time_point _lastPause;
	time_point _start;

	double _sumPause = 0;

	bool _flag = false;

public:

	static time_point now(void) {
		const auto& nowTimePoint = std::chrono::system_clock::now();
		return nowTimePoint;
	}

	double timer = 0.f;

	Clock(double timer = 0.0) : timer(timer), _start(now()) {

	}
	~Clock() {
	}

	/* restart without the start */
	inline double reset() { 
		const auto& e = elapsed();
		
		_start = now();
		_sumPause = 0;
		
		return e;
	}
	inline double pause() {
		if (_flag) return elapsed();
		
		_lastPause = now();
		_flag = true;
		
		return elapsed();
	}
	inline double resume() {
		if (!_flag) return elapsed();

		_sumPause += std::chrono::duration<double, std::ratio<1, 1>>(now() - _lastPause).count();
		_flag = false;

		return elapsed();
	}
	inline double restart() {
		const auto& e = elapsed();

		_start = now();
		_sumPause = 0;
		_flag = false;

		return e;
	}
	inline double elapsed() const {
		if (_flag) {
			// (_n - _s) - (_n - _l) - _sum == _n - _s -_n + _l -_sum == _l - (_s + _sum)
			return (std::chrono::duration<double, std::ratio<1, 1>>(_lastPause - _start).count() - _sumPause);
		} 
			return (std::chrono::duration<double, std::ratio<1, 1>>(now() - _start).count() - _sumPause);
	}
	inline bool isOver() const {
		return elapsed() >= timer;
	}
	inline bool isPaused() const {
		return _flag;
	}
};
