#include "Boss.hpp"

#include "./../Game.hpp"
#include "./../Common.hpp"

#include "./../Managers/AssetsManager.hpp"
#include "./../Managers/TimerManager.hpp"
#include "./../Managers/MemoryManager.hpp"

#include "Player/Player.hpp"
#include "Projectile.hpp"
#include "Patterns.hpp"

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

Boss::Boss(Boss& other) :
	Boss(other._json, other._update, other._init, other._unInit) {}

Boss::~Boss() noexcept {
	if (_blinkDownKey) {
		TM::removeFunction(_blinkDownKey);
	}
	for (auto& k : _keyPatterns) {
		TimerManager::removeFunction(k);
	}
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

	collider = std::make_unique<Disk>();
	_disk = (Disk*)collider.get();
	_disk->userPtr = this;
	_disk->r = _radius;
	_disk->onEnter = [&](Object* obj) { collision(obj); };

	_init(*this);
}
void Boss::exitLevel() {
	for (auto& k : _keyPatterns) {
		TimerManager::removeFunction(k);
	}
	_keyPatterns.clear();
	
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

	for (size_t i = 0u; i < _keyPatterns.size();) {
		if (TM::functionsExist(_keyPatterns[i])) {
			++i;
			continue;
		}
		_keyPatterns.erase(_keyPatterns.begin() + i);
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
		_blinkDownKey.nullify();
		return true;
	};

	TimerManager::addFunction(0.05f, blinkDown);
	_color = hexToColor(_json["blinkColor"]);
}

void Boss::shoot(const std::shared_ptr<Projectile>& projectile) {
	if (_level && _life > 0) {
		_projectilesToShoot.push_back(projectile);
	}
}

void Boss::shoot(const nlohmann::json& json,
	const Vector2f& pos_, const Vector2f& dir) 
{
	if (_level && _life > 0) {
		_projectilesToShoot.push_back(
			std::make_shared<Projectile>(json, pos_, dir, false)
		);
	}
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
Level* Boss::getLevel() const {
	return _level;
}

void Boss::addKeyTimer(UUID key) {
	_keyPatterns.push_back(key);
}
void Boss::delKeyTimer(UUID key) {
	_keyPatterns.erase(std::find(_keyPatterns.begin(), _keyPatterns.end(), key));
}
