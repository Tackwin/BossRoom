#pragma once
#include <functional>

#include "Physics/Object.hpp"

class Zone : public Object {
public:
	using Callback = std::function<void(Object*)>;

	Callback entered = [](Object*) {};
	Callback exited = [](Object*) {};
	Callback inside = [](Object*) {};


	Zone(float r = 0.f);
	Zone(const Zone& other);

	void setRadius(float r);
	float getRadius() const;

	void setRemove(bool remove = true);
	bool needRemove() const;

	Zone& operator=(const Zone& other);
protected:
	std::vector<Object*> _objectsColliding;

	std::shared_ptr<Disk> _disk;

	bool _toRemove = false;

	float _radius = 0.f;

	void collision(Object* obj);
};