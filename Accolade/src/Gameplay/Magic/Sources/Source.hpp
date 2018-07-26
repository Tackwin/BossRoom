#pragma once
#include <SFML/Graphics.hpp>

#include "3rd/json.hpp"
#include "Math/Vector.hpp"

#include "Components/Removable.hpp"
#include "Physics/Object.hpp"

struct SourceInfo {
	static constexpr auto JSON_ID = "Source";

	int id{ 0 };

	float reloadTime{ 0.f };

	Vector2f pos;
	Vector2f size{ 0.66f, 0.66f };

	std::string sprite{ "source" };

	static SourceInfo loadJson(nlohmann::json json) noexcept;
	static nlohmann::json saveJson(SourceInfo info) noexcept;
};

struct SourceBoomerangInfo {
	static constexpr auto JSON_ID = "SourceBoomerang";

	SourceInfo source;

	static SourceBoomerangInfo loadJson(nlohmann::json json) noexcept;
	static nlohmann::json saveJson(SourceBoomerangInfo info) noexcept;
};

class Source : public Object, public Removable {
public:
	Source(SourceInfo info) noexcept;

	Source(Source&) = delete;
	Source& operator=(Source&) = delete;

	Source(Source&&) = default;
	Source& operator=(Source&&) = default;

	virtual void update(double dt) noexcept;
	virtual void render(sf::RenderTarget& target) noexcept;

private:
	SourceInfo _info;

	sf::Sprite _sprite;
};

class SourceBoomerang : public Source {
public:

	SourceBoomerang(SourceBoomerangInfo info) noexcept;

	virtual void update(double dt) noexcept override;
	virtual void render(sf::RenderTarget& target) noexcept override;

private:

	SourceBoomerangInfo _info;

};
