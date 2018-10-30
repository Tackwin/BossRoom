#pragma once

#include "./../Utils/UUID.hpp"

struct Event {
	static constexpr auto FIRE = "fire";

	void* user_ptr{ nullptr };
};