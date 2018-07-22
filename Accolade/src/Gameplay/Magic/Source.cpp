#include "Source.hpp"

#include "./../../Managers/AssetsManager.hpp"

SourceInfo SourceInfo::loadJson(nlohmann::json json) noexcept {
	SourceInfo info;
	info.id = json.at("id");
	info.reloadTime = json.at("reloadTime");
	info.pos = Vector2f::loadJson(json.at("pos"));
	info.size = Vector2f::loadJson(json.at("size"));
	info.sprite = json.at("sprite").get<std::string>();
	return info;
}
nlohmann::json SourceInfo::saveJson(SourceInfo info) noexcept {
	nlohmann::json json;
	json["id"] = info.id;
	json["reloadTime"] = info.reloadTime;
	json["pos"] = Vector2f::saveJson(info.pos);
	json["size"] = Vector2f::saveJson(info.size);
	json["sprite"] = info.sprite;
	return json;
}

Source::Source(SourceInfo info) noexcept : 
	Object(),
	_info(info)
{
	pos = _info.pos;
	_sprite.setTexture(AM::getTexture(_info.sprite));
}

void Source::update(double) noexcept {}
void Source::render(sf::RenderTarget& target) const noexcept {

	double ratio = (double)_sprite.getTextureRect().width / _sprite.getTextureRect().height;

	_sprite.setScale(
		_info.size.fitUpRatio(ratio).x / _sprite.getTextureRect().width,
		_info.size.fitUpRatio(ratio).y / _sprite.getTextureRect().height
	);
	_sprite.setPosition(pos);

	target.draw(_sprite);
}