#pragma once
#include <SFML/Graphics.hpp>

#include "3rd/json.hpp"
#include "Math/Vector.hpp"

#include "Components/Removable.hpp"
#include "Physics/Object.hpp"

struct SourceInfo {
	
	int id{ 0 };

	float reloadTime{ 0.f };

	Vector2f pos;
	Vector2f size{ 0.66f, 0.66f };

	std::string sprite{"source"};

	static SourceInfo loadJson(nlohmann::json json) noexcept;
	static nlohmann::json saveJson(SourceInfo info) noexcept;
};

class Source: public Object, public Removable {
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
	
	mutable sf::Sprite _sprite;
};