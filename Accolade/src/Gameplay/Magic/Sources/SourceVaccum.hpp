#pragma once

#include "Source.hpp"

struct SourceVaccumInfo {
	static constexpr auto JSON_ID = "SourceVaccumInfo";

	SourceInfo source;
	float range{ 0.5f };

	static SourceVaccumInfo loadJson(nlohmann::json json) noexcept;
	static nlohmann::json saveJson(SourceVaccumInfo info) noexcept;
};


class SourceVaccum : public Source {
public:
	static constexpr auto JSON_ID = "SourceVaccum";

	SourceVaccum(SourceVaccumInfo info) noexcept;

	virtual void update(double dt) noexcept override;
	virtual void render(sf::RenderTarget& target) noexcept override;

	void onEnter(Object* obj) noexcept;

private:

	SourceVaccumInfo info_;

};
