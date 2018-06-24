#include "Clock.hpp"

Clock::time_point Clock::now(void) {
	const auto& nowTimePoint = std::chrono::system_clock::now();
	return nowTimePoint;
}

Clock::Clock(double timer) : timer(timer), _start(now()) {
}
Clock::~Clock() {
}

/* restart without the start */
double Clock::reset() {
	const auto& e = elapsed();

	_start = now();
	_sumPause = 0;

	return e;
}
double Clock::pause() {
	if (_flag) return elapsed();

	_lastPause = now();
	_flag = true;

	return elapsed();
}
double Clock::resume() {
	if (!_flag) return elapsed();

	_sumPause += std::chrono::duration<double, std::ratio<1, 1>>(now() - _lastPause).count();
	_flag = false;

	return elapsed();
}
double Clock::restart() {
	const auto& e = elapsed();

	_start = now();
	_sumPause = 0;
	_flag = false;

	return e;
}
double Clock::elapsed() const {
	if (_flag) {
		// (_n - _s) - (_n - _l) - _sum == _n - _s -_n + _l -_sum == _l - (_s + _sum)
		return (std::chrono::duration<double, std::ratio<1, 1>>(_lastPause - _start).count() - _sumPause);
	}
	return (std::chrono::duration<double, std::ratio<1, 1>>(now() - _start).count() - _sumPause);
}
bool Clock::isOver() const {
	return elapsed() >= timer;
}
bool Clock::isPaused() const {
	return _flag;
}