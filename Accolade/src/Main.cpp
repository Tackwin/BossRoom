#include <thread>
#include <iostream>

#include "Time/Clock.hpp"

int main(int, char**) {
	using namespace std::chrono_literals;

	Clock b;
	for (uint32_t i = 0u; i < 100'000; ++i) {
		/*double time[5];

		a.restart();
		//std::this_thread::sleep_for(100ms);
		time[0] = a.pause();
		//std::this_thread::sleep_for(57ms);
		time[1] = a.resume();
		//std::this_thread::sleep_for(150ms);
		time[2] = a.pause();
		//std::this_thread::sleep_for(456ms);
		time[3] = a.resume();
		//std::this_thread::sleep_for(500ms);
		time[4] = a.elapsed();
		*/
		for (uint32_t j = 0; j < 500; ++j) {
			Clock::now();
			//printf("%lf\n", time[j]);
		}
	}
	/*
	for (uint32_t j = 0u; j < 1'000; ++j) {
		Clock a;
		for (uint32_t i = 0; i < 1'000'000; ++i) {
			a.now();
		}
		std::cout << a.elapsed() << std::endl;
	}
	*/
	std::cout << 1e9 * b.elapsed() / (500 * 100'000) << std::endl;
	std::cin.get();
	return 0;
}