#pragma once

#include <unordered_map>
#include <functional>
#include <stdarg.h>
#include <vector>
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
	using EventCallback = void(*)(uint32_t, ...);
	
	static void subscribe(const std::string& key, const EventCallback& callback);
	static void unSuscribe(const std::string& key, const EventCallback& callback);

	static void fire(const std::string& key, uint32_t n, ...);
private:

	static 
	std::unordered_map<
		std::string, 
		std::vector<EventCallback>
	> _callbacks;
};