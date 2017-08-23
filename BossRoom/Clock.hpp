#pragma once
#include <chrono>

struct Clock {
private:
	double _T = 0;
	double _L = 0;
	bool _flag = false;

public:

	static inline double now(void) {
		return std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now()).time_since_epoch().count() / 1'000'000'000.0;
	}

	double timer = 0.f;

	Clock(double timer = 0.0) : timer(timer), _L(now()) {}
	~Clock() {}

	inline double reset() {
		return _T = 0;
	}
	inline double resume() {
		_L = now();
		_flag = false;
		return _T;
	}
	inline double pause() {
		_T += now() - _L;
		_flag = true;
		return _T;
	}
	inline double restart() {
		const double t = (_flag) ? _T : _T + now() - _L;
		_T = 0;
		resume();
		return t;
	}
	inline double elapsed() const {
		if(_flag)
			return _T;
		else
			return _T + now() - _L;
	}
	inline bool isOver() const {
		return elapsed() >= timer;
	}
	inline bool isPaused() const {
		return _flag;
	}
};
