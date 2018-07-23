#include "Game.hpp"

#include "Common.hpp"

#include "Managers/AssetsManager.hpp"
#include "Managers/InputsManager.hpp"
#include "Managers/MemoryManager.hpp"
#include "Managers/TimerManager.hpp"

#include "Screens/RoomSelectorScreen.hpp"
#include "Screens/HeritageScreen.hpp"
#include "Screens/LevelScreen.hpp"
#include "Screens/StartScreen.hpp"

#include "Gameplay/Wearable/Wearable.hpp"
#include "Gameplay/Player/Player.hpp"
#include "Gameplay/Level.hpp"
#include "Gameplay/Boss.hpp"

#include "Options/KeyBindings.hpp"

void Game::start() {
	enterScreen(std::make_shared<StartScreen>());
}

void Game::enterDungeon() {
	enterRoom(0u);
}

void Game::nextRoom() {
	if (
		auto screen = dynamic_cast<LevelScreen*>(_screens.top().get()); 
		screen
	) {
		enterRoom(screen->getIndex() + 1);
	}
}

void Game::render(sf::RenderTarget& target) {
	auto initialView = target.getView();

	_screens.top()->render(target);
	target.setView(initialView);
	for (auto& [_, v] : _debugText) {_;
		target.draw(v);
	}

	target.setView(initialView);
}

void Game::updateDebugText(double) {
	static Clock fpsClock;
	static double avgMs = 0;
	static int avgN = 0;
	static constexpr int avgRange = MAX_FPS == 0 ? 100 : MAX_FPS;

	avgMs += fpsClock.restart();
	if (++avgN > avgRange) {
		u32 fps = (u32)(avgRange / (avgMs == .0f ? 1 : avgMs));
		u32 µs = (u32)(1000'000 * avgMs / avgRange); // :)
		_debugText["ups"].setString(
			"Fps: " + std::to_string(fps) + "\tµs: " + std::to_string(µs)
		);

		avgN = 0;
		avgMs = 0;
	}
}

Screen::Type Game::getLastScreen() const noexcept {
	return _lastScreen;
}

void Game::update(double dt) noexcept {
	updateDebugText(dt);
	_screens.top()->update(dt);
}

void Game::exitScreen() {
	pop();
	if (!_screens.empty())
		_screens.top()->onEnter(_returnedFromScreen);
}

void Game::enterScreen(std::shared_ptr<Screen> s) {
	if (!_screens.empty()) {
		_lastScreen = _screens.top()->getType();
		_returnedFromScreen = _screens.top()->onExit();
	}
	s->onEnter(_lastScreen);
	_screens.push(s);
}

void Game::enterRoom(u32 n) {
	exitScreen();
	enterScreen(std::make_shared<LevelScreen>(n));
}

Game::~Game() {
	for (size_t i = _screens.size(); i > 0u; --i) {
		pop();
	}

	Level::destroyLevels();
	Boss::destroyBosses();
}

Game::Game()
{
	Wearable::InitWearable();
	Boss::createBosses();
	Level::createLevels();

	_debugText["ups"].setFont(AssetsManager::getFont("consola"));
	_debugText["ups"].setCharacterSize(20);
	_debugText["ups"].setPosition(5, 5);
	_debugText["ups"].setFillColor(sf::Color(100, 100, 100));

    _keyBindings.emplace_back(AM::getJson("default_key"));
	_currentKeyBindings = 0;

	_playerInfo = loadPlayerInfo(PlayerInfo::BOLOSS);
}

void Game::pop() {
	if (_screens.empty()) return;

	_lastScreen = _screens.top()->getType();
	_returnedFromScreen = _screens.top()->onExit();
	_screens.pop();
}

void Game::enterHeritage() {
	exitScreen();
	auto s = std::make_shared<HeritageScreen>(getPlayerInfo());
	enterScreen(s);
}

const PlayerInfo& Game::getPlayerInfo() const noexcept {
	return _playerInfo;
}

void Game::setPlayerInfo(const PlayerInfo& playerInfo) noexcept {
	_playerInfo = playerInfo;
}
const KeyBindings& Game::getCurrentKeyBindings() const noexcept {
	return _keyBindings[_currentKeyBindings];
}

PlayerInfo Game::loadPlayerInfo(std::string character) const noexcept {
	auto characters = AM::getJson(JSON_KEY)["player"]["characters"];
	auto characterPath = characters.at(character).get<std::string>();

	if (!AM::haveJson(character)) {
		auto b = AM::loadJson(character, ASSETS_PATH + characterPath); assert(b);
	}

	auto json = AM::getJson(character);

	return PlayerInfo(json);
}
