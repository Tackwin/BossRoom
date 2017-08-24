#pragma once

#include <functional>

#include "3rd/json.hpp"
#include "Math/Vector2.hpp"

class Particle {
public:
	using Function = std::function<void(Particle&, float)>;
private:
	Vector2 _pos;
	Vector2 _dir;
	Vector2 _scale;

	nlohmann::json _json;

	Function _update;

	sf::Sprite _sprite;

	bool _remove = false;
public:
	Particle(nlohmann::json json, Vector2 pos, Vector2 dir, Function update = [](Particle&, float) {});
	~Particle();

	void update(float dt);
	void render(sf::RenderTarget& target);
};

