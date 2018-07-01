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

	void loadJson(const nlohmann::json& json);

	bool toRemove() const noexcept;

	void setPos(const Vector2f& pos);
	Vector2f getPos() const;

	void start();
	void pause();
	void resume();
	void restart();

	void update(double dt);
	void render(sf::RenderTarget& target);

private:

	nlohmann::json _json{};
	nlohmann::json _particule{};

	bool _paused = false;

	double _timer{ 0.0 };
	double _iTime{ 0.0 };

	Vector2f _pos = { 0, 0 };
	u32 _remain = 0u;
	u32 _nParticles = 0u;

	std::vector<std::string> _keys = {};

	std::vector<std::shared_ptr<Particle>> _particles = {};
};