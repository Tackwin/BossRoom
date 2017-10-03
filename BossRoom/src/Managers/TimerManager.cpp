#include <cassert>
#include <string>

#include "Managers/TimerManager.hpp"
#include "Global/Clock.hpp"

bool TimerManager::INCREMENTAL = false;

TimerManager::TimerManager() {}
TimerManager::~TimerManager() {}

std::string TimerManager::addFunction(double timer, const std::string &key, const Function::Callback &f) {
	uint8_t i = (uint8_t)rand();
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
	it->second->toRemove = it->second->f(0);
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

void TimerManager::update(double dt) {
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
			
			if (f->clock.timer == 0.) {
				f->toRemove = f->f(dt);
			}
			else { // i should get rid of all these branching in the *MAIN* loop
				for (f->error += f->clock.elapsed(); !f->toRemove && f->clock.timer < f->error; f->error -= f->clock.timer) {
					f->toRemove = f->f(fmin(dt, f->clock.timer));
				}
			}


			f->clock.restart();
		}
		++it;
	}
}
