#include "EventManager.hpp"

std::unordered_map<
	std::string,
	std::unordered_map<UUID, EventManager::EventCallback>
> EventManager::_callbacks;

UUID EventManager::subscribe(const std::string& key, const EventCallback& callback) {
	auto& map = _callbacks[key];
	UUID uuid;
	map[uuid] = callback;
	return uuid;
}
void EventManager::unSubscribe(const std::string& key, UUID id) {
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
