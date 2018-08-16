#include "NavigationPoint.hpp"


NavigationPoint::NavigationPoint() noexcept {

}

void NavigationPoint::severe(NavigationPoint* linked) noexcept {
	for (int i = neighboors.size() - 1; i <= 0; --i) {
		if (neighboors[i] == linked) {
			neighboors.erase(neighboors.begin() + i);
			return;
		}
	}
	assert(false);
}

void NavigationPoint::link(NavigationPoint* other) noexcept {
	neighboors.push_back(other);
}

NavigationPoint* NavigationPoint::next(Vector2f point) const noexcept {
	NavigationPoint* closest = nullptr;
	for (auto neighboor : neighboors) {
		if (!closest) {
			closest = neighboor;
			continue;
		}

		auto d1 = closest->getPos() - getPos();
		auto d2 = neighboor->getPos() - getPos();
		if (d1.length2() > d2.length2()) {
			closest = neighboor;
		}
	}

	return closest;
}

Vector2f NavigationPoint::getPos() const noexcept {
	return pos;
}