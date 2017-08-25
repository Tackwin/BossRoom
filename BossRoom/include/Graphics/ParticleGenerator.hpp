#pragma once

#include <memory>
#include <vector>
#include <string>

#include <SFML/Graphics.hpp>

#include "3rd/json.hpp"

#include "Math/Vector2.hpp"

class Particle;
class ParticleGenerator {
public:
	ParticleGenerator(nlohmann::json json_, Vector2 pos_);

	void pause() {};
	void resume() {};
	void restart() {};

	void update(float dt);
	void render(sf::RenderTarget& target);

private:

	nlohmann::json _json;

	Vector2 _pos;

	std::vector<std::string> _keys;
	std::function<bool(float)> _lambda;

	std::vector<std::shared_ptr<Particle>> _particles;
};