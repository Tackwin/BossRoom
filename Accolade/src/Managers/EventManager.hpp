#pragma once

#include <unordered_map>
#include <functional>
#include <stdarg.h>
#include <vector>
#include <any>

#include "./../Common.hpp"
#include "./../Utils/UUID.hpp"

class EventManager {
public:
	using EventCallbackParameter = const std::initializer_list<std::any>&;
	using EventCallback = std::function<void(EventCallbackParameter)>;
	
	static UUID subscribe(const std::string& key, const EventCallback& callback);
	static void unSubscribe(const std::string& key, UUID id);

	static void fire(std::string key, EventCallbackParameter args = {});
private:

	static u32 ID;

	static std::unordered_map<
		std::string, 
		std::unordered_map<UUID, EventCallback>
	> _callbacks;
};