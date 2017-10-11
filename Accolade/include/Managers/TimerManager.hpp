#pragma once
#include <unordered_map>
#include <functional>
#include <memory>

#include "Time/Clock.hpp"

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

	static std::unordered_map<std::string, std::shared_ptr<Function>> _functions;

public:
	static std::string addFunction(double timer, const std::string& key, const Function::Callback&& f);
	static std::string addFunction(double timer, const std::string& key, const Function::Callback& f);
	static std::string cloneFunction(const std::string& key);
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
	static std::string addPowerOEase(double t, std::string key, T* v, T min, T max, float p);
	template<typename T = double>
	static std::string addPowerIOEase(double t, std::string key, T* v, T min, T max, float p);
	template<typename T = double>
	static std::string addCustomEase(double t, std::string key, T* v, T min, T max, std::function<double(double)> f);
	template<typename T = double>
	static std::string addLinearEase(double t, std::string key, T* v, T min, T max) {return TimerManager::addPowerEase<T>(t, key, v, min, max, 1);}
	template<typename T = double>
	static std::string addSquaredEase(double t, std::string key, T* v, T min, T max){return TimerManager::addPowerEase<T>(t, key, v, min, max, 2);}
	template<typename T = double>
	static std::string addSquaredIOEase(double t, std::string key, T* v, T min, T max){return TimerManager::addPowerIOEase<T>(t, key, v, min, max, 2);}
	template<typename T = double>
	static std::string addSinEase(double t, std::string key, T* v, T min, T max);
	template<typename T = double>
	static std::string addSinOEase(double t, std::string key, T* v, T min, T max);
	template<typename T = double>
	static std::string addSinIOEase(double t, std::string key, T* v, T min, T max);

	static void update(double dt);
};

template<typename T>
std::string TimerManager::addPowerEase(double t, std::string key, T* v, T min, T max, float p) {
	return addCustomEase<T>(t, key, v, min, max, [p](double x)->double {return pow(x, p);});
}
template<typename T>
std::string TimerManager::addPowerOEase(double t, std::string key, T* v, T min, T max, float p) {
	return addCustomEase<T>(t, key, v, min, max, [p](double x)->double {return 1 - pow(1-x, p)});
}
template<typename T>
std::string TimerManager::addPowerIOEase(double t, std::string key, T* v, T min, T max, float p) {
	return addCustomEase<T>(t, key, v, min, max, [p](double x)->double {
		return x < 0.5 ? pow(2*x, p) / 2. : 1 - pow(2 * (1 - x), p) / 2.;
	});
}
template<typename T>
std::string TimerManager::addSinEase(double t, std::string key, T* V, T mmin, T max) {
	return TimerManager::addCustomEase<T>(t, key, v, min, max, [](double x)->double {
		return 1 - sin(C::PI * (1 - x) / 2.);
	});
}
template<typename T>
std::string TimerManager::addSinOEase(double t, std::string key, T* V, T mmin, T max) {
	return TimerManager::addCustomEase<T>(t, key, v, min, max, [](double x)->double {
		return sin(C::PI * x / 2.);
	});
}
template<typename T>
std::string TimerManager::addSinIOEase(double t, std::string key, T* V, T mmin, T max) {
	return TimerManager::addCustomEase<T>(t, key, v, min, max, [](double x)->double {
		return (x < 0.5 ? (1 - sin(C::PI * (1 - 2 * x) / 2.f)) : sin((x - 0.5) * C::PI)) / 2.f;
	});
}
template<typename T>
std::string TimerManager::addCustomEase(double t, std::string key, T* v, T min, T max, std::function<double(double)> pf) {
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

