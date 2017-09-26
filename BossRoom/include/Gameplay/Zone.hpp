#pragma once
#include <functional>

#include <Physics/Object.hpp>

struct Zone : public Object {
	using Callback = std::function<void(Object*)>;

	Callback entered = [](Object*) {};
	Callback exited = [](Object*) {};
	Callback inside = [](Object*) {};

	bool toRemove = false;
	bool sensor = true;

	Zone(float r = 0.f);
	virtual ~Zone() override;

	void setRadius(float r);
	float getRadius() const;
private:
	std::vector<Object*> _objectsColliding;

	std::shared_ptr<Disk> _disk;

	void collision(Object* obj);
};