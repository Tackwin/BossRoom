#include "Gameplay/Boss.hpp"

#include "Game.hpp"
#include "Common.hpp"

#include "Managers/AssetsManager.hpp"
#include "Managers/TimerManager.hpp"
#include "Managers/MemoryManager.hpp"

#include "Gameplay/Projectile.hpp"
#include "Gameplay/Patterns.hpp"
#include "Gameplay/Player.hpp"

using namespace nlohmann;

std::vector<std::shared_ptr<Boss>> Boss::bosses(C::N_LEVEL);

// Assume AARRGGBB
sf::Color hexToColor(const std::string& hex) {
	return sf::Color(
		static_cast<sf::Uint8>(
			std::stoi(std::string(hex.data() + 2, 2), nullptr, 16)
		),
		static_cast<sf::Uint8>(
			std::stoi(std::string(hex.data() + 4, 2), nullptr, 16)
		),
		static_cast<sf::Uint8>(
			std::stoi(std::string(hex.data() + 6, 2), nullptr, 16)
		),
		static_cast<sf::Uint8>(
			std::stoi(std::string(hex.data() + 0, 2), nullptr, 16)
		)
	);
}

Boss::Boss(const basic_json<>& json,
	std::function<void(double, Boss&)> updateFunction,
	std::function<void(Boss&)> init,
	std::function<void(Boss&)> unInit) :

	Object(),
	_json(json),
	_update(updateFunction),
	_init(init),
	_unInit(unInit) 
{
	idMask.set((size_t)Object::BIT_TAGS::BOSS);
	collisionMask.set((size_t)Object::BIT_TAGS::PROJECTILE);
}

void Boss::enterLevel(Level* level) {
	_level = level;

	_life = getJsonValue<float>(_json, "life");
	_maxLife = _life;
	pos.x = _json["startpos"]["x"];
	pos.y = _json["startpos"]["y"];
	_radius = _json["radius"];
	_color = hexToColor(_json["color"]);

	const auto& str = _json["sprite"].get<std::string>();
	_sprite = AnimatedSprite(str);
	_sprite.pushAnim("idle");

	_sprite.getSprite().setOrigin(_sprite.getSize() * 0.5f);
	_sprite.getSprite().setScale(
		-(4 * _radius) / _sprite.getSize().x,
		(4 * _radius) / _sprite.getSize().y
	);

	collider = &_disk;
	_disk.userPtr = this;
	_disk.r = _radius;
	_disk.onEnter = [&](Object* obj) { collision(obj); };

	_init(*this);
}
void Boss::exitLevel() {
	for (u32 i = _keyPatterns.size(); i != 0; --i) {
		auto& k = _keyPatterns[i - 1];
		TimerManager::removeFunction(k);

		_keyPatterns.erase(_keyPatterns.begin() + i - 1);
	}
	_unInit(*this);
	_level = nullptr;
}

void Boss::die() {
	for (auto& k : _keyPatterns) {
		TimerManager::removeFunction(k);
	}
	_keyPatterns.clear();
}


void Boss::update(double dt) {
	_update(dt, *this);
	for (auto& fx : _particleEffects)
		fx->update(dt);

	for (size_t i = _particleEffects.size(); i > 0u; i--) {
		if (_particleEffects[i - 1]->isEnded()) {
			delete _particleEffects[i - 1];
			_particleEffects.erase(_particleEffects.begin() + i - 1);
		}
	}
}

void Boss::render(sf::RenderTarget &target) {
	_sprite.getSprite().setPosition(getPos());
	_sprite.getSprite().setColor(_color);
	_sprite.render(target);
	for (auto& fx : _particleEffects)
		fx->render(target);
}

void Boss::hit(float d) {
	_life -= d;
	_life = _life < 0 ? 0 : _life;

	const auto& blinkDown = [&](double)mutable->bool {
		_color = hexToColor(_json["color"]);
		return true;
	};

	TimerManager::addFunction(0.05f, "blinkDown", blinkDown);
	_color = hexToColor(_json["blinkColor"]);
}

void Boss::hit(u32 d) {
	_life -= d;
	_life = _life < 0.f ? 0.f : _life;

	const auto& blinkDown = [&](double)mutable->bool {
		_color = hexToColor(_json["color"]);
		return true;
	};

	TimerManager::addFunction(0.05f, "blinkDown", blinkDown);
	_color = hexToColor(_json["blinkColor"]);
}

void Boss::addProjectile(const std::shared_ptr<Projectile>& projectile) {
	if ((_level) && (_life != 0))
		_projectilesToShoot.push_back(projectile);//TODO: debug why the callbacks doesn't delete when the boss die
}

void Boss::shoot(const nlohmann::json& json,
	const Vector2f& pos_, const Vector2f& dir) 
{
	if (_level && _life > 0) {
		_projectilesToShoot.push_back(
			MM::make_unique<Projectile>(json, pos_, dir, false)
		);
	}
}

Vector2f Boss::getDirToFire() const {
	return (game->_player->getPos() - getPos()).normalize();
}
Vector2f Boss::getPos() const {
	return pos;
}

const std::vector<std::shared_ptr<Projectile>>& Boss::getProtectilesToShoot() const {
	return _projectilesToShoot;
}
void Boss::clearProtectilesToShoot() {
	_projectilesToShoot.clear();
}
void Boss::collision(Object* obj) {
	if (auto ptr = static_cast<Projectile*>(obj); 
			obj->idMask.test((size_t)Object::BIT_TAGS::PROJECTILE) && 
			ptr->isFromPlayer()
		) 
	{
		hit(ptr->getDamage());

		auto& particleGeneratorJson =
			AssetsManager::getJson(JSON_KEY)["particlesGenerator"];
		
		std::string particleGenerator = 
			_json["hitParticle"];

		_particleEffects.push_back(
			new ParticleGenerator(
				particleGeneratorJson[particleGenerator],
				ptr->getPos()
			)
		);
		_particleEffects.back()->start();
		
		ptr->remove();
	}
}
float Boss::getRadius() const {
	return _radius;
}

float Boss::getLife() const {
	return _life;
}
float Boss::getMaxLife() const {
	return _maxLife;
}
const Level* Boss::getLevel() const {
	return _level;
}
