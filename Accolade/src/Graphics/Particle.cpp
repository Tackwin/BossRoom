#include "Graphics/Particle.hpp"

#include "Managers/AssetsManager.hpp"
#include "Managers/TimerManager.hpp"

Particle::Particle(nlohmann::json json_, Vector2f pos_, Vector2f dir_, Function update_) :
	_json(json_),
	_pos(pos_),
	_dir(dir_),
	_update(update_)
{
	std::string strSprite = _json["sprite"];
	_sprite.setTexture(AssetsManager::getTexture(strSprite));
	_sprite.setOrigin(
		_sprite.getTextureRect().width / 2.f, 
		_sprite.getTextureRect().height / 2.f
	);
	_keys["destroy"] = TimerManager::addFunction(
		getJsonValue<float>(_json, "lifetime"), 
		"destroy", 
		[&](double)mutable->bool {
			_remove = true;
			return true;
		}
	);

	_json["speed"] = getJsonValue<float>(_json, "speed"); //collapse the wave function :p
	if (_json.find("a") != _json.end()) {
		_sprite.setRotation(getJsonValue<float>(_json, "a") * 360 / 3.1415926f);
	}
	if (_json.find("size") != _json.end()) {
		const auto& size = getJsonValue<float>(_json, "size");
		_sprite.setScale(size, size);
	}
}

void Particle::update(double dt) {
	_pos += _dir * dt * _json["speed"].get<float>();
	_update(dt);
}

void Particle::render(sf::RenderTarget& target) {
	_sprite.setPosition(_pos);
	target.draw(_sprite);
}

std::vector<std::shared_ptr<Particle>> 
	Particle::burst(nlohmann::json json_, Vector2f pos_)
{
	std::vector<std::shared_ptr<Particle>> particles;
	auto nParticles = (u32)getJsonValue<i32>(json_, "nParticles");
	auto particleJson = json_["particle"];

	particles.reserve(nParticles);

	for (u32 i = 0u; i < nParticles; ++i) {
		Vector2f pos = pos_;
		Vector2f dir = 
			Vector2f::createUnitVector(getJsonValue<float>(json_, "dir"));

		particles.push_back(std::make_shared<Particle>(particleJson, pos, dir));
	}
	
	return particles;
}
