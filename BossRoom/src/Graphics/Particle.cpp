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
	_keys["destroy"] = TimerManager::addFunction(getJsonValue<float>(_json, "lifetime"), "destroy", [&](float)mutable->bool {
		_remove = true;
		return true;
	});

	_json["speed"] = getJsonValue<float>(_json, "speed"); //collapse the wave function :p
}

void Particle::update(float dt) {
	_pos += _dir * dt * _json["speed"].get<float>();
	_update(dt);
}

void Particle::render(sf::RenderTarget& target) {
	_sprite.setPosition(_pos);
	target.draw(_sprite);
}

std::vector<std::shared_ptr<Particle>> Particle::burst(nlohmann::json json_, Vector2 pos_){
	std::vector<std::shared_ptr<Particle>> particles;
	uint32_t nParticles = getJsonValue<uint32_t>(json_, "nParticles");
	auto particleJson = json_["particle"];

	particles.reserve(nParticles);

	for (uint32_t i = 0; i < nParticles; ++i) {
		Vector2 pos = pos_;
		Vector2 dir = Vector2::createUnitVector(getJsonValue<float>(json_, "dir"));
		particles.push_back(std::make_shared<Particle>(particleJson, pos, dir));
	}
	
	return particles;
}
