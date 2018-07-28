#pragma once

#include "Source.hpp"

struct SourceBoomerangInfo {
	static constexpr auto JSON_ID = "SourceBoomerang";

	SourceInfo source;
	float range{ 0.5f };

	static SourceBoomerangInfo loadJson(nlohmann::json json) noexcept;
	static nlohmann::json saveJson(SourceBoomerangInfo info) noexcept;
};


class SourceBoomerang : public Source {
public:

	SourceBoomerang(SourceBoomerangInfo info) noexcept;

	virtual void update(double dt) noexcept override;
	virtual void render(sf::RenderTarget& target) noexcept override;

private:

	float gened{ 0.f };

	SourceBoomerangInfo info_;

};
