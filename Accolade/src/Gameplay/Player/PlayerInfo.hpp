#pragma once
#include <string>
#include "./../../Utils/UUID.hpp"
#include "./../../3rd/json.hpp"

struct PlayerInfo {
	static constexpr auto BOLOSS = "char_boloss";

	float maxLife{ 0.f };
	float speed{ 0.f };
	float radius{ 0.f };
	float dashRange{ 0.f };
	float specialSpeed{ 0.f };
	float invincibilityTime{ 0.f };
	float jumpHeight{ 0.f };

	std::string name{ "" };
	std::string sprite{ "" };

	UUID weapon{ UUID::null };

	PlayerInfo();
	PlayerInfo(nlohmann::json json);
};