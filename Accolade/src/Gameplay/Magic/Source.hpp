#pragma once
#include <SFML/Graphics.hpp>

#include "./../../3rd/json.hpp"
#include "./../../Math/Vector.hpp"

struct SourceInfo {
	
	int id{ 0 };

	float reloadTime{ 0.f };

	Vector2f pos;
	Vector2f size;

	std::string sprite;

	static SourceInfo load(nlohmann::json json) noexcept;
};

class Source {
public:
	Source(SourceInfo info) noexcept;

	Source(Source&) = delete;
	Source& operator=(Source&) = delete;

	Source(Source&&) = default;
	Source& operator=(Source&&) = default;

	void update(double dt) noexcept;
	void render(sf::RenderTarget& target) const noexcept;

private:
	SourceInfo _info;
	Vector2f pos;

	mutable sf::Sprite _sprite;
};