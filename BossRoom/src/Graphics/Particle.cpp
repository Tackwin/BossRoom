#include "Graphics/Particle.hpp"

#include "Global/Const.hpp"
#include "Managers/AssetsManager.hpp"
#include "Managers/TimerManager.hpp"

Particle::Particle(nlohmann::json json_, Vector2 pos_, Vector2 dir_, Function update_) :
	_json(json_),
	_pos(pos_),
	_dir(dir_),
	_update(update_)
{
	std::string strSprite = _json["sprite"];
	assert(AssetsManager::loadTexture(strSprite, std::string(ASSETS_PATH) + strSprite));
	_sprite.setTexture(AssetsManager::getTexture(strSprite));
	_keys["destroy"] = TimerManager::addFunction(_json["lifetime"], "destroy", [&](float)mutable->bool {
		_remove = true;
		return true;
	});
}

void Particle::update(float dt) {
	_pos += _dir * dt * _json["speed"].get<float>();
	_update(dt);
}

void Particle::render(sf::RenderTarget& target) {
	_sprite.setPosition(_pos);
	target.draw(_sprite);
}