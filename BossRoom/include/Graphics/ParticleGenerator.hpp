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
	ParticleGenerator();
	ParticleGenerator(nlohmann::json json_, Vector2 pos_);

	void pause();
	void resume();
	void restart();

	void update(double dt);
	void render(sf::RenderTarget& target);

private:

	nlohmann::json _json;

	bool _paused = false;

	Vector2 _pos;

	std::vector<std::string> _keys;
	std::function<bool(double)> _lambda;
	std::string _lambdaKey = "";

	std::vector<std::shared_ptr<Particle>> _particles;
};