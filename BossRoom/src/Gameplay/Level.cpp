#include "Gameplay/Level.hpp"

#include "Managers/InputsManager.hpp"
#include "Managers/AssetsManager.hpp"
#include "Managers/TimerManager.hpp" 
#include "Gameplay/Projectile.hpp"
#include "Screens/LevelScreen.hpp"
#include "Graphics/Particle.hpp"
#include "Gameplay/Weapon.hpp"
#include "Gameplay/Player.hpp"
#include "Gameplay/Probe.hpp"
#include "Gameplay/Boss.hpp"
#include "Gameplay/Game.hpp"
#include "Global/Const.hpp"

std::vector<std::shared_ptr<Level>> Level::levels(N_LEVEL);

void Level::createLevels() {
	for (uint32 i = 0; i < N_LEVEL; ++i) {
		levels[i] = std::make_shared<Level>(std::make_shared<Boss>(*Boss::bosses[i]), i);
	}
}
void Level::destroyLevels() {
	levels.clear();
}

Level::Level(std::shared_ptr<Boss> boss, uint32 n) : 
	_boss(boss), 
	_n(n),
	_ended(false),
	_entranceToNext(100),
	_aimSprite(AssetsManager::getTexture("aim")) {
	_aimSprite.setScale(0.7f, 0.7f);
	_aimSprite.setOrigin(_aimSprite.getTextureRect().width / 2.f, _aimSprite.getTextureRect().height / 2.f);

	_entranceToNext.setFillColor(sf::Color::Red);
	_entranceToNext.setOrigin(100, 100);
	_entranceToNext.setPosition((float)WIDTH, HEIGHT / 2.f);
}

Level::~Level() {
}

void Level::start(LevelScreen* screen_) {
	_screen = screen_;
	_player = game->_player;

	_boss->enterLevel(this);
	_player->enterLevel(this);
}
void Level::stop() {
	_boss->exitLevel();
	_player->exitLevel();

	_projectiles.clear();
	_probes.clear();

	_player.reset();

	_screen = nullptr;
}

void Level::update(float dt) {
	if (!_ended && _boss->_life <= 0) {
		_ended = true;
		_boss->die();

		createLoots();
		_loot.push_back(std::make_shared<Weapon>(*Weapon::_weapons[1]));
		_loot.push_back(std::make_shared<Weapon>(*Weapon::_weapons[1]));

		_loot[0]->loot({ 1000.f, HEIGHT / 3.f});
		_loot[1]->loot({ 1000.f, 2 * HEIGHT / 3.f });
	}

	if (!_ended)
		updateRunning(dt);
	else
		updateEnding(dt);
	updateParticles(dt);
}
void Level::updateRunning(float dt) {
	_player->update(dt);
	_boss->update(dt);

	if (_aiming) {
		Vector2 dist = _player->_pos - Vector2(_aimSprite.getPosition());
		_aimSprite.move(
			dist * dt * ( 50 / (1 + dist.length()) + 1) * _player->_speed / 30
		);
	}

	for (auto &p : _probes) {
		p->update(dt);
	}
	for (int i = _probes.size() - 1; i >= 0; --i) {
		if (_probes[i]->_remove) {
			_probes[i] = _probes.back();
			_probes.pop_back();
		}
	}

	for (auto it = _projectiles.begin(); it != _projectiles.end(); ++it) {
		auto& p0 = *it;
		
		p0->update(dt);

		if (p0->_remove || !p0->_damageable)
			continue;

		if (!p0->_player && !_player->_invincible && Vector2::equal(p0->_pos, _player->_pos, p0->_radius + _player->_radius)) {
			p0->_remove = true;
			_player->hit(p0->_damage);
			_screen->shakeScreen(1);
		}
		else if (p0->_player) {
			if(Vector2::equal(p0->_pos, _boss->_pos, p0->_radius + _boss->_radius)) {
				p0->_remove = true;
				_boss->hit(p0->_damage);

				const auto& jsonParticles = AssetsManager::getJson(JSON_KEY)["particles"]["particleBurstFeather"];
				auto jsonBurst = jsonParticles;
				jsonBurst["dir"]["range"] = PIf / 8.f;
				jsonBurst["dir"]["mean"] = _boss->_pos.angleTo(p0->_pos);
				jsonBurst["dir"]["type"] = "normal";

				const auto& newParticles = Particle::burst(jsonBurst, p0->_pos);
				_particles.insert(_particles.end(), newParticles.begin(), newParticles.end());

			}
			else if (p0->_destroyOthers) {
				for (auto it2 = _projectilesBoss.begin(); it2 != _projectilesBoss.end(); ++it2) {
					auto& p1 = *it2;
					auto ptr = p1.lock();
					if (!ptr || ptr->_remove)
						continue;
					if (Vector2::equal(p0->_pos, ptr->_pos, p0->_radius + ptr->_radius)) {
						p0->_remove = true;
						ptr->_remove = true;
						break;
					}
				}
			}
		}
	}

	_projectiles.erase(
		std::remove_if(_projectiles.begin(), _projectiles.end(), [](const std::shared_ptr<Projectile>& A)->bool {return A->_remove; }),
		_projectiles.end()
	);
	_projectilesPlayer.erase(
		std::remove_if(_projectilesPlayer.begin(), _projectilesPlayer.end(), [](const std::weak_ptr<Projectile>& A)->bool {return A.expired() ? true : A.lock()->_remove; }),
		_projectilesPlayer.end()
	);
	_projectilesBoss.erase(
		std::remove_if(_projectilesBoss.begin(), _projectilesBoss.end(), [](const std::weak_ptr<Projectile>& A)->bool {return A.expired() ? true : A.lock()->_remove; }),
		_projectilesBoss.end()
	);
}
void Level::updateEnding(float dt) {
	_player->update(dt);

	for (uint32 i = 0; i < _loot.size(); ++i) {
		auto loot = _loot[i];
		if (loot->_lootable && Vector2::equal(_player->_pos, loot->_lootedSprite.getPosition(), _player->_radius + loot->_radius)) {
			_player->_weapon->loot(_player->_pos);
			_player->_weapon->_lootable = false;
			
			TimerManager::addFunction(0.f, "loot again only if the player get out of the loot range", [player = _player, weapon = _player->_weapon](float)mutable->bool {
				if (Vector2::equal(player->_pos, weapon->_lootedPos, player->_radius + weapon->_radius)) {
					return false; //He is not out yet, damn it player what are you doing !!
				}
				weapon->_lootable = true;
				return true;
			});

			_loot[i] = _player->_weapon;

			_player->swapWeapon(loot);
			break;
		}
	}
	
	if (Vector2::equal(_player->_pos, _entranceToNext.getPosition(), _player->_radius + _entranceToNext.getRadius())) {
		game->nextRoom();
	}

	for (auto& p : _projectiles) {
		p->update(dt);
	}


	_projectiles.erase(
		std::remove_if(_projectiles.begin(), _projectiles.end(), [](const std::shared_ptr<Projectile>& A)->bool {return A->_remove; }),
		_projectiles.end()
	);
	_projectilesPlayer.erase(
		std::remove_if(_projectilesPlayer.begin(), _projectilesPlayer.end(), [](const std::weak_ptr<Projectile>& A)->bool {return A.expired() ? true : A.lock()->_remove; }),
		_projectilesPlayer.end()
	);
	_projectilesBoss.erase(
		std::remove_if(_projectilesBoss.begin(), _projectilesBoss.end(), [](const std::weak_ptr<Projectile>& A)->bool {return A.expired() ? true : A.lock()->_remove; }),
		_projectilesBoss.end()
	);
}
void Level::updateParticles(float dt) {
	for (auto& p : _particles) {
		p->update(dt);
	}

	_particles.erase(
		std::remove_if(_particles.begin(), _particles.end(), [](const auto& A) {return A->toRemove(); }),
		_particles.end()
	);
}

void Level::render(sf::RenderTarget& target) {
	if (!_ended) {
		_boss->render(target);
		for (auto &p : _probes) {
			p->render(target);
		}
	}
	else {
		for (auto& l : _loot) {
			l->render(target);
		}
	}
	for (auto &p : _projectiles)
		p->render(target);
	for (auto& p : _particles)
		p->render(target);
	if (_ended)
		target.draw(_entranceToNext);

	_player->render(target);
	if (!_ended && _aiming) {
		_aimSprite.setColor(sf::Color(255, 255, 255, _aimSpriteOpacity));
		target.draw(_aimSprite);
	}
}

void Level::startAim() {
	_aiming = true;
	_aimSprite.setPosition(unitaryRng(RNG) * WIDTH, unitaryRng(RNG) * HEIGHT);
	_aimSpriteOpacity = 0;
	_aimSprite.setColor(sf::Color(255, 255, 255, _aimSpriteOpacity));

	TimerManager::addLinearEase<uint8>(0.3f, "opacityAim", &_aimSpriteOpacity, (uint8)0, (uint8)255);
}
void Level::stopAim() {
	_aiming = false;
}

void Level::CaCHit(unsigned int d) {
	Vector2 dir = (InputsManager::getMouseWorldPos() - _player->_pos).normalize();
	if (Vector2::equal(_player->_pos + dir * 100, _boss->_pos, _boss->_radius + _player->_radius)) {
		_boss->hit(d);
	}
}

void Level::addProjectile(const std::shared_ptr<Projectile>& projectile) {
	_projectiles.push_back(projectile);
	if (projectile->_player) {
		_projectilesPlayer.push_back(std::weak_ptr<Projectile>(projectile));
	}
	else{
		_projectilesBoss.push_back(std::weak_ptr<Projectile>(projectile));
	}
}

void Level::createLoots() {
	uint32_t nLoots = 2;

	for (uint32_t i = 0; i < nLoots; ++i) {
		_loot.push_back(std::make_shared<Weapon>(*Weapon::_weapons[(uint32_t)(unitaryRng(RNG) * Weapon::_weapons.size())]));
		_loot.back()->loot({ 950 + unitaryRng(RNG) * 250, 60 + unitaryRng(RNG) * 500 });
	}
}

void Level::bossDied() {
	for (auto& p : _projectilesBoss)
		if (const auto& ptr = p.lock(); ptr)
			ptr->_remove = true;

	_projectiles.erase(
		std::remove_if(_projectiles.begin(), _projectiles.end(), [](const auto& A) {return A->_remove; }),
		_projectiles.end()
	);
	_projectilesBoss.clear();
}


float Level::getNormalizedBossLife() const {
	assert(_boss);
	return _boss->_life / (float)_boss->_maxLife;
}