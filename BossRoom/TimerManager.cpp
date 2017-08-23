#include <cassert>
#include <string>

#include "TimerManager.hpp"
#include "Clock.hpp"

struct Function {
	Clock clock;

	bool toRemove = false;
	bool paused = false;

	float timer;
	float time;
	
	std::function<bool(float)> f;

	Function() {};
	Function(float timer, const std::function<bool(float)> &f)
		: clock(timer), timer(timer), time(timer), f(f) {

	};
};

bool TimerManager::INCREMENTAL = false;

TimerManager::TimerManager() {}
TimerManager::~TimerManager() {}

std::string TimerManager::addFunction(float timer, const std::string &key, const std::function<bool(float)> &f) {
	unsigned char i = (unsigned char)(rand() % (1 << 8));
	std::string candidate = key + std::to_string(i);
	while(_functions.find(candidate) != _functions.end()) {
		candidate = candidate + std::to_string(i);
	}
	_functions[candidate] = std::make_shared<Function>(timer, f);
	return candidate;
}
void TimerManager::resetTimerFunction(const std::string &key) {
	auto it = _functions.find(key);
	assert(it != std::end(_functions) && "Can't find the function");
	it->second->clock.reset();
	it->second->paused = false;
}
void TimerManager::restartFunction(const std::string &key) {
	auto it = _functions.find(key);
	assert(it != std::end(_functions) && "Can't find the function");
	it->second->clock.restart();
	it->second->paused = false;
}
void TimerManager::callFunction(const std::string &key) {
	auto it = _functions.find(key);
	assert(it != std::end(_functions) && "Can't find the function");
	it->second->f(0);
}
bool TimerManager::functionsExist(const std::string & key) {
	return _functions.find(key) != std::end(_functions);
}
void TimerManager::removeFunction(const std::string &key) {
	auto it = _functions.find(key);
	assert(it != std::end(_functions) && "Can't find the function");
	it->second->toRemove = true;
}
void TimerManager::pauseFunction(const std::string &key) {
	auto it = _functions.find(key);
	assert(it != std::end(_functions) && "Can't find the function");
	it->second->clock.pause();
	it->second->paused = true;
}
void TimerManager::resumeFunction(const std::string &key) {
	auto it = _functions.find(key);
	assert(it != std::end(_functions) && "Can't find the function");
	it->second->clock.resume();
	it->second->paused = false;
}
std::string TimerManager::cloneFunction(const std::string &key) {
	auto it = _functions.find(key);
	assert(it != std::end(_functions) && "Can't find the function");
	std::string candidate = key; 
	static unsigned int i;
	i = 0;
	while (_functions.find(candidate) != _functions.end()) {
		candidate = key + std::to_string(i++);
	}
	_functions[candidate] = std::make_shared<Function>(static_cast<float>(_functions[key]->clock.timer), _functions[key]->f);
	return candidate;
}

void TimerManager::update(float dt) {
	for(auto it = std::begin(_functions); it != std::end(_functions);){
		if(!it->second->paused)
			it->second->time -= dt;

		auto &f = it->second;
		if(f->toRemove) {
			_functions.erase((it++)->first);
			continue;
		}
		if((!INCREMENTAL && f->clock.isPaused()) || (INCREMENTAL && it->second->paused)) {
			++it;
			continue;
		}
		if((!INCREMENTAL && f->clock.isOver()) || (INCREMENTAL && it->second->time <= 0)) {
			it->second->time = it->second->timer;
			f->toRemove = f->f(dt);
			f->clock.restart();
		}
		++it;
	}
}
