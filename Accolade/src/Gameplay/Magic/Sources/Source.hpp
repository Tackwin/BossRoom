#pragma once
#include <SFML/Graphics.hpp>

#include "3rd/json.hpp"

#include "Math/Vector.hpp"

#include "Components/Removable.hpp"

#include "Physics/Object.hpp"

class Section;
struct SourceInfo {
	static constexpr auto JSON_ID = "source";

	enum Kind {
		SourceTarget,
		SourceVaccum,
		SourceDirection,
		Count
	};

	int id{ 0 };

	float reloadTime{ 0.5f };

	Vector2f pos;
	Vector2f origin{ 0.f, 0.f };
	Vector2f size{ 0.66f, 0.66f };

	std::string texture{ "source" };

	Kind kind;

	static SourceInfo loadJson(nlohmann::json json) noexcept;
	static nlohmann::json saveJson(SourceInfo info) noexcept;
};

extern bool is_in(const Vector2f& p, const SourceInfo& info) noexcept;

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

	virtual void remove() noexcept override;
	virtual bool toRemove() const noexcept override;

protected:
	// Well... looks like _.* identifier are reserved by the standard...
	// I need to eventually phase _all of them_ out.

	Section* section_{ nullptr };

	sf::Sprite sprite_;

private:
	SourceInfo info_;

	bool _remove{ false };
};
