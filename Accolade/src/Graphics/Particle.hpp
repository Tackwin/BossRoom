#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

#include <SFML/Graphics.hpp>

#include "./../3rd/json.hpp"

#include "./../Math/Vector.hpp"

#include "./../Utils/UUID.hpp"

class Particle {
public:
	using Function = std::function<void(double)>;
private:
	sf::Sprite _sprite;

	Vector2f _pos;
	Vector2f _dir;

	float speed{ NAN };
	double _countdownLifetime{ 0.0 };

	Function _update;


	std::unordered_map<std::string, UUID> _keys;

public:
	Particle(
		const nlohmann::json& json_,
		Vector2f pos_,
		Vector2f dir_,
		Function update_ = [](double) {}
	) noexcept;
	Particle(
		std::normal_distribution<> size_dist,
		std::normal_distribution<> speed_dist,
		std::normal_distribution<> lifetime_dist,
		std::normal_distribution<> angle_dist,
		sf::Texture& texture,
		Vector2f pos_,
		Vector2f dir_,
		Function update_ = [](double) {}
	) noexcept;

	void update(double dt);
	void render(sf::RenderTarget& target);

	bool toRemove() const { return _countdownLifetime <= 0.0; }

	static std::vector<std::shared_ptr<Particle>> burst(nlohmann::json json_, Vector2f pos_);
};