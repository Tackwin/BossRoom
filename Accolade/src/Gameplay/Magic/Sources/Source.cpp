#include "Source.hpp"

#include "Managers/AssetsManager.hpp"

#include "Ruins/Section.hpp"

#include "Gameplay/Magic/Spells/SpellTarget.hpp"

SourceInfo SourceInfo::loadJson(nlohmann::json json) noexcept {
	SourceInfo info;

#define X(x, y) if (auto i = json.find(#x); i != json.end()) { info.x = y(*i); }
	X(id,);
	X(reloadTime,);
	X(kind, (SourceInfo::Kind));
	X(pos, Vector2f::loadJson);
	X(size, Vector2f::loadJson);
	X(origin, Vector2f::loadJson);
	X(sprite, [](auto a) {return a.get<std::string>(); });
#undef X

	return info;
}

nlohmann::json SourceInfo::saveJson(SourceInfo info) noexcept {
	nlohmann::json json;
	json["id"]			= info.id;
	json["sprite"]		= info.sprite;
	json["kind"]		= (int)info.kind;
	json["reloadTime"]	= info.reloadTime;
	json["pos"]			= Vector2f::saveJson(info.pos);
	json["size"]		= Vector2f::saveJson(info.size);
	json["origin"]		= Vector2f::saveJson(info.origin);
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
	sf::CircleShape point{ 0.05f };
	point.setOrigin(point.getRadius(), point.getRadius());
	point.setPosition(pos);
	
	double ratio = (double)sprite_.getTextureRect().width / sprite_.getTextureRect().height;

	sprite_.setOrigin(
		info_.origin.x * sprite_.getTextureRect().width,
		info_.origin.y * sprite_.getTextureRect().height
	);
	sprite_.setScale(
		info_.size.fitUpRatio(ratio).x / sprite_.getTextureRect().width,
		info_.size.fitUpRatio(ratio).y / sprite_.getTextureRect().height
	);
	sprite_.setPosition(pos);

	target.draw(sprite_);
	target.draw(point);
}

void Source::remove() noexcept {
	_remove = true;
}

bool Source::toRemove() const noexcept {
	return _remove;
}