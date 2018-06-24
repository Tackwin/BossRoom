#pragma once
#include <unordered_map>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <mutex>
#include <cmath>

#include "./../Common.hpp"
#include "./../Time/Clock.hpp"

class TimerManager {
private:
	struct Function {
		using Callback = std::function<bool(double)>;

		Clock clock;

		bool toRemove = false;
		bool paused = false;

		double timer;
		double time;

		double error = 0.;

		Callback f;

		Function() {};
		Function(double timer, const Callback& f)
			: clock(timer), timer(timer), time(timer), f(f) {
		};
		Function(double timer, const Callback&& f)
			: clock(timer), timer(timer), time(timer), f(f) {
		};
	};

	static std::unordered_map<
		std::string, 
		Function
	> _functions;


public:
	static u32 getNFunction();

	static std::string addFunction(
		double timer, const std::string& key, const Function::Callback&& f
	);
	static std::string addFunction(double timer, const Function::Callback&& f);
	static void resetTimerFunction(const std::string& key);
	static void restartFunction(const std::string& key);
	static void removeFunction(const std::string& key);
	static void resumeFunction(const std::string& key);
	static void pauseFunction(const std::string& key);
	static void callFunction(const std::string& key);

	static bool functionsExist(const std::string& key);

	//ASSUREZ VOUS QUE LE POINTEUR RESTE VALIDE TOUT AU LONG DU PROCESSUS !!!!!!
	template<typename T = double>
	static std::string addPowerEase(double t, std::string key, T* v, T min, T max, float p);
	template<typename T = double>
	static std::string addPowerOEase(
		double t, std::string key, T* v, T min, T max, float p
	);
	template<typename T = double>
	static std::string addPowerIOEase(
		double t, std::string key, T* v, T min, T max, float p
	);
	template<typename T = double>
	static std::string addCustomEase(
		double t, std::string key, T* v, T min, T max, std::function<double(double)> f
	);
	template<typename T = double>
	static std::string addLinearEase(double t, std::string key, T* v, T min, T max) {
		return TimerManager::addPowerEase<T>(t, key, v, min, max, 1);
	}
	template<typename T = double>
	static std::string addSquaredEase(double t, std::string key, T* v, T min, T max){
		return TimerManager::addPowerEase<T>(t, key, v, min, max, 2);
	}
	template<typename T = double>
	static std::string addSquaredIOEase(double t, std::string key, T* v, T min, T max){
		return TimerManager::addPowerIOEase<T>(t, key, v, min, max, 2);
	}
	template<typename T = double>
	static std::string addsinEase(double t, std::string key, T* v, T min, T max);
	template<typename T = double>
	static std::string addsinOEase(double t, std::string key, T* v, T min, T max);
	template<typename T = double>
	static std::string addsinIOEase(double t, std::string key, T* v, T min, T max);

	static void update(double dt);
	static void updateInc(double dt, uint32_t n);
};

template<typename T>
std::string TimerManager::addPowerEase(
	double t, std::string key, T* v, T min, T max, float p
) {
	return addCustomEase<T>(t, key, v, min, max, [p](double x)->double {return pow(x, p);});
}
template<typename T>
std::string TimerManager::addPowerOEase(
	double t, std::string key, T* v, T min, T max, float p
) {
	return addCustomEase<T>(
		t, key, v, min, max, [p](double x)->double {return 1 - pow(1-x, p)}
	);
}
template<typename T>
std::string TimerManager::addPowerIOEase(
	double t, std::string key, T* v, T min, T max, float p
) {
	return addCustomEase<T>(t, key, v, min, max, [p](double x)->double {
		return x < 0.5 ? pow(2*x, p) / 2. : 1 - pow(2 * (1 - x), p) / 2.;
	});
}
template<typename T>
std::string TimerManager::addsinEase(double t, std::string key, T* V, T mmin, T max) {
	return TimerManager::addCustomEase<T>(t, key, v, min, max, [](double x)->double {
		return 1 - std::sin(C::PI * (1 - x) / 2.);
	});
}
template<typename T>
std::string TimerManager::addsinOEase(double t, std::string key, T* V, T mmin, T max) {
	return TimerManager::addCustomEase<T>(t, key, v, min, max, [](double x)->double {
		return std::sin(C::PI * x / 2.);
	});
}
template<typename T>
std::string TimerManager::addsinIOEase(double t, std::string key, T* V, T mmin, T max) {
	return TimerManager::addCustomEase<T>(t, key, v, min, max, [](double x)->double {
		return 0.5f * (x < 0.5 
			? (1 - std::sin(C::PI * (1 - 2 * x) / 2.f)) 
			: std::sin((x - 0.5) * C::PI));
	});
}
template<typename T>
std::string TimerManager::addCustomEase(
	double t, std::string key, T* v, T min, T max, std::function<double(double)> pf
) {
	std::shared_ptr<Clock> clock = std::make_shared<Clock>(t);
	auto f = [t, v, min, max, pf, clock, intT = 0.f](double)mutable->bool {
		*v = static_cast<T>(min + (max - min) * pf(clock->elapsed() / t));
		if(clock->isOver()) {
			*v = max;
			return true;
		}
		return false;
	};
	return TimerManager::addFunction(0., key, f);
}

using TM = TimerManager;