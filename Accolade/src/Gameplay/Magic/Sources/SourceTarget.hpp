#pragma once

#include "Source.hpp"


struct SourceTargetInfo {
	static constexpr auto JSON_ID = "SourceTargetInfo";

	SourceInfo source;
	float range{ 0.5f };

	static SourceTargetInfo loadJson(nlohmann::json json) noexcept;
	static nlohmann::json saveJson(SourceTargetInfo info) noexcept;
};

class Player;
class SourceTarget : public Source {
public:
	static constexpr auto JSON_ID = "SourceTarget";

	SourceTarget(SourceTargetInfo info) noexcept;

	virtual void update(double dt) noexcept override;
	virtual void render(sf::RenderTarget& target) noexcept override;

	void onEnter(Object* object) noexcept;
	void onExit(Object* object) noexcept;
private:

	Player * player_{ nullptr };

	float gened{ 0.f };

	SourceTargetInfo info_;

};
