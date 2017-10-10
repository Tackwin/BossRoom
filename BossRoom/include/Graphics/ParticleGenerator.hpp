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
	~ParticleGenerator();

	void loadJson(const nlohmann::json& json);

	void setPos(const Vector2& pos);
	Vector2 getPos() const;

	void start();
	void pause();
	void resume();
	void restart();

	void update(double dt);
	void render(sf::RenderTarget& target);

	bool isEnded() const;
private:

	nlohmann::json _json = {};

	bool _paused = false;
	bool _ended = false;

	Vector2 _pos = Vector2::ZERO;
	uint32_t _remain = 0u;
	uint32_t _nParticles = 0u;

	std::vector<std::string> _keys = {};
	std::function<bool(double)> _lambda = [](auto) {return false; };
	std::string _lambdaKey = "";

	std::vector<std::shared_ptr<Particle>> _particles = {};
};