#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

#include <SFML/Graphics.hpp>

#include "./../3rd/json.hpp"

#include "./../Math/Vector.hpp"

class Particle {
public:
	using Function = std::function<void(double)>;
private:
	sf::Sprite _sprite;

	Vector2f _pos;
	Vector2f _dir;


	bool _remove = false;

	Function _update;

	nlohmann::json _json;

	std::unordered_map<std::string, std::string> _keys;

public:
	Particle(nlohmann::json json_, Vector2f pos_, Vector2f dir_, Function update_ = [](double) {});

	void update(double dt);
	void render(sf::RenderTarget& target);

	bool toRemove() const { return _remove; }

	static std::vector<std::shared_ptr<Particle>> burst(nlohmann::json json_, Vector2f pos_);
};