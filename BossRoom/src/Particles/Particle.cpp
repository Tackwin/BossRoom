#include "Particles/Particle.hpp"

#include <string>

#include "Managers/AssetsManager.hpp"

Particle::Particle(nlohmann::json json, Vector2 pos, Vector2 dir, Function update) :
	_json(json),
	_pos(pos),
	_dir(dir),
	_update(update)
{
	auto spritePath = json["sprite"].get<std::string>();
	AssetsManager::loadTexture(spritePath, spritePath);
	_sprite = sf::Sprite(AssetsManager::getTexture(spritePath));
	_sprite.setOrigin(_sprite.getTextureRect().width / 2.f, _sprite.getTextureRect().height / 2.f);
}

Particle::~Particle() {
}

void Particle::update(float dt) {
	_update(*this, dt);
}

void Particle::render(sf::RenderTarget& target) {

}