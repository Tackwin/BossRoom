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

	static time_point now(void);

	double timer = 0.f;

	Clock(double timer = 0.0);
	~Clock();

	/* restart without the start */
	double reset();
	double pause();
	double resume();
	double restart();
	double elapsed() const;
	bool isOver() const;
	bool isPaused() const;
};
