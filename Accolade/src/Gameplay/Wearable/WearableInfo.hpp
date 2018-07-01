#pragma once
#include <SFML/Graphics.hpp>

#include "./../../3rd/json.hpp"
#include "./../../Utils/UUID.hpp"

class Wearable;
struct WearableInfo {

	using OnUpdate = std::function<void(double, Wearable&)>;
	using OnUnmount = std::function<void(Wearable&)>;
	using OnMount = std::function<void(Wearable&)>;

	std::string name{ "" };

	nlohmann::json configuration;

	OnMount onMount = [](auto&) {};
	OnUnmount onUnmount = [](auto&) {};
	OnUpdate onUpdate = [](auto, auto&) {};

	std::string uiTexture;
	unsigned cost{ 0u };
};