#pragma once
#include <functional>

#include <Physics/Object.hpp>

struct Zone : public Object{
	using Callback = std::function<void(Object*)>;

	Callback entered = [](Object*) {};
	Callback exited = [](Object*) {};
	Callback inside = [](Object*) {};

	bool toRemove = false;
	bool sensor = true;

	Zone(float r = 0.f);
	virtual ~Zone() override;

private:
	std::vector<Object*> _objectsColliding;

	void collision(Object* obj);
};