#include "Source.hpp"

#include "Managers/AssetsManager.hpp"

#include "Ruins/Section.hpp"

#include "Gameplay/Magic/Spells/SpellBoomerang.hpp"

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
	info_(info)
{
	pos = info_.pos;
	sprite_.setTexture(AM::getTexture(info_.sprite));
}

void Source::enter(Section* section) noexcept {
	section_ = section;
}
void Source::exit() noexcept {
	section_ = nullptr;
}

void Source::update(double) noexcept {}

void Source::render(sf::RenderTarget& target) noexcept {
	double ratio = (double)sprite_.getTextureRect().width / sprite_.getTextureRect().height;

	sprite_.setScale(
		info_.size.fitUpRatio(ratio).x / sprite_.getTextureRect().width,
		info_.size.fitUpRatio(ratio).y / sprite_.getTextureRect().height
	);
	sprite_.setPosition(pos);

	target.draw(sprite_);
}
