#include "Zone.hpp"

#include "./../Managers/TimerManager.hpp"
#include "./../Managers/MemoryManager.hpp"
#include "./../Managers/AssetsManager.hpp"

#include "./../Common.hpp"

#include "Projectile.hpp"
#include "Player/Player.hpp"

std::shared_ptr<Zone> Zone::buildExplosion(
	const std::shared_ptr<Projectile>& p
) {
	if (p->getJson().find("explosion") == p->getJson().cend()) {
		return std::shared_ptr<Zone>();
	}
	auto json = p->getJson()["explosion"];

	auto z = std::make_shared<Zone>(
		getJsonValue<float>(json, "radius")
	);

	float d = getJsonValue<float>(json, "damage");
	float f = getJsonValue<float>(json, "force");

	z->pos = p->pos;
	TimerManager::addFunction(
		getJsonValue<float>(json, "time"),
		[z](double) -> bool {
			z->setRemove();
			return false;
		}
	);
	z->collisionMask.set((size_t)Object::BIT_TAGS::PLAYER);
	z->collider->onEnter = [d, f, z](Object* obj) {
		if (!obj->idMask[(size_t)Object::BIT_TAGS::PLAYER]) return;

		Player* p = static_cast<Player*>(obj);
		auto vec = Vector2f::createUnitVector(z->pos.angleTo(p->pos)) * f;
		vec.y = -sqrtf(G * 2.f * -vec.y) / 1.5f;

		p->hit(d);
		p->applyVelocity(vec);
	};

	z->addSprite("white", sf::Sprite(AM::getTexture("white_disk")));
	return z;
}

Zone::Zone(float r) : 
	Object(),
	_radius(r) 
{

	collider = std::make_unique<Disk>();
	_disk = (Disk*)collider.get();
	_disk->r = r;
	_disk->sensor = true;
	collider->onEnter = [&](Object* obj) mutable { entered(obj); };
	collider->onExit = [&](Object* obj) mutable { exited(obj); };
	
	idMask.set((size_t)BIT_TAGS::ZONE);
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
	reinterpret_cast<Disk*>(collider.get())->r = r;
}

float Zone::getRadius() const {
	return reinterpret_cast<Disk*>(collider.get())->r;
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
