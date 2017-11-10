#include "Gameplay/Level.hpp"

#include "Game.hpp"
#include "Common.hpp"

#include "Managers/InputsManager.hpp"
#include "Managers/AssetsManager.hpp"
#include "Managers/TimerManager.hpp" 

#include "Screens/LevelScreen.hpp"

#include "Graphics/Particle.hpp"

#include "Gameplay/Projectile.hpp"
#include "Gameplay/Weapon.hpp"
#include "Gameplay/Player.hpp"
#include "Gameplay/Probe.hpp"
#include "Gameplay/Boss.hpp"

std::vector<std::shared_ptr<Level>> Level::levels(N_LEVEL);

void Level::createLevels() {
	for (u32 i = 0; i < N_LEVEL; ++i) {
		levels[i] = std::make_shared<Level>(
			MM::make_shared<Boss>(*Boss::bosses[i]), 
			i
		);
	}
}
void Level::destroyLevels() {
	levels.clear();
}

Level::Level(std::shared_ptr<Boss> boss, u32 n) : 
	_boss(boss), 
	_n(n),
	_ended(false),
	_aimSprite(AssetsManager::getTexture("aim")) {
	_aimSprite.setScale(0.7f, 0.7f);
	_aimSprite.setOrigin(
		_aimSprite.getTextureRect().width / 2.f, 
		_aimSprite.getTextureRect().height / 2.f
	);

	_entranceToNext.r = 100.f;
	_entranceToNext.dtPos = { (float)WIDTH, HEIGHT / 2.f };
}
Level::~Level() {
}

void Level::start(LevelScreen* screen_) {
	_screen = screen_;
	_player = game->_player;
	_player->setPos({ 100.f, HEIGHT / 2.f });

	_boss->enterLevel(this);
	_player->enterLevel(this);

	_world.addObject(_player);
	_world.addObject(_boss);

	_floor = MM::make_shared<Object>();
	_floor->idMask.set((size_t)Object::BIT_TAGS::FLOOR);
	_floor->pos = { 0.f, 600.f };
	_floor->collider = &_floorHitBox;
	static_cast<Box*>(_floor->collider)->size = { 3000.f, 120.f };
	static_cast<Box*>(_floor->collider)->onEnter = 
		[p = _player](auto) { p->floored(); };
	_world.addObject(_floor);
}
void Level::stop() {
	_boss->exitLevel();
	_player->exitLevel();

	_projectiles.clear();
	_player.reset();

	_world.purge();

	_screen = nullptr;
}

void Level::update(double dt) {
	removeNeeded();

	if (!_ended && _boss->getLife() <= 0) {
		_ended = true;
		_boss->die();
		_world.delObject(_boss);

		createLoots();
	}

	pullPlayerObjects();

	for (auto& p : _boss->getProtectilesToShoot()) {
		_projectiles.push_back(p);
		_world.addObject(p);
	}
	_boss->clearProtectilesToShoot();

	_player->update(dt);
	_boss->update(dt);
	for (auto& p : _projectiles) {
		p->update(dt);
	}
	_world.updateInc(dt, 10);


	if (!_ended)
		updateRunning(dt);
	else
		updateEnding(dt);
}
void Level::updateRunning(double dt) {
	if (_aiming) {
		Vector2f dist = _player->getPos() - Vector2f(_aimSprite.getPosition());
		_aimSprite.move(
			dist * dt * ( 50 / (1 + dist.length()) + 1) * _player->_speed / 30
		);
	}
}
void Level::updateEnding(double) {
	for (u32 i = _loots.size(); i > 0u; --i) {
		auto loot = _loots[i - 1u];

		if (loot->toRemove()) {
			_loots.erase(_loots.begin() + i - 1);
		}
	}

	if (_entranceToNext.collideWith(&_player->_hitBox)) {
		game->nextRoom();
	}
}

void Level::render(sf::RenderTarget& target) {

	_world.render(target);
	if (_ended) {
		_entranceToNext.render(target);

		for (auto& l : _loots) {
			l->render(target);
		}
	}
	else {
		_boss->render(target);

		if (_aiming) {
			_aimSprite.setColor(sf::Color(255, 255, 255, _aimSpriteOpacity));
			target.draw(_aimSprite);
		}
	}

	for (auto& p : _projectiles) {
		p->render(target);
	}
	for (auto& p : _playerZones) {
		p->render(target);
	}

	_player->render(target);

	for (u32 i = 0; i < _zones.size(); ++i) {
		_zones[i].collider->render(target);
	}
}

void Level::startAim() {
	_aiming = true;
	_aimSprite.setPosition(unitaryRng(RD) * WIDTH, unitaryRng(RD) * HEIGHT);
	_aimSpriteOpacity = 0;
	_aimSprite.setColor(sf::Color(255, 255, 255, _aimSpriteOpacity));

	TimerManager::addLinearEase<u08>(
		0.3f, 
		"opacityAim", 
		&_aimSpriteOpacity, 
		(u08)0, 
		(u08)255
	);
}
void Level::stopAim() {
	_aiming = false;
}

void Level::createLoots() {
	for (u32 i = 0u; i < 3u; ++i) {
		u32 nWeapon = rngRange<u32>(0u, Weapon::_weapons.size() - 1u);
		auto loot_ptr = std::make_shared<Loot>(10.f);
		
		_loots.push_back(loot_ptr);
		
		loot_ptr->pos = rngRange<Vector2f>({ 700, 500 }, { 1100, 600 });
		loot_ptr->setLootType(Loot::LOOT_TYPE::WEAPON);
		loot_ptr->setWeapon(
			std::make_shared<Weapon>(Weapon::_weapons[nWeapon])
		);
		loot_ptr->setLootable(true);
		_world.addObject(loot_ptr);
	}
}

float Level::getNormalizedBossLife() const {
	return _boss->getLife() / (float)_boss->getMaxLife();
}

void Level::removeNeeded() {
	for (size_t i = _projectiles.size(); i > 0; --i) {
		if (!_projectiles[i - 1]->toRemove()) continue;

		_world.delObject(_projectiles[i - 1]);
		_projectiles.erase(_projectiles.begin() + i - 1);
	}
	
	for (size_t i = _playerZones.size(); i > 0; --i) {
		if (!_playerZones[i - 1]->toRemove()) continue;

		_world.delObject(_playerZones[i - 1]);
		_playerZones.erase(_playerZones.begin() + i - 1);
	}
}

Vector2f Level::getPlayerPos() const {
	return _player->getPos();
}

void Level::pullPlayerObjects() {
	for (auto& p : _player->getZonesToApply()) {
		_world.addObject(p);
		_playerZones.push_back(p);
	}
	_player->clearZonesToApply();

	for (auto& p : _player->getProtectilesToShoot()) {
		_world.addObject(p);
		_projectiles.push_back(p);
	}
	_player->clearProtectilesToShoot();
}
