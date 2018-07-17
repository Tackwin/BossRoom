#include <cstdio>

#include <SFML/Graphics.hpp>

#include <GLFW/glfw3.h>

#include <functional>
#include <queue>

#include "Managers/MemoryManager.hpp"
#include "Managers/AssetsManager.hpp"
#include "Managers/InputsManager.hpp"
#include "Managers/TimerManager.hpp"
#include "Managers/EventManager.hpp"

#include "Game.hpp"
#include "Gameplay/Patterns.hpp"
#include "System/Window.hpp"

double C::DT{ 0.0 };
std::default_random_engine C::RD(SEED);
std::uniform_real_distribution<float> C::unitaryRng(0.f, 1.f - FLT_EPSILON);
std::shared_ptr<Game> C::game;

void startGame();
void loadRessources();

void loadSpriteFromJson(const nlohmann::json& json);
void loadSoundsFromJson(const nlohmann::json& json);
void loadFontsFromJson(const nlohmann::json& json);
void loadJsonsFromJson(const nlohmann::json& json);

bool update(sf::RenderWindow& window, double dt) {
	InputsManager::update(window);
	//if (dt - FIXED_DT == dt) return false;
	C::DT = dt;
	C::game->update(dt);
	return false;
}

bool render(sf::RenderWindow& window) {
	Clock c;
	if (window.isOpen()) {
		window.clear(sf::Color(50, 50, 50));
		C::game->render(window);
		window.display();
	}
	return false;
}

int main(int, char**) {

	/*if (!glfwInit()) {
		printf("couldn't init glfw");
		assert(true);
	}*/

	//let's try to fit our game into 16MiB
	//MemoryManager::I().initialize_buffer(1024 * 1024 * 16);
	loadRessources();
	startGame();

	//glfwTerminate();
	return 0;
}

void startGame() {
	Patterns::_json = AssetsManager::getJson(JSON_KEY).at("patterns");
	C::game = std::make_shared<Game>();
	game->start();

	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT, 24), "Boss room");
	window.setKeyRepeatEnabled(false);
	window.setFramerateLimit(0);

	const auto& updateKey = TimerManager::addFunction(
		MIN_MS,
		std::bind(&update, std::ref(window), std::placeholders::_1)
	);
	Clock fpsClock;
	const auto& renderKey = TimerManager::addFunction(
		0,
		std::bind(&render, std::ref(window))
	);
	
	while (window.isOpen()) {
		static Clock dtClock;

		const double dt = dtClock.restart();

		TM::update(dt);
	}
	C::game.reset();

	TimerManager::removeFunction(renderKey);
	TimerManager::removeFunction(updateKey);
}
void loadRessources() {
	printf("Loading jsons...\n");

	AssetsManager::loadJson(JSON_KEY, JSON_PATH);
	loadSpriteFromJson(AssetsManager::getJson(JSON_KEY));
	loadSoundsFromJson(AssetsManager::getJson(JSON_KEY));
	loadFontsFromJson(AssetsManager::getJson(JSON_KEY));
	loadJsonsFromJson(AM::getJson(JSON_KEY));
}
void loadSpriteFromJson(const nlohmann::json& json) {
	assert(!json["sprites"].is_null());
	const auto& j = json["sprites"];
	const u32 n = j.size();

	printf("Loading %u textures from json...\n", n);

	u32 i = 1u;
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
	const u32 n = j.size();

	printf("Loading %u sounds from json...\n", n);

	u32 i = 1u;
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
	const u32 n = j.size();

	printf("Loading %u fonts from json...\n", n);

	u32 i = 1u;
	for (auto it = j.begin(); it != j.end(); ++it, ++i) {
		printf("\t%u/%u ", i, n);

		const std::string& key = it.key();
		const std::string& path = it.value();
		AssetsManager::loadFont(key, ASSETS_PATH + path);
	}
}

void loadJsonsFromJson(const nlohmann::json& json) {
	assert(!json["jsons"].is_null());
	const auto& j = json["jsons"];
	const u32 n = j.size();

	printf("Loading %u jsons from json...\n", n);

	u32 i = 1u;
	for (auto it = j.begin(); it != j.end(); ++it, ++i) {
		printf("\t%u/%u ", i, n);

		const std::string& key = it.key();
		const std::string& path = it.value();
		AssetsManager::loadJson(key, ASSETS_PATH + path);
	}
}