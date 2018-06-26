#pragma once

#include <memory>
#include <vector>
#include <string>

#include <SFML/Graphics.hpp>

#include "./../Common.hpp"

#include "./../3rd/json.hpp"

#include "./../Math/Vector.hpp"

#include "./../Utils/UUID.hpp"

class Particle;
class ParticleGenerator {
public:
	ParticleGenerator();
	ParticleGenerator(nlohmann::json json_, Vector2f pos_);
	~ParticleGenerator();

	void loadJson(const nlohmann::json& json);

	void setPos(const Vector2f& pos);
	Vector2f getPos() const;

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

	Vector2f _pos = { 0, 0 };
	u32 _remain = 0u;
	u32 _nParticles = 0u;

	std::vector<std::string> _keys = {};
	std::function<bool(double)> _lambda = [](auto) {return false; };
	UUID _lambdaKey{ UUID::null };

	std::vector<std::shared_ptr<Particle>> _particles = {};
};