#include "Particle.hpp"

#include "./../Managers/AssetsManager.hpp"
#include "./../Managers/TimerManager.hpp"

Particle::Particle(
	const nlohmann::json& json_, Vector2f pos_, Vector2f dir_, Function update_
) noexcept :
	_pos(pos_),
	_dir(dir_),
	_update(update_)
{
	std::string strSprite = json_["sprite"];
	_sprite.setTexture(AssetsManager::getTexture(strSprite));
	_sprite.setOrigin(
		_sprite.getTextureRect().width / 2.f,
		_sprite.getTextureRect().height / 2.f
	);
	_countdownLifetime = getJsonValue<double>(json_, "lifetime");

	speed = getJsonValue<float>(json_, "speed"); //collapse the wave function :p
	if (json_.find("a") != json_.end()) {
		_sprite.setRotation(getJsonValue<float>(json_, "a") * 360 / 3.1415926f);
	}
	if (json_.find("size") != json_.end()) {
		const auto& size = getJsonValue<float>(json_, "size");
		_sprite.setScale(
			size / _sprite.getTextureRect().width,
			size / _sprite.getTextureRect().height
		);
	}
}

Particle::Particle(
	std::normal_distribution<> size_dist,
	std::normal_distribution<> speed_dist,
	std::normal_distribution<> lifetime_dist,
	std::normal_distribution<> angle_dist,
	sf::Texture& texture,
	Vector2f pos_,
	Vector2f dir_,
	Function update_
) noexcept :
	_pos(pos_),
	_dir(dir_),
	_update(update_)
{
	_sprite.setTexture(texture);
	_sprite.setOrigin(
		_sprite.getTextureRect().width / 2.f,
		_sprite.getTextureRect().height / 2.f
	);
	_countdownLifetime = lifetime_dist(C::RD);

	speed = (float)speed_dist(C::RD);
	_sprite.setRotation((float)angle_dist(C::RD) * 360 / 3.1415926f);
	const auto& size = (float)size_dist(C::RD);
	_sprite.setScale(
		size / _sprite.getTextureRect().width,
		size / _sprite.getTextureRect().height
	);
}

void Particle::update(double dt) {
	_pos += _dir * dt * speed;
	_countdownLifetime -= dt;
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
