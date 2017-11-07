#include <Gameplay/Zone.hpp>

Zone::Zone(float r) : 
	Object(),
	_radius(r) 
{
	_disk = std::make_shared<Disk>();
	_disk->r = r;

	collider = _disk.get();
	collider->onEnter = entered;
	collider->onExit = exited;
	
	idMask.set((size_t)BIT_TAGS::ZONE);
}
Zone::Zone(const Zone& other) : Object() {

	_disk = std::make_shared<Disk>();
	_disk->r = other._disk->r;

	collider = _disk.get();
	collider->onEnter = [&](Object* obj) { collision(obj);  };

	idMask = other.idMask;
	collisionMask = other.collisionMask;
}

void Zone::render(sf::RenderTarget& target) {
	for (auto& p : _sprites) {
		auto s = p.second;
		s.setOrigin(
			s.getTextureRect().width * 0.5f,
			s.getTextureRect().height * 0.5f
		);
		s.setScale(
			2.f * _radius / s.getTextureRect().width,
			2.f * _radius / s.getTextureRect().height
		);
		s.setPosition(pos);
		target.draw(s);
	}
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

void Zone::setRemove(bool remove) {
	_toRemove = remove;
}
bool Zone::toRemove() const {
	return _toRemove;
}

void Zone::addSprite(const std::string& key, const sf::Sprite& sprite) {
	_sprites[key] = sprite;
}
void Zone::removeSprite(const std::string& key) {
	if (auto it = _sprites.find(key); it != _sprites.end()) {
		_sprites.erase(it);
	}
}
sf::Sprite& Zone::getSprite(const std::string& key) {
	if (auto it = _sprites.find(key); it != _sprites.end()) {
		return _sprites[key];
	}
	else {
		assert("no sprite");	//the fuck i'm supposed to do ?
								// i guess it's raise the question of an error handling system
		return _sprites[key];
	}
}

void Zone::cleanSprites() {
	_sprites.clear();
}
