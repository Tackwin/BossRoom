#include <Physics/World.hpp>

#include <Managers/AssetsManager.hpp>

void World::update(float dt) {
	removeNeeded();
	pullAllProjectiles();

	updatePlayers(dt);
	updateBosses(dt);
	updateProjectiles(dt);
	updateParticles(dt);
}

void World::removeNeeded() {
	for (size_t i = _zones.size(); i > 0u; --i) {
		if (!_zones[i - 1] || _zones[i - 1]->toRemove) {
			_zones.erase(_zones.begin() + i - 1);
		}
	}

	for (size_t i = _players.size(); i > 0u; --i) {
		if (_players[i - 1].expired()) {
			_players.erase(_players.begin() + i - 1);
		}
	}

	for (size_t i = _bosses.size(); i > 0u; --i) {
		if (_bosses[i - 1].expired()) {
			_bosses.erase(_bosses.begin() + i - 1);
		}
	}

	for (size_t i = _projectiles.size(); i > 0u; --i) {
		if (!_projectiles[i - 1] || _projectiles[i - 1]->toRemove()) {
			_projectiles.erase(_projectiles.begin() + i - 1);
		}
	}

	for (size_t i = _particles.size(); i > 0u; --i) {
		if (!_particles[i - 1] || _particles[i - 1]->toRemove()) {
			_particles.erase(_particles.begin() + i - 1);
		}
	}
}

void World::pullAllProjectiles() {
	for (auto& p : _players) {
		auto &projectiles = p.lock()->getProtectilesToShoot();
		for (uint32_t i = 0u; i < projectiles.size(); ++i) {
			_projectiles.push_back(projectiles[i]);
		}

		if (!projectiles.empty())
			p.lock()->clearProtectilesToShoot();
	}
	for (auto& b : _bosses) {
		auto &projectiles = b.lock()->getProtectilesToShoot();
		for (uint32_t i = 0u; i < projectiles.size(); ++i) {
			_projectiles.push_back(projectiles[i]);
		}

		if (!projectiles.empty())
			b.lock()->clearProtectilesToShoot();
	}
}

void World::updatePlayers(float dt) {
	for (auto& p : _players) {
		const auto& player = p.lock();

		std::vector<bool> colliding(_zones.size());
		for (uint32_t i = 0U; i < _zones.size(); ++i) {
			colliding[i] = _zones[i]->collider->collideWith(&player->_disk);
		}

		player->update(dt);
		auto pos = player->getNewPos(dt);
		pos.y += 128 * dt;

		for (auto& f : _floors) {
			if (f.y < pos.y + player->_disk.r) {
				pos.y = f.y - player->_disk.r;
				break;
			}
		}

		player->setPos(pos);

		for (uint32_t i = 0U; i < _zones.size(); ++i) {
			const auto& zone = _zones[i];

			if (zone->collider->collideWith(&player->_disk)) {
				if (!colliding[i])
					zone->entered(player->_disk);

				zone->inside(player->_disk);
			}
			else if (colliding[i]) {
				zone->exited(player->_disk);
			}
		}
	}
}

void World::updateBosses(float dt) {
	for (auto& b : _bosses) {
		const auto& boss = b.lock();

		std::vector<bool> colliding(_zones.size());
		for (uint32_t i = 0U; i < _zones.size(); ++i) {
			const auto& zone = _zones[i];

			colliding[i] = zone->collider->collideWith(&boss->_disk);
		}

		boss->update(dt);

		for (uint32_t i = 0U; i < _zones.size(); ++i) {
			const auto& zone = _zones[i];

			if (zone->collider->collideWith(&boss->_disk)) {
				if (!colliding[i])
					zone->entered(boss->_disk);
				zone->inside(boss->_disk);
			}
			else if (colliding[i]) {
				zone->exited(boss->_disk);
			}
		}
	}
}

void World::updateProjectiles(float dt) {
	for (auto it = _projectiles.begin(); it != _projectiles.end(); ++it) {
		const auto& p0 = *it;

		p0->update(dt);

		if (p0->_remove || !p0->_damageable)
			continue;

		if (!p0->_player) {
			for (auto& p : _players) {
				const auto& player = p.lock();

				if (!player->isInvicible() && p0->_disk.collideWith(&player->_disk)) {
					p0->remove();
					player->hit(p0->getDamage());
					//_screen->shakeScreen(1); Need to handle that
				}
			}
		}
		else if (p0->_player) {
			for (auto& b : _bosses) {
				const auto& boss = b.lock();

				if (p0->_disk.collideWith(&boss->_disk)) {
					p0->remove();
					boss->hit(p0->getDamage());
					burstParticle(boss, p0->getPos());
				}
			}

		}
	}
}

void World::updateParticles(float dt) {
	for (auto& p : _particles) {
		p->update(dt);
	}
}

void World::addProjectile(const std::shared_ptr<Projectile>& projectile) {
	_projectiles.push_back(projectile);
}

void World::setPlayer(const std::shared_ptr<Player>& player, uint32_t idx) {
	if (idx >= _players.size())
		_players.resize(idx + 1);

	_players[idx] = player;
}
void World::delPlayer(uint32_t idx) {
	if (idx < _players.size()) {
		_players.erase(_players.begin() + idx);
	}
}

void World::setBoss(const std::shared_ptr<Boss>& boss, uint32_t idx) {
	if (idx >= _bosses.size())
		_bosses.resize(idx + 1);

	_bosses[idx] = boss;
}
void World::delBoss(uint32_t idx) {
	if (idx < _bosses.size()) {
		_bosses.erase(_bosses.begin() + idx);
	}
}

void World::render(sf::RenderTarget& target) {
	for (auto& p : _projectiles) {
		p->render(target);
	}

	for (auto& p : _particles) {
		p->render(target);
	}

	for (auto& z : _zones) {
		z->collider->render(target);
	}

	for (auto& f : _floors) {
		sf::RectangleShape shape({ f.x, WIDTH - f.y });
		shape.setFillColor(sf::Color(200, 200, 200, 200));
		shape.setPosition(0, f.y);
		target.draw(shape);
	}
}


void World::burstParticle(const std::shared_ptr<const Boss>& boss, const Vector2& pos) {
	const auto& jsonParticles = AssetsManager::getJson(JSON_KEY)["particlesGenerator"]["particleBurstFeather"];
	auto jsonBurst = jsonParticles;
	jsonBurst["dir"]["range"] = PIf / 8.f;
	jsonBurst["dir"]["mean"] = boss->getPos().angleTo(pos);
	jsonBurst["dir"]["type"] = "normal";

	const auto& newParticles = Particle::burst(jsonBurst, pos);
	_particles.insert(_particles.end(), newParticles.begin(), newParticles.end());
}

void World::addZone(const std::shared_ptr<Zone>& zone) {
	_zones.push_back(zone);
}

void World::addFloor(Vector2 vec) {
	_floors.push_back(vec);
}

void WorldExp::update(float dt) {
	for (auto& obj : _objects) {

		Vector2 pos = obj->pos;

		Vector2 nVel = obj->velocity + obj->force * dt;
		Vector2 nPos = obj->pos + obj->velocity * dt;

		obj->pos.x = nPos.x;

		for (auto& obj2 : _objects) {
			if (obj == obj2) continue;

			if (obj->collider->collideWith(obj2->collider)) {
				nVel.x = 0.f;
				obj->pos.x = pos.x;
				break;
			}
		}

		obj->pos.y = nPos.y;
		for (auto& obj2 : _objects) {
			if (obj == obj2) continue;

			if (obj->collider->collideWith(obj2->collider)) {
				nVel.y = 0.f;
				obj->pos.y = pos.y;
				break;
			}
		}

		obj->velocity = nVel;
		obj->force = Vector2::ZERO;
	}
}