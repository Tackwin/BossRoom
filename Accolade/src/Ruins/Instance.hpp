#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

#include "3rd/json.hpp"

#include "Math/Vector.hpp"

struct InstanceInfo {

	size_t nSamples{ 0 };
	std::vector<Vector2f> roofSamples;

	static InstanceInfo loadJson(const nlohmann::json& json) noexcept;
	static nlohmann::json saveJson(const InstanceInfo& info) noexcept;

};

class Instance {
public:
	Instance(const InstanceInfo& info) noexcept;

	void runAlgo(sf::RenderWindow& window) noexcept;

	bool rerun{ false };
private:

	void samples(sf::RenderWindow& window) noexcept;
	void smoothSamples(sf::RenderWindow& window) noexcept;

	InstanceInfo info;
	sf::View v;
};