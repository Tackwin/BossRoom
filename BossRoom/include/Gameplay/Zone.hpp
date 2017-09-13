#pragma once
#include <functional>

#include <Physics/Collider.hpp>

struct Zone {
	using Callback = std::function<void(const Collider&)>;

	Disk disk;

	Callback entered = [](const Collider&) {};
	Callback exited = [](const Collider&) {};
	Callback inside = [](const Collider&) {};

	bool toRemove = false;
};