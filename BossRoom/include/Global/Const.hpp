#pragma once
#include <enTT/registry.hpp>
#include <type_traits>
#include <cstdint>
#include <variant>
#include <string>
#include <random>
#include <memory>
#include <vector>
#include <mutex>
#include <map>

#include "3rd/json.hpp"

#define ASSETS_PATH "res/"

#include<SFML/Window.hpp>

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

	constexpr unsigned int MAX_FPS = 60;

	constexpr unsigned int N_LEVEL = 5;

	constexpr float MIN_DELTA = 1 / 30.f;
	constexpr float MIN_MS = 0.000f;

	constexpr float G = 2'500.f;

	constexpr double PId = 3.1415926535897923846;
	constexpr float PIf = 3.1415926f;

	extern std::default_random_engine RNG;
	extern std::uniform_real_distribution<float> unitaryRng;
	constexpr unsigned int SEED = 0;

	extern std::shared_ptr<Game> game;

#ifdef ECS
	extern Ecs ecs;
#endif
	
};
using namespace C;