#include <Gameplay/Zone.hpp>

Zone::Zone(float r) : Object() {
	collider = new Disk();
	idMask |= ZONE;
	collisionMask |= PLAYER;
	collider->onEnter = [&](Object* obj) {collision(obj); };
	reinterpret_cast<Disk*>(collider)->r = r;
}
Zone::~Zone() {
	Object::~Object();

	delete collider;
}

void Zone::collision(Object* obj) {
	inside(obj);
}