#pragma once
#include <functional>

#include <Physics/Collider.hpp>

struct Zone {
	using Callback = std::function<void(const Collider&)>;

	std::shared_ptr<Collider> collider;

	Callback entered = [](const Collider&) {};
	Callback exited = [](const Collider&) {};
	Callback inside = [](const Collider&) {};

	bool toRemove = false;
	bool sensor = true;
};