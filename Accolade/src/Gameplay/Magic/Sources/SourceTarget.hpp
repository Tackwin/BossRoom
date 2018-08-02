#pragma once

#include "Source.hpp"

struct SourceTargetInfo {
	static constexpr auto JSON_ID = "SourceTargetInfo";

	SourceInfo source;
	float range{ 0.5f };

	static SourceTargetInfo loadJson(nlohmann::json json) noexcept;
	static nlohmann::json saveJson(SourceTargetInfo info) noexcept;
};


class SourceTarget : public Source {
public:

	SourceTarget(SourceTargetInfo info) noexcept;

	virtual void update(double dt) noexcept override;
	virtual void render(sf::RenderTarget& target) noexcept override;

private:

	float gened{ 0.f };

	SourceTargetInfo info_;

};
