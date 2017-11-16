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


std::default_random_engine C::RD(SEED);
std::uniform_real_distribution<float> C::unitaryRng(0.f, 1.f - FLT_EPSILON);
std::shared_ptr<Game> C::game;

void startGame();
void loadRessources();

void loadSpriteFromJson(const nlohmann::json& json);
void loadSoundsFromJson(const nlohmann::json& json);
void loadFontsFromJson(const nlohmann::json& json);

bool update(sf::RenderWindow& window, double dt) {
	InputsManager::update(window);
	C::game->update((float)(dt > MIN_MS ? dt : MIN_MS));
	return false;
}

bool render(sf::RenderWindow& window) {
	static u64 i = 0;
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
}

int main(int, char**) {

	/*if (!glfwInit()) {
		printf("couldn't init glfw");
		assert(true);
	}*/

	//let's try to fit our game into 16MiB
	MemoryManager::I().initialize_buffer(1024 * 1024 * 16);
	loadRessources();
	startGame();

	//glfwTerminate();
	return 0;
}

void startGame() {
	Patterns::_json = AssetsManager::getJson(JSON_KEY)["patterns"];
	C::game = MemoryManager::make_shared<Game>();
	game->start();

	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT, 24), "Boss room");
	window.setKeyRepeatEnabled(false);
	window.setFramerateLimit(0);

	const auto& updateKey = TimerManager::addFunction(
		MIN_MS,
		"update",
		std::bind(&update, std::ref(window), std::placeholders::_1)
	);
	const auto& renderKey = TimerManager::addFunction(
		MAX_FPS == 0.f ? 0.f : 1.f / MAX_FPS,
		"render",
		std::bind(&render, std::ref(window))
	);
	const auto& sizeKey = TimerManager::addFunction(
		10.f,
		"size",
		[](double) -> bool {
			const auto buffer_size = MM::I().get_buffer_size();
			const auto free_size = MM::I().get_free_size();
			printf("Size of MM's buffer: %u \t bytes\n", buffer_size);
			printf("Size used by MM:     %u \t bytes\n", buffer_size - free_size);
			return false;
		}
	);

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
	TimerManager::removeFunction(sizeKey);
}
void loadRessources() {
	printf("Loading jsons...\n");

	AssetsManager::loadJson(JSON_KEY, JSON_PATH);
	loadSpriteFromJson(AssetsManager::getJson(JSON_KEY));
	loadSoundsFromJson(AssetsManager::getJson(JSON_KEY));
	loadFontsFromJson(AssetsManager::getJson(JSON_KEY));
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
