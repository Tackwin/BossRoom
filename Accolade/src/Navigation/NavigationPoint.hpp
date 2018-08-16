#pragma once

#include "Math/Vector.hpp"

#include "Utils/UUID.hpp"

class NavigationPoint {
public:

	NavigationPoint() noexcept;

	NavigationPoint(NavigationPoint&) = delete;
	NavigationPoint& operator=(NavigationPoint&) = delete;

	NavigationPoint(NavigationPoint&&) = delete;
	NavigationPoint& operator=(NavigationPoint&&) = delete;

	void severe(NavigationPoint* linked) noexcept;
	void link(NavigationPoint* other) noexcept;

	NavigationPoint* next(Vector2f point) const noexcept;

	Vector2f getPos() const noexcept;

private:

	std::vector<NavigationPoint*> neighboors;
	Vector2f pos;
	UUID id;

};