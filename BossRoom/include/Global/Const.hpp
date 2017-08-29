#pragma once
#include <enTT/registry.hpp>
#include <cstdint>
#include <string>
#include <random>
#include <memory>
#include <vector>
#include <mutex>
#include <map>

#include "3rd/json.hpp"

#define ASSETS_PATH "res/"

using int8 = std::int8_t;
using uint8 = std::uint8_t;
using int16 = std::int16_t;
using uint16 = std::uint16_t;
using int32 = std::int32_t;
using uint32 = std::uint32_t;
using int64 = std::int64_t;
using uint64 = std::uint64_t;

class Game;
namespace C {

	constexpr char TITLE[] = "Boss room";
	constexpr char JSON_KEY[] = "json";
	constexpr char JSON_PATH[] = ASSETS_PATH"config.json";

	constexpr unsigned int WIDTH = 1280;
	constexpr unsigned int HEIGHT = 720;

	constexpr unsigned int MAX_FPS = 0;

	constexpr unsigned int N_LEVEL = 5;

	constexpr float MIN_DELTA = 1 / 30.f;
	constexpr float MIN_MS = 0.00f;

	constexpr double PId = 3.1415926535897923846;
	constexpr float PIf = 3.1415926f;

	extern std::default_random_engine RNG;
	extern std::uniform_real_distribution<float> unitaryRng;
	constexpr unsigned int SEED = 0;

	extern std::shared_ptr<Game> game;

#ifdef ECS
	extern Ecs ecs;
#endif
	template<typename T>
	T getJsonValue(nlohmann::json json_, std::string key) {
		assert(json_.find(key) != json_.end());
		auto j = json_[key];

		if (j.is_primitive())
			return j;
		if (j.find("type") == j.end())
			j["type"] = "uniform";
		
		if (j.find("real") == j.end())
			j["real"] = true;

		assert(j.find("range") != j.end());
		assert(j.find("mean") != j.end());

		float mean = j["mean"]; 
		float range = j["range"];
		if (j["type"] == "uniform") {
			return static_cast<T>(j["real"].get<bool>() ? 
				std::uniform_real_distribution<float>(mean - range, mean + range)(RNG) :
				std::uniform_int_distribution<int>((int)(mean - range), (int)(mean + range))(RNG)
			);
		} 
		else if (j["type"] == "normal") {
			return static_cast<T>(j["real"].get<bool>() ? 
				std::normal_distribution<float>(mean, range)(RNG) :
				static_cast<int>(std::normal_distribution<float>(mean, range)(RNG))
			);
		} else {
			return j;
		}
	}
};
using namespace C;