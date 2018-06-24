#include "EventManager.hpp"

std::unordered_map<std::string, std::unordered_map<u32, EventManager::EventCallback>> EventManager::_callbacks;

u32 EventManager::subscribe(const std::string& key, const EventCallback& callback) {
	auto& map = _callbacks[key];

	u32 id = 0u;
	bool flag = false;
	do {
		flag = false;
		for (auto& it : map) {
			if (it.first == id) {
				id++;
				flag = true;
				break;
			}
		}

	} while (flag);
	
	map[id] = callback;
	return id;
}
void EventManager::unSubscribe(const std::string& key, u32 id) {
	if (const auto& it = _callbacks.find(key); it == _callbacks.end())
		return;

	auto& map = _callbacks[key];

	const auto& it = map.find(id);
	if (it == map.end())
		return;

	map.erase(it);
}
void EventManager::fire(std::string key, EventCallbackParameter args) {
	const auto& it = _callbacks.find(key);
	if (it == _callbacks.end())
		return;

	auto& map = it->second;
	for (auto& [_, i] : map) {_;
		i(args);
	}
}
