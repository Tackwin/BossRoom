#include "LevelScreen.hpp"

#include <tuple>

#include "./../Game.hpp"

#include "./../Common.hpp"

#include "./../Managers/InputsManager.hpp"
#include "./../Managers/AssetsManager.hpp"
#include "./../Managers/TimerManager.hpp"

#include "./../Graphics/GUI/Panel.hpp"
#include "Graphics/GUI/Label.hpp"

#include "./../Gameplay/Projectile.hpp"
#include "./../Gameplay/Wearable/Wearable.hpp"
#include "./../Gameplay/Player/Player.hpp"
#include "./../Gameplay/Level.hpp"

#include "EditSectionScreen.hpp"

LevelScreen::LevelScreen(u32 n) :
	_n(n),
	_bossHealthTileSprite(AssetsManager::getTexture("health_tile")),
	_gameViewPos({ WIDTH * 0.5f, HEIGHT * 0.5f}),
	_gameViewSize({ (float)WIDTH, (float)HEIGHT }),
	_guiView(_gameViewPos, _gameViewSize),
	_gameView(_gameViewPos, _gameViewSize)//,
	//_level(std::make_unique<Level>(*Level::levels[_n]))
{
	constexpr float bossHealthTileSize = 60;
	AssetsManager::getTexture("health_tile").setSmooth(true);
	_bossHealthTileSprite.setOrigin(
		_bossHealthTileSprite.getTextureRect().width * 0.5f, 
		_bossHealthTileSprite.getTextureRect().height * 0.5f
	);
	_bossHealthTileSprite.setScale(
		bossHealthTileSize / _bossHealthTileSprite.getTextureRect().width,
		bossHealthTileSize / _bossHealthTileSprite.getTextureRect().height
	);
	_bossHealthTileSprite.setPosition(35, 35);

	InstanceInfo info = InstanceInfo::generate_graph(10, "res/rooms/working/");;
	instance = std::make_unique<Instance>(info);
	instance->startAt(0);

	std::filesystem::path startRoom = AM::getJson(JSON_KEY)["StartRoom"].get<std::string>();
	AM::loadJson("StartRoom", (ASSETS_PATH / startRoom).string());

	section_id_label = std::make_unique<Label>(nlohmann::json{
		{"pos", {WIDTH - 10, 10}},
		{"origin", {1, 0}},
		{"text", "Section: "},
		{"font", "consola"},
		{"charSize", 8}
	});

	inventory = std::make_unique<Inventory>();
}

LevelScreen::~LevelScreen() {
}

void LevelScreen::onEnter(std::any input) {
	Screen::onEnter(input);
	if (game->getLastScreen() == Screen::edit_screen) {
		auto returnValue = std::any_cast<EditSectionScreen::ReturnType>(input);

		instance->hardSetCurrentSection(std::get<0>(returnValue));

		//_section = std::make_unique<Section>(std::get<0>(returnValue));
		//_section->setFilepath(std::get<1>(returnValue));
	}

	instance->getCurrentSection().enter();
	//_section->enter();
	//_level->start(this);

	auto player = instance->getCurrentSection().getPlayer();

	for (int i = 0; i < player->getPlayerInfo().maxLife; i++) {
		_playerLife.push_back(sf::RectangleShape());
		_playerLife[i].setOutlineColor(sf::Color(10, 10, 30));
		_playerLife[i].setSize({ 40, 40 });
		_playerLife[i].setOutlineThickness(2);
		_playerLife[i].setFillColor(
			i < player->getLife()
				? sf::Color(180, 50, 50)
				: sf::Color(90, 20, 20)
		);
		_playerLife[i].setPosition(
			10 + i * (_playerLife[i].getSize().x + 5),
			(C::HEIGHT - 10) - _playerLife[i].getSize().y
		);
	}
}

std::any LevelScreen::onExit() {
	//if (_level) 
	//	_level->stop();

	instance->getCurrentSection().exit();
	_playerLife.clear();
	return {};
}

void LevelScreen::update(double dt) {
	if (_gameViewPos != _gameView.getCenter())
		_gameView.setCenter(_gameViewPos);
	if (_gameViewSize != _gameView.getSize())
		_gameView.setSize(_gameViewSize);

	auto player = instance->getCurrentSection().getPlayer();

	if (IM::isKeyJustPressed(sf::Keyboard::I)) {
		inventory_is_up = !inventory_is_up;
		if (inventory_is_up) {
			inventory->open();
			inventory->clearItems();

			std::vector<Eid<Item>> items;
			for (auto& x : player->getItems()) {
				items.push_back((Eid<Item>)x);
			}
			inventory->populateItems(items);
		}
		else {
			inventory->quit();
		}
	}

	if (inventory_is_up) {
		inventory->postOrderInput({});
		if (inventory->needToQuit()) inventory_is_up = false;
	}

	for (int i = 0; i < player->getPlayerInfo().maxLife; i++) {
		_playerLife[i].setFillColor(
			i < player->getLife()
				? sf::Color(180, 50, 50) 
				: sf::Color(90, 20, 20)
		);
		_playerLife[i].setPosition(
			10 + i * (_playerLife[i].getSize().x + 5), 
			(C::HEIGHT - 10) - _playerLife[i].getSize().y
		);
	}
	
	instance->update(dt);
	section_id_label->setStdString(
		"Section: " + std::to_string(instance->getCurrentSectionIndex())
	);

	/*if (_level) {
		_level->update(dt);
		if (_level->lost()) {
			game->enterHeritage();
		}
	}*/
	if (IM::isLastSequenceJustFinished({ sf::Keyboard::E }, { sf::Keyboard::LControl })) {
		// if we came from the edit screen then we can just go back to it.
		if (game->getLastScreen() == Screen::edit_screen) {
			game->go_back_screen();
		}
		else {
			C::game->enterScreen(
				std::make_shared<EditSectionScreen>(&instance->getCurrentSection())
			);
		}
		return;
	}
}

void LevelScreen::render(sf::RenderTarget& target) {
	const auto& initialView = target.getView();

	_gameView.setCenter(_gameViewPos + _gameViewOffset);
	target.setView(_gameView);
	//if (_level) _level->render(target);
	instance->render(target);
	instance->renderDebug(target);


	target.setView(_guiView);

	renderGui(target);

	for (auto& i : _playerLife)
		target.draw(i);

	target.setView(initialView);
}

void LevelScreen::renderGui(sf::RenderTarget& target) {
	constexpr u32 nTiles = 10;
	constexpr float padding = -3;
	constexpr std::tuple<float, float, float> colors[] = {
		{ 0.f, 0.f, 0.f },
		{ 0.31f, 0.f, 0.f },
		{ 0.73f, 0.31f, 0.f },
		{ 1.f, 0.73f, 0.31f }
	};
	constexpr u32 nColors = 
		sizeof(colors) / sizeof(std::tuple<float, float, float>) - 1;

	const auto pos = _bossHealthTileSprite.getPosition();
	const float& bossTileWidth = _bossHealthTileSprite.getGlobalBounds().width;
	const float& bossLife = 10; /*
		_level->getNormalizedBossLife() == 1.f 
			? 1.f - FLT_EPSILON 
			: _level->getNormalizedBossLife(); // hack to bypass the problem below*/

	for (u32 i = 0; i < nTiles; ++i) { // This depends on the fact that if a = n*b then  a mod 1 / n == b, need to check if it's true for float
		const u32& color = (u32)(bossLife * nColors) +  // Not true for nColors == 3 :(
		(
			((float)i / (nTiles * nColors)) < fmodf(bossLife, 1.f / nColors) 
				? 1 
				: 0
		);
		const auto& [r, g, b] = colors[color];

		_bossHealthTileSprite.setColor(sf::Color(
			(u08)(r * 255), 
			(u08)(g * 255), 
			(u08)(b * 255)
		));
		target.draw(_bossHealthTileSprite);

		_bossHealthTileSprite.move(bossTileWidth + padding, 0);
	}
	_bossHealthTileSprite.setPosition(pos);
	
	auto player = instance->getCurrentSection().getPlayer();

	auto weapon = player->getWeapon();
	if (weapon) {
		auto texture = Wearable::GetWearableinfo(weapon.value()).uiTexture;
		sf::Sprite weaponGuiSprite(AM::getTexture(texture));
		weaponGuiSprite.setPosition({ WIDTH - 50.f, HEIGHT - 50.f });
		target.draw(weaponGuiSprite);
	}

	section_id_label->propagateRender(target);

	if (inventory_is_up) {
		inventory->propagateRender(target);
	}
}

void LevelScreen::shakeScreen(float power) {
	constexpr u32 nShakes = 5;
	constexpr float iTimeShakes = 0.03f;

	const auto& lambda = [&, p = power, n = nShakes](double)mutable->bool {
		Vector2f from = _gameViewOffset;
		Vector2f to = Vector2f::createUnitVector(unitaryRng(RD) * 2 * PIf) * power;

		if (n-- <= 0) {
			TimerManager::addLinearEase(
				iTimeShakes,
				&_gameViewOffset,
				_gameViewOffset,
				{ 0, 0 }
			);
		}
		else {
			TimerManager::addLinearEase(
				iTimeShakes,
				&_gameViewOffset,
				from,
				to
			);
		}

		return n == 0;
	};

	TimerManager::addFunction(iTimeShakes, lambda);

}

u32 LevelScreen::getIndex() const {
	return _n;
}
