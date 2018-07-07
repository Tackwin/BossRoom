#include "Plateforme.hpp"
#include "./../../Managers/AssetsManager.hpp"

Plateforme::Plateforme(PlateformeInfo info) noexcept {
	auto boxPtr = std::make_unique<Box>();
	boxPtr->setSize(info.rectangle.size);

	auto temp = std::unique_ptr<Collider>(boxPtr.release());
	collider.swap(temp);
	this->idMask.set(Object::STRUCTURE);


	pos = info.rectangle.pos;
}

void Plateforme::update(double) noexcept {
}
void Plateforme::render(sf::RenderTarget&) noexcept {
}
void Plateforme::renderDebug(sf::RenderTarget&) noexcept {
}


PlateformeInfo PlateformeInfo::loadFromJson(const nlohmann::json& json) noexcept {
	PlateformeInfo info;

	info.rectangle.x = json.at("rectangle").get<std::vector<float>>()[0];
	info.rectangle.y = json.at("rectangle").get<std::vector<float>>()[1];
	info.rectangle.w = json.at("rectangle").get<std::vector<float>>()[2];
	info.rectangle.h = json.at("rectangle").get<std::vector<float>>()[3];

	return info;
}