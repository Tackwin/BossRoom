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
	using EventCallback = std::function<void(const EventCallbackParameter&)>;
	
	enum class EventType : size_t {
		Player_Use_Main_Weapon = 0,
		Player_Consume_Binded,
		Count
	};

	static UUID subscribe(const EventType& type, const EventCallback& callback);
	static UUID subscribe(const std::string& key, const EventCallback& callback);
	static void unSubscribe(const EventType& type, UUID id);
	static void unSubscribe(const std::string& key, UUID id);

	static void fire(std::string key, EventCallbackParameter args = {});
	static void fire(const EventType& key, const EventCallbackParameter& args = {});
private:

	static u32 ID;

	static std::vector<std::unordered_map<UUID, EventCallback>> callbacks;

	static std::unordered_map<
		std::string, 
		std::unordered_map<UUID, EventCallback>
	> _callbacks;
};

using EM = EventManager;
