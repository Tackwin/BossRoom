#include "Managers/EventManager.hpp"

std::unordered_map<std::string, std::vector<EventManager::EventCallback>> EventManager::_callbacks;

void EventManager::subscribe(const std::string& key, const EventCallback& callback) {
	_callbacks[key].push_back(callback);
}
void EventManager::unSuscribe(const std::string&, const EventCallback&) {
	/*if (const auto& it = _callbacks.find(key); it == _callbacks.end())
		return;

	auto& vector = _callbacks[key];

	const auto& it = std::find(vector.begin(), vector.end(), callback);
	if (it == vector.end())
		return;

	vector.erase(it);*/
}
void EventManager::fire(const std::string& key, uint32_t n, ...) {
	const auto& it = _callbacks.find(key);
	if (it == _callbacks.end())
		return;

	va_list args;
	va_start(args, n);

	auto& vector = it->second;
	for (auto& i : vector) {
		i(n, args);
	}

	va_end(args);
}
