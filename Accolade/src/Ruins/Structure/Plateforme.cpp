#include "Plateforme.hpp"
#include "./../../Managers/AssetsManager.hpp"

Plateforme::Plateforme(PlateformeInfo info) noexcept : 
	_info(info)
{
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
	/*sf::RectangleShape rectangle{ _info.rectangle.size };
	rectangle.setPosition(_info.rectangle.pos);
	rectangle.setFillColor({ 255, 30, 30, 255 });
	target.draw(rectangle);*/
}


PlateformeInfo PlateformeInfo::loadJson(const nlohmann::json& json) noexcept {
	PlateformeInfo info;
	info.rectangle = Rectangle2f::loadJson(json.at("rectangle"));
	return info;
}

nlohmann::json PlateformeInfo::saveJson(const PlateformeInfo& info) noexcept {
	nlohmann::json json;
	json["rectangle"] = Rectangle2f::saveJson(info.rectangle);
	return json;
}