#pragma once
#include <map>
#include <memory>
#include <functional>

#include "Global/Clock.hpp"

struct Function;
class TimerManager {
private:
	static std::map < std::string, std::shared_ptr<Function>> _functions;

private:
	TimerManager();
	~TimerManager();

public:
	static bool INCREMENTAL;

	static std::string addFunction(float timer, const std::string& key, const std::function<bool(float)>& f);
	static std::string cloneFunction(const std::string& key);
	static void resetTimerFunction(const std::string& key);
	static void restartFunction(const std::string& key);
	static void removeFunction(const std::string& key);
	static void resumeFunction(const std::string& key);
	static void pauseFunction(const std::string& key);
	static void callFunction(const std::string& key);

	static bool functionsExist(const std::string& key);

	//ASSUREZ VOUS QUE LE POINTEUR RESTE VALIDE TOUT AU LONG DU PROCESSUS !!!!!!
	template<typename T = float>
	static std::string addPowerEase(float t, std::string key, T* v, T min, T max, float p);
	template<typename T = float>
	static std::string addPowerOEase(float t, std::string key, T* v, T min, T max, float p);
	template<typename T = float>
	static std::string addPowerIOEase(float t, std::string key, T* v, T min, T max, float p);
	template<typename T = float>
	static std::string addCustomEase(float t, std::string key, T* v, T min, T max, std::function<float(float)> f);
	template<typename T = float>
	static std::string addLinearEase(float t, std::string key, T* v, T min, T max) {return TimerManager::addPowerEase<T>(t, key, v, min, max, 1);}
	template<typename T = float>
	static std::string addSquaredEase(float t, std::string key, T* v, T min, T max){return TimerManager::addPowerEase<T>(t, key, v, min, max, 2);}
	template<typename T = float>
	static std::string addSquaredIOEase(float t, std::string key, T* v, T min, T max){return TimerManager::addPowerIOEase<T>(t, key, v, min, max, 2);}
	template<typename T = float>
	static std::string addSinEase(float t, std::string key, T* v, T min, T max);
	template<typename T = float>
	static std::string addSinOEase(float t, std::string key, T* v, T min, T max);
	template<typename T = float>
	static std::string addSinIOEase(float t, std::string key, T* v, T min, T max);

	static void update(float dt);
};

template<typename T>
std::string TimerManager::addPowerEase(float t, std::string key, T* v, T min, T max, float p) {
	return addCustomEase<T>(t, key, v, min, max, [p](float x)->float {return pow(x, p);});
}
template<typename T>
std::string TimerManager::addPowerOEase(float t, std::string key, T* v, T min, T max, float p) {
	return addCustomEase<T>(t, key, v, min, max, [p](float x)->float {return 1 - pow(1-x, p)});
}
template<typename T>
std::string TimerManager::addPowerIOEase(float t, std::string key, T* v, T min, T max, float p) {
	return addCustomEase<T>(t, key, v, min, max, [p](float x)->float {
		return x < 0.5 ? pow(2*x, p) / 2.f : 1 - pow(2 * (1 - x), p) / 2.f;
	});
}
template<typename T>
std::string TimerManager::addSinEase(float t, std::string key, T* V, T mmin, T max) {
	return TimerManager::addCustomEase<T>(t, key, v, min, max, [](float x)->float {
		return 1 - sin(C::PI * (1 - x) / 2.f);
	});
}
template<typename T>
std::string TimerManager::addSinOEase(float t, std::string key, T* V, T mmin, T max) {
	return TimerManager::addCustomEase<T>(t, key, v, min, max, [](float x)->float {
		return sin(C::PI * x / 2.f);
	});
}
template<typename T>
std::string TimerManager::addSinIOEase(float t, std::string key, T* V, T mmin, T max) {
	return TimerManager::addCustomEase<T>(t, key, v, min, max, [](float x)->float {
		return (x < 0.5 ? (1 - sin(C::PI * (1 - 2 * x) / 2.f)) : sin((x - 0.5) * C::PI)) / 2.f;
	});
}
template<typename T>
std::string TimerManager::addCustomEase(float t, std::string key, T* v, T min, T max, std::function<float(float)> pf) {
	std::shared_ptr<Clock> clock = std::make_shared<Clock>(t);
	auto f = [t, v, min, max, pf, clock, intT = 0.f](float)mutable->bool {
		*v = static_cast<T>(min + (max - min) * pf((float)clock->elapsed() / t));
		if(clock->isOver()) {
			*v = max;
			return true;
		}
		return false;
	};
	return TimerManager::addFunction(0.f, key, f);
}

