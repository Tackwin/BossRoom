#pragma once
#include <random>
#include <memory>

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

	constexpr unsigned int WIDTH = 1280;
	constexpr unsigned int HEIGHT = 720;

	constexpr unsigned int MAX_FPS = 60;

	constexpr float MIN_DELTA = 1 / 30.f;
	constexpr float MIN_MS = 0.000f;

	constexpr float G = 2'500.f;

	constexpr float SOUND_LEVEL = 0.05f;

	constexpr double PId = 3.1415926535897923846;
	constexpr float PIf = 3.1415926f;

	constexpr u32 N_LEVEL = 5u;

	extern std::default_random_engine RNG;
	extern std::uniform_real_distribution<float> unitaryRng;
	constexpr unsigned int SEED = 0;

	extern std::shared_ptr<Game> game;
};
using namespace C;