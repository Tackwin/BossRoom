
#include <thread>

#include <SFML/Graphics.hpp>

#include "Managers/AssetsManager.hpp"
#include "Managers/InputsManager.hpp"
#include "Managers/TimerManager.hpp"
#include "Gameplay/Game.hpp"
#include "Gameplay/Patterns.hpp"

std::map<std::string, std::shared_ptr<Function>> TimerManager::_functions;	//Initializing here just to make sure that it is destroyed at the END END of the program
																			//(particularly after the destructor of everything)
																			//TODO: make tihs more robust
std::default_random_engine C::RNG(SEED);
std::uniform_real_distribution<float> C::unitaryRng(0.f, 1.f - FLT_EPSILON);
std::shared_ptr<Game> C::game;
nlohmann::json Patterns::_json;

void loadSpriteFromJson(const nlohmann::json& json);
void loadSoundsFromJson(const nlohmann::json& json);
void loadFontsFromJson(const nlohmann::json& json);

int main(int, char**) {
	printf("Loading jsons...\n");

	AssetsManager::loadJson(JSON_KEY, JSON_PATH);
	loadSpriteFromJson(AssetsManager::getJson(JSON_KEY));
	loadSoundsFromJson(AssetsManager::getJson(JSON_KEY));
	loadFontsFromJson(AssetsManager::getJson(JSON_KEY));

	Patterns::_json = AssetsManager::getJson(JSON_KEY)["patterns"];
	C::game = std::make_shared<Game>();
	game->start();

	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT, 24), "Boss room");

	const auto& updateKey = TimerManager::addFunction(MIN_MS, "update", [&window](auto dt)mutable->bool {
		InputsManager::update(window);
		C::game->update((float)(dt > MIN_MS ? dt : MIN_MS));
		return false;
	});
	const auto& renderKey = TimerManager::addFunction(MAX_FPS == 0.f ? 0.f : 1.f / MAX_FPS, "render", [&window](auto)mutable->bool {
		static uint64_t i = 0;
		Clock c;
		if (window.isOpen()) {
			window.clear(sf::Color(50, 50, 50));
			C::game->render(window);
			window.display();
		}
		if (i++ >= ((MAX_FPS == 0) ? 1'000 : MAX_FPS)) {
			//const auto& dt = c.elapsed() * 1'000'000.0;
			//printf("Rendered in %u us.\n", static_cast<uint32_t>(dt));
			i = 0;
		}
		return false;
	});

	while (window.isOpen()) {
		static sf::Clock dtClock;
		static float dt = 0;
		static float sum = 0;
		dt = dtClock.restart().asSeconds();
		dt = dt < MIN_DELTA ? dt : MIN_DELTA;

		TimerManager::update(dt);
	}
	C::game.reset();

	TimerManager::removeFunction(renderKey);
	TimerManager::removeFunction(updateKey);
	return 0;
}

void loadSpriteFromJson(const nlohmann::json& json) {
	assert(!json["sprites"].is_null());
	const auto& j = json["sprites"];
	const uint32_t n = j.size();

	printf("Loading %u textures from json...\n", n);

	uint32_t i = 1u;
	for (auto it = j.begin(); it != j.end(); ++it, ++i) {
		printf("\t%u/%u ", i, n);

		assert(!it.value()["sheet"].is_null());

		const std::string& key = it.key();
		const std::string& path = it.value()["sheet"];
		AssetsManager::loadTexture(key, ASSETS_PATH + path);
	}
}

void loadSoundsFromJson(const nlohmann::json& json) {
	assert(!json["sounds"].is_null());
	const auto& j = json["sounds"];
	const uint32_t n = j.size();

	printf("Loading %u sounds from json...\n", n);

	uint32_t i = 1u;
	for (auto it = j.begin(); it != j.end(); ++it, ++i) {
		printf("\t%u/%u ", i, n);

		const std::string& key = it.key();
		const std::string& path = it.value();
		AssetsManager::loadSound(key, ASSETS_PATH + path);
	}
}

void loadFontsFromJson(const nlohmann::json& json) {
	assert(!json["fonts"].is_null());
	const auto& j = json["fonts"];
	const uint32_t n = j.size();

	printf("Loading %u fonts from json...\n", n);

	uint32_t i = 1u;
	for (auto it = j.begin(); it != j.end(); ++it, ++i) {
		printf("\t%u/%u ", i, n);

		const std::string& key = it.key();
		const std::string& path = it.value();
		AssetsManager::loadFont(key, ASSETS_PATH + path);
	}
}
