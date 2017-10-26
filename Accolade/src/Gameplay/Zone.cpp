#include <Gameplay/Zone.hpp>

Zone::Zone(float r) : Object() {
	_disk = std::make_shared<Disk>();
	_disk->r = r;

	collider = _disk.get();
	collider->onEnter = [&](Object* obj) {collision(obj); };
	
	idMask |= BIT_TAGS::ZONE;
}
Zone::Zone(const Zone& other) : Object() {

	_disk = std::make_shared<Disk>();
	_disk->r = other._disk->r;

	collider = _disk.get();
	collider->onEnter = [&](Object* obj) { collision(obj);  };

	idMask = other.idMask;
	collisionMask = other.collisionMask;
}

Zone::~Zone() {
	Object::~Object();
}

void Zone::collision(Object* obj) {
	inside(obj);
}

void Zone::setRadius(float r) {
	reinterpret_cast<Disk*>(collider)->r = r;
}

float Zone::getRadius() const {
	return reinterpret_cast<Disk*>(collider)->r;
}

Zone& Zone::operator=(const Zone& other) {
	this->Object::operator=(other);

	_disk = std::make_shared<Disk>();
	_disk->r = other._disk->r;

	collider = _disk.get();
	collider->onEnter = [&](Object* obj) { collision(obj);  };

	idMask = other.idMask;
	collisionMask = other.collisionMask;

	return *this;
}
