#pragma once
#include <functional>

#include "Physics/Object.hpp"

struct Zone : public Object {
	using Callback = std::function<void(Object*)>;

	Callback entered = [](Object*) {};
	Callback exited = [](Object*) {};
	Callback inside = [](Object*) {};

	bool toRemove = false;

	Zone(float r = 0.f);
	Zone(const Zone& other);
	virtual ~Zone() override;

	void setRadius(float r);
	float getRadius() const;

	Zone& operator=(const Zone& other);
private:
	std::vector<Object*> _objectsColliding;

	std::shared_ptr<Disk> _disk;

	void collision(Object* obj);
};