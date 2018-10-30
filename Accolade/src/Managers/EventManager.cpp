#include "EventManager.hpp"

std::unordered_map<
	std::string,
	std::unordered_map<UUID, EventManager::EventCallback>
> EventManager::_callbacks;

std::vector<std::unordered_map<UUID, EM::EventCallback>>
EM::callbacks{ (size_t)EM::EventType::Count };

UUID EventManager::subscribe(const std::string& key, const EventCallback& callback) {
	auto& map = _callbacks[key];
	UUID uuid;
	map[uuid] = callback;
	return uuid;
}
UUID EventManager::subscribe(
	const EventManager::EventType& type, const EventCallback& callback
) {
	auto& map = callbacks[(size_t)type];
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
void EventManager::unSubscribe(const EM::EventType& key, UUID id) {
	auto& map = callbacks[(size_t)key];

	const auto& it = map.find(id);
	assert(it != map.end());

	map.erase(it);
}


void EventManager::fire(std::string key, EventCallbackParameter args) {
	const auto& it = _callbacks.find(key);
	if (it == _callbacks.end())
		return;

	auto& map = it->second;
	for (auto&[_, i] : map) {
		_;
		i(args);
	}
}

void EventManager::fire(const EM::EventType& key, const EventCallbackParameter& args) {
	auto& map = callbacks[(size_t)key];
	for (auto&[_, i] : map) {
		_;
		i(args);
	}
}
