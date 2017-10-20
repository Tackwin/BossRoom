#pragma once

#include <unordered_map>
#include <functional>
#include <stdarg.h>
#include <vector>
#include <any>

class EventManager {
public:
	using EventCallbackParameter = const std::initializer_list<std::any>&;
	using EventCallback = std::function<void(EventCallbackParameter)>;
	
	static uint32_t subscribe(const std::string& key, const EventCallback& callback);
	static void unSubscribe(const std::string& key, uint32_t id);

	static void fire(std::string key, EventCallbackParameter args = {});
private:

	static uint32_t ID;

	static std::unordered_map<
		std::string, 
		std::unordered_map<uint32_t, EventCallback>
	> _callbacks;
};