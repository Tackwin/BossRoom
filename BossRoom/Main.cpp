#include <thread>

#include <SFML/Graphics.hpp>

#include "AssetsManager.hpp"
#include "InputsManager.hpp"
#include "TimerManager.hpp"
#include "Patterns.hpp"
#include "Systems.hpp"
#include "Const.hpp"
#include "Game.hpp"

sf::Color sf::Color::hexToColor(const std::string& hex) {
	return sf::Color(
		(sf::Uint8)std::stoi(std::string(hex.data() + 2, 2), NULL, 16),
		(sf::Uint8)std::stoi(std::string(hex.data() + 4, 2), NULL, 16),
		(sf::Uint8)std::stoi(std::string(hex.data() + 6, 2), NULL, 16),
		(sf::Uint8)std::stoi(std::string(hex.data() + 0, 2), NULL, 16)
	);
}

std::map<std::string, std::shared_ptr<Function>> TimerManager::_functions;	//Initializing here just to make sure that it is destroyed at the END END of the program
																			//(particularly after the destructor of everything)
																			//TODO: make tihs more robust
std::default_random_engine C::RNG(SEED);
std::uniform_real_distribution<float> C::unitaryRng(0.f, 1.f - FLT_EPSILON);
std::shared_ptr<Game> C::game;
nlohmann::json Patterns::_json;
//Ecs C::ecs;

//decltype(System::views) System::views; //Does that really work ?? Is it bad practice ??
//decltype(System::currentView) System::currentView;;

int main(int, char**) {
	printf("Loading jsons...\n");
	assert(AssetsManager::loadJson(JSON_KEY, JSON_PATH));
	printf("Loading fonts...\n");
	assert(AssetsManager::loadFont("consola", ASSETS_PATH "consola.ttf"));
	printf("Loading sound...\n");
	assert(AssetsManager::loadSound("shoot", ASSETS_PATH "shoot.wav"));
	assert(AssetsManager::loadSound("hit", ASSETS_PATH "hit.wav"));
	printf("Loading textures...\n");
	assert(AssetsManager::loadTexture("aim", ASSETS_PATH "images/aim.png"));
	
	Patterns::_json = AssetsManager::getJson(JSON_KEY)["patterns"];
	C::game = std::make_shared<Game>();

	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT, 24), "Boss room");
	//System::views["default"] = window.getDefaultView();

	const auto& updateKey = TimerManager::addFunction(0, "update", [](float dt)->bool {
		C::game->update(dt);
		return false;
	});
	const auto& renderKey = TimerManager::addFunction(0.01666666f, "render", [&window](float)mutable->bool {
		if (window.isOpen()) {
			window.clear(sf::Color(50, 50, 50));
			C::game->render(window);
			window.display();
		}
		return false;
	});

	while (window.isOpen()) {
		static sf::Clock dtClock;
		static float dt = 0;
		static float sum = 0;
		dt = dtClock.restart().asSeconds();
		dt = dt < MIN_DELTA ? dt : MIN_DELTA;

		InputsManager::update(window);
		TimerManager::update(dt);
	}
	C::game.reset();

	//ecs.reset();
	TimerManager::removeFunction(renderKey);
	TimerManager::removeFunction(updateKey);
	return 0;
}