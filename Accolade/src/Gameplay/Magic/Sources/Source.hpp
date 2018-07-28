#pragma once
#include <SFML/Graphics.hpp>

#include "3rd/json.hpp"

#include "Math/Vector.hpp"

#include "Components/Removable.hpp"

#include "Physics/Object.hpp"

class Section;
struct SourceInfo {
	static constexpr auto JSON_ID = "Source";

	int id{ 0 };

	float reloadTime{ 0.5f };

	Vector2f pos;
	Vector2f size{ 0.66f, 0.66f };

	std::string sprite{ "source" };

	static SourceInfo loadJson(nlohmann::json json) noexcept;
	static nlohmann::json saveJson(SourceInfo info) noexcept;
};

class Source : public Object, public Removable {
public:
	Source(SourceInfo info) noexcept;

	Source(Source&) = delete;
	Source& operator=(Source&) = delete;

	Source(Source&&) = default;
	Source& operator=(Source&&) = default;

	virtual void enter(Section* section) noexcept;
	virtual void exit() noexcept;

	virtual void update(double dt) noexcept;
	virtual void render(sf::RenderTarget& target) noexcept;

protected:
	// Well... looks like _.* identifier are reserved by the standard...
	// I need to eventually phase _all of them_ out.

	Section* section_{ nullptr };

	sf::Sprite sprite_;

private:
	SourceInfo info_;
};
