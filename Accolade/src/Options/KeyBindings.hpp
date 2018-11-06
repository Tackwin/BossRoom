#pragma once

#include "./../3rd/json.hpp"

#include <string>

// >TODO
// Seriously i don't know what i was thinking, it's not even compile time strings...
// Enum this shit.
class KeyBindings {
public:
	static constexpr auto MOVE_UP = "move_up";
	static constexpr auto MOVE_LEFT = "move_left";
	static constexpr auto MOVE_DOWN = "move_down";
	static constexpr auto MOVE_RIGHT = "move_right";
	static constexpr auto CONSUME_BINDED = "consume_binded";
	static constexpr auto ACTION = "action";
	static constexpr auto DASH = "dash";
	static constexpr auto SPECIAL_SPEED = "special_speed";
	static constexpr auto JUMP = "jump";

	using Command = decltype(MOVE_DOWN);
public:
	KeyBindings();
	KeyBindings(nlohmann::json config);

	int getKey(Command command) const noexcept;
private:
	nlohmann::json _config;
};