#pragma once
#include <random>
#include <memory>
#include <type_traits>

#include "3rd/json.hpp"

#define ASSETS_PATH "res/"

using i08 = std::int8_t;
using u08 = std::uint8_t;
using i16 = std::int16_t;
using u16 = std::uint16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;
using i64 = std::int64_t;
using u64 = std::uint64_t;

class Game;
namespace C {

	constexpr char TITLE[] = "Boss room";
	constexpr char JSON_KEY[] = "json";
	constexpr char JSON_PATH[] = ASSETS_PATH"config.json";

	constexpr u32 WIDTH = 1280;
	constexpr u32 HEIGHT = 800;
	constexpr double RATIO = (double)WIDTH / (double)HEIGHT;

	constexpr u32 MAX_FPS = 60;

	constexpr float MIN_DELTA = 1 / 30.f;
	constexpr float MIN_MS = 0.000f;

	constexpr float G = 100.f;

	constexpr float SOUND_LEVEL = 0.2f;

	constexpr double FIXED_DT = 0.001;

	constexpr double PId = 3.1415926535897923846;
	constexpr float PIf = 3.1415926f;

	constexpr u32 N_LEVEL = 5u;

	extern double DT;

	extern std::default_random_engine RD;
	extern std::uniform_real_distribution<float> unitaryRng;
	constexpr u32 SEED = 0;

	extern std::shared_ptr<Game> game;

	template<typename T> 
	T rngRange(T min, T max) {
		return static_cast<T>(min + unitaryRng(RD) * (max - min));
	}

	template<typename T>
	T getJsonValue(nlohmann::json json_, std::string key) noexcept {
		auto j = json_.at(key);

		if (j.is_primitive() || std::is_same<T, nlohmann::json>::value)
			return j;
		if (j.find("type") == j.end())
			j["type"] = "uniform";

		if (j.find("real") == j.end())
			j["real"] = true;

		float mean = j.at("mean");
		float range = j.at("range");

		T rng = static_cast<T>(mean);
		if (j["type"] == "uniform") {
			rng = static_cast<T>(j["real"].get<bool>() ?
				std::uniform_real_distribution<double>(mean - range, mean + range)(RD) :
				std::uniform_int_distribution<long long>((long long)(mean - range), (long long)(mean + range))(RD)
				);
		}
		else if (j["type"] == "normal") {
			rng = static_cast<T>(std::normal_distribution<double>(mean, range)(RD));
		}

		if (auto it = j.find("min"); it != j.end() && rng < it.value().get<T>()) {
			rng = it.value().get<T>();
		}

		return rng;
	}

};
using namespace C;