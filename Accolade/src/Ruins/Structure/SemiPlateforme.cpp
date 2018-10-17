#include "SemiPlateforme.hpp"

#include <memory>
#include "Physics/Collider.hpp"

#define SAVE(x, y) json[#x] = y(info.x);

nlohmann::json SemiPlateformeInfo::saveJson(const SemiPlateformeInfo& info) noexcept {
	nlohmann::json json;
	SAVE(rec, Rectangle2f::saveJson);
	return json;
}
#undef SAVE
#define LOAD(x, y) if (auto i = json.find(#x); i != json.end()) info.x = y(*i);

SemiPlateformeInfo SemiPlateformeInfo::loadJson(const nlohmann::json& json) noexcept {
	SemiPlateformeInfo info;
	LOAD(rec, Rectangle2f::loadJson);
	return info;
}

#undef LOAD

void to_json(nlohmann::json& j, const SemiPlateformeInfo& p) noexcept {
	j = SemiPlateformeInfo::saveJson(p);
}
void from_json(const nlohmann::json& j, SemiPlateformeInfo& p) noexcept {
	p = SemiPlateformeInfo::loadJson(j);
}

SemiPlateforme::SemiPlateforme(const SemiPlateformeInfo& info) noexcept : info(info) {
	auto box = std::make_unique<Box>();
	box->setSize(info.rec.size);
	collider = std::move(box);

	idMask.set(Object::STRUCTURE);

	pos = info.rec.pos;
}

void SemiPlateforme::update(double) noexcept {};

void SemiPlateforme::render(sf::RenderTarget&) noexcept {};
void SemiPlateforme::renderDebug(sf::RenderTarget& target) noexcept {
	sf::RectangleShape shape{ info.rec.size };
	shape.setPosition(pos);
	shape.setOutlineThickness(info.rec.size.x / 10);
	shape.setFillColor(Vector4d{ 0, 0, 0, 0 });
	shape.setOutlineColor(Vector4d{ 0, 1, 0, 1 });
	target.draw(shape);
}

