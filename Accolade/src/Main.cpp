/**
	L'op�ration Accolade est un assaut amphibie britannique,
	planifi� sur l'�le de Rhodes et les �les du Dod�can�se dans la mer �g�e,
	durant la Seconde Guerre mondiale.

	Pr�conis�e par Winston Churchill comme un suivi de la prise de la Sicile en 1943,
	cette op�ration n'est jamais ex�cut�e.
*/



#include <cstdio>
#include <iostream>
#include <typeinfo>
#include <chrono>

#include <SFML/Graphics.hpp>

#include <functional>
#include <queue>
#include "Managers/MemoryManager.hpp"
#include "Managers/AssetsManager.hpp"
#include "Managers/InputsManager.hpp"
#include "Managers/TimerManager.hpp"
#include "Managers/EventManager.hpp"

#include "brx/Parser.hpp"

#include "Game.hpp"
#include "Gameplay/Patterns.hpp"
#include "System/Window.hpp"

#include "Metaprogramming/TypeList.hpp"

#include "Utils/meta_algorithms.hpp"

#include "Graphics/GUI/Switcher.hpp"

#include "OS/PathDefinition.hpp"

#include "Ruins/Instance.hpp"
#include "Entity/EntityStore.hpp"
#include "RTTI/dyn_struct.hpp"



double C::DT{ 0.0 };
std::default_random_engine C::RD(SEED);
std::uniform_real_distribution<float> C::unitaryRng(0.f, 1.f - FLT_EPSILON);
std::shared_ptr<Game> C::game;
const std::filesystem::path C::ASSETS_PATH{ "res/" };
// @Release change this to get_executable_dir
const std::filesystem::path C::EXE_DIR = std::filesystem::current_path();

sf::RenderWindow* C::render_window{ nullptr };
EntityStore* C::es_instance{ nullptr };

void cleanup();
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
	C::game->update(dt > 0.016 ? 0.016 : dt);
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

	//cleanup();
	return 0;
}

void startGame() {
	C::es_instance = new EntityStore();
	Patterns::_json = AssetsManager::getJson(JSON_KEY).at("patterns");
	C::game = std::make_shared<Game>();
	game->start();

	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT, 24), "Boss room");
	window.setKeyRepeatEnabled(false);
	window.setFramerateLimit(0);
	render_window = &window;

	const auto& updateKey = TimerManager::addFunction(
		MIN_MS,
		std::bind(&update, std::ref(window), std::placeholders::_1)
	);
	Clock fpsClock;

	const auto& renderKey = TimerManager::addFunction(
		0,
		[&window](auto) { return render(window); }
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

	AssetsManager::loadJson(JSON_KEY, (ASSETS_PATH / "config.json").string());
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
		const std::filesystem::path path = it.value()["sheet"].get<std::string>();
		bool smooth = (it.value().count("smooth") ? it.value().at("smooth") : true);
		bool repeat = (it.value().count("repeat") ? it.value().at("repeat") : true);
		AssetsManager::loadTexture(key, (ASSETS_PATH / path).string());
		AM::getTexture(key).setSmooth(smooth);
		AM::getTexture(key).setRepeated(repeat);
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
		const std::filesystem::path path = it.value().get<std::string>();

		AssetsManager::loadSound(key, (ASSETS_PATH / path).string());
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
		const std::filesystem::path path = it.value().get<std::string>();
		AssetsManager::loadFont(key, (ASSETS_PATH / path).string());
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
		const std::filesystem::path path = it.value().get<std::string>();
		AssetsManager::loadJson(key, (ASSETS_PATH / path).string());
	}
}

void cleanup() {
	if (es_instance) {
		delete es_instance;
		es_instance = nullptr;
	}
}