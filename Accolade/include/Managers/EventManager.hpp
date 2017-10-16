#pragma once

#include <unordered_map>
#include <functional>
#include <stdarg.h>
#include <vector>
#include <map>
#include <any>
/*

EventListener::create() {
	subscirbe("event", &on_event);
}

EventListener::on_event(T a, U b,...) {
	unSubscirbe("event", &on_event);
	// Do stuff...
}

And here is a producer:
EventFirer::collided_with(CollisionObject object) {
	fire("event", new T(), new U());
}


*/



class EventManager {
public:
	using EventCallbackParameter = const std::initializer_list<std::any>&;
	using EventCallback = std::function<void(EventCallbackParameter)>;
	
	static uint32_t subscribe(const std::string& key, const EventCallback& callback);
	static void unSubscribe(const std::string& key, uint32_t id);

	static void fire(std::string key, EventCallbackParameter args = {});
private:

	static uint32_t ID;

	static 
	std::unordered_map<
		std::string, 
		std::map<uint32_t, EventCallback>
	> _callbacks;
};