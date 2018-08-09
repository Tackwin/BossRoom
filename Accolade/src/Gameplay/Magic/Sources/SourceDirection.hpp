#pragma once

#include "Source.hpp"

struct SourceDirectionInfo {
	static constexpr auto JSON_ID = "SourceDirectionInfo";

	SourceInfo source;
	float range{ 0.5f };

	static nlohmann::json saveJson(SourceDirectionInfo info) noexcept;
	static SourceDirectionInfo loadJson(nlohmann::json json) noexcept;
};

class Player;
class SourceDirection : public Source {
public:
	static constexpr auto JSON_ID = "SourceDirection";

	SourceDirection(SourceDirectionInfo info) noexcept;

	virtual void update(double) noexcept;
	virtual void render(sf::RenderTarget&) noexcept;

private:

	void onEnter(Object* object) noexcept;
	void onExit(Object* object) noexcept;

	float timer_{ 0.f };

	Player* player_{ nullptr };
	SourceDirectionInfo info_;
};