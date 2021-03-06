// THIS WHOLE CLASS IS DEPRECATED IT SHOULD BE UNREACHABLE CODE

#include "Level.hpp"

#include "./../Game.hpp"
#include "./../Common.hpp"

#include "./../Managers/InputsManager.hpp"
#include "./../Managers/AssetsManager.hpp"
#include "./../Managers/MemoryManager.hpp"
#include "./../Managers/TimerManager.hpp"
#include "./../Managers/EventManager.hpp"

#include "./../Screens/LevelScreen.hpp"

#include "./../Graphics/Particle.hpp"

#include "Player/Player.hpp"
#include "Projectile.hpp"
#include "Wearable/Wearable.hpp"
#include "Probe.hpp"
#include "Boss.hpp"

std::vector<std::shared_ptr<Level>> Level::levels(N_LEVEL);

void Level::createLevels() {
	for (u32 i = 0; i < N_LEVEL; ++i) {
		levels[i] = std::make_shared<Level>(
			std::make_shared<Boss>(*Boss::bosses[i]), 
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
	_aimSprite(AssetsManager::getTexture("aim_texture")) {
	_aimSprite.setScale(0.7f, 0.7f);
	_aimSprite.setOrigin(
		_aimSprite.getTextureRect().width / 2.f, 
		_aimSprite.getTextureRect().height / 2.f
	);

	_entranceToNext.r = 100.f;
	_entranceToNext.dtPos = { (float)WIDTH, HEIGHT / 2.f };
	assert(true);
}
Level::~Level() {
}

void Level::start(LevelScreen* screen) {
	this->_screen = screen;
	_player = std::make_shared<Player>(C::game->getPlayerInfo());
	_player->setPos({ 100.f, HEIGHT / 2.f });

	// _boss->enterLevel(this);
	_player->enterLevel(this);

	_player->collider->onEnter =
		std::bind(&Level::playerOnEnter, this, std::placeholders::_1);

	_world.addObject(_player);
	_world.addObject(_boss);

	_floor = std::make_shared<Object>();
	_floor->idMask.set((size_t)Object::BIT_TAGS::FLOOR);
	_floor->collisionMask.set((size_t)Object::BIT_TAGS::PLAYER);
	_floor->pos = { 0.f, 600.f };
	_floor->collider = std::make_unique<Box>();
	_floorHitBox = (Box*)_floor->collider.get();
	_floorHitBox->size = { 3000.f, 120.f };
	_world.addObject(_floor);

	_view = sf::View({ WIDTH / 2.f, HEIGHT / 2.f }, { (float)WIDTH, (float)HEIGHT });

	subscribeToEvents();
}
void Level::stop() {
	unsubscribeToEvents();

	//_boss->exitLevel();
	_player->exitLevel();

	_projectiles.clear();

	_world.purge();

	_boss.reset();
	_player.reset();

	_screen = nullptr;
}

void Level::update(double dt) {
	removeNeeded();

	if (!_ended && _boss->getLife() <= 0) {
		_ended = true;
		_boss->die();
		_world.delObject(_boss->uuid);

		createLoots();
	}

	pullPlayerObjects();

	for (auto& p : _boss->getProtectilesToShoot()) {
		addProjectile(p);
	}
	_boss->clearProtectilesToShoot();

	auto mouse = InputsManager::getMousePosInView(_view);
	_player->setFacingDir((float) (mouse - _player->getPos()).angleX());
	_player->update(dt);
	_boss->update(dt);
	for (auto& p : _projectiles) {
		p->update(dt);
		
		if (p->toExplode()) {
			if (auto z = Zone::buildExplosion(p); z)
				addZone(z);

			p->remove();
		}
	}
	_world.updateInc(dt, 3);

	if (!_ended)
		updateRunning(dt);
	else
		updateEnding(dt);
}
void Level::updateRunning(double dt) {
	if (_aiming) {
		Vector2f dist = _player->getPos() - Vector2f(_aimSprite.getPosition());
		auto rateOfAproach = dist * (50 / (1 + dist.length()) + 1);

		_aimSprite.move(
			rateOfAproach * dt * _player->getPlayerInfo().speed / 30
		);
	}
}
void Level::updateEnding(double) {
	for (size_t i = _loots.size(); i > 0u; --i) {
		auto loot = _loots[i - 1u];

		if (loot->toRemove()) {
			_loots.erase(_loots.begin() + i - 1);
		}
	}

	if (_entranceToNext.collideWith(_player->collider.get())) {
		game->nextRoom();
	}
}

void Level::render(sf::RenderTarget& target) {
	auto oldView = target.getView();
	target.setView(_view);

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
	for (auto& p : _zones) {
		p->render(target);
	}

	_player->render(target);
	target.setView(oldView);
}

void Level::startAim() {
	_aiming = true;
	_aimSprite.setPosition(unitaryRng(RD) * WIDTH, unitaryRng(RD) * HEIGHT);
	_aimSpriteOpacity = 0;
	_aimSprite.setColor(sf::Color(255, 255, 255, _aimSpriteOpacity));

	TimerManager::addLinearEase<u08>(
		0.3f,
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
		u32 nWeapon = rngRange<u32>(0u, sizeof(Wearable::EVERY_WEARABLE));
		auto loot_ptr = std::make_shared<Loot>(30.f);
		
		_loots.push_back(loot_ptr);
		
		loot_ptr->pos = rngRange<Vector2f>({ 700, 500 }, { 1100, 600 });
		loot_ptr->setLootType(Loot::LOOT_TYPE::WEAPON);
		loot_ptr->setWeapon(Wearable::EVERY_WEARABLE[nWeapon]);
		loot_ptr->setLootable(true);
		_world.addObject(loot_ptr);
	}
}

float Level::getNormalizedBossLife() const {
	return _boss->getLife() / (float)_boss->getMaxLife();
}

void Level::removeNeeded() {
	for (size_t i = _projectiles.size(); i > 0; --i) {
		if (_projectiles[i - 1]->toRemove()) {
			delProjectile((u32)i - 1);
		}
	}
	for (size_t i = _zones.size(); i > 0; --i) {
		if (_zones[i - 1]->toRemove()) {
			delZone((u32)i - 1);
		}
	}
}

Vector2f Level::getPlayerPos() const {
	return _player->getPos();
}
Vector2f Level::getDirToPlayer(Vector2f pos) const noexcept {
	return Vector2f::createUnitVector((pos - _player->getPos()).angleX());
}


void Level::pullPlayerObjects() {
	for (auto& z : _player->getZonesToApply()) {
		addZone(z);
	}
	_player->clearZonesToApply();

	for (auto& p : _player->getProtectilesToShoot()) {
		addProjectile(p);
	}
	_player->clearProtectilesToShoot();
}

void Level::addProjectile(const std::shared_ptr<Projectile>& p) {
	p->collider->onEnter = std::bind(
		&Level::projectileOnEnter, this, std::weak_ptr(p), std::placeholders::_1
	);
	_world.addObject(p);
	_projectiles.push_back(p);
}
void Level::delProjectile(const std::shared_ptr<Projectile>& p) {
	_world.delObject(p->uuid);
	_projectiles.erase(std::find(_projectiles.begin(), _projectiles.end(), p));
}
void Level::delProjectile(u32 i) {
	_world.delObject(_projectiles[i]->uuid);
	_projectiles.erase(_projectiles.begin() + i);
}
void Level::addZone(const std::shared_ptr<Zone>& z) {
	_world.addObject(z);
	_zones.push_back(z);
}
void Level::delZone(const std::shared_ptr<Zone>& z) {
	_world.delObject(z->uuid);
	_zones.erase(std::find(_zones.begin(), _zones.end(), z));
}
void Level::delZone(u32 i) {
	_world.delObject(_zones[i]->uuid);
	_zones.erase(_zones.begin() + i);
}

bool Level::lost() const {
	return !_player->isAlive();
}

void Level::playerOnEnter(Object* object){
	if (object->idMask.test(Object::BIT_TAGS::FLOOR)) {
		_player->floored();
	}
}
void Level::playerOnExit(Object*){}
	 
void Level::projectileOnEnter(std::weak_ptr<Projectile> ptrProjectile, Object* object){
	if (ptrProjectile.expired()) return;
	auto projectile = ptrProjectile.lock();

	if (object->idMask.test(Object::BIT_TAGS::FLOOR) && !projectile->isFromPlayer()) {
		projectile->explode();
	} else if (
		object->idMask.test(Object::BIT_TAGS::PLAYER) && !projectile->isFromPlayer()
	) {
		projectile->remove();
		_player->hit(projectile->getDamage());
	}
}
void Level::projectileOnExit(std::weak_ptr<Projectile>, Object*){}

void Level::subscribeToEvents() noexcept {
	_keyPressedEvent = EventManager::subscribe("keyPressed",
		[&](EventManager::EventCallbackParameter args) -> void {
		auto key = std::any_cast<sf::Keyboard::Key>(*args.begin());

		_player->keyPressed(key);
	}
	);
	_keyReleasedEvent = EventManager::subscribe("keyReleased",
		[&](EventManager::EventCallbackParameter args) -> void {
		auto key = std::any_cast<sf::Keyboard::Key>(*args.begin());
		_player->keyReleased(key);
	}
	);
}

void Level::unsubscribeToEvents() noexcept {
	EventManager::unSubscribe("keyPressed", _keyPressedEvent);
	EventManager::unSubscribe("keyReleased", _keyReleasedEvent);
}