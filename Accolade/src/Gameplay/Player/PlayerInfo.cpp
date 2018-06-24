#include "PlayerInfo.hpp"

PlayerInfo::PlayerInfo() {

}

PlayerInfo::PlayerInfo(nlohmann::json json) :
	maxLife(json["max_life"]),
	speed(json["speed"]),
	radius(json["radius"]),
	dashRange(json["dash_range"]),
	specialSpeed(json["special_speed"]),
	invincibilityTime(json["invincibility_time"]),
	jumpHeight(json["jump_height"]),
	name(json["name"].get<std::string>()),
	sprite(json["sprite"].get<std::string>())
{}