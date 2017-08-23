#include "Boss.hpp"

#include "AssetsManager.hpp"
#include "TimerManager.hpp"
#include "Patterns.hpp"
#include "Player.hpp"

using namespace nlohmann;

std::vector<std::shared_ptr<Boss>> Boss::bosses(C::N_LEVEL);

void Boss::createBosses() {
	bosses[0] = std::make_shared<Boss>(AssetsManager::getJson(JSON_KEY)["bosses"][0],
		[](float, Boss& boss) { /*Update function*/
			static bool goodToGo = false;
			if (!goodToGo) {
				goodToGo = boss._life < 0.33f * boss._maxLife;
				if (goodToGo) {
					TimerManager::removeFunction(boss._keyPatterns[0]);
					TimerManager::removeFunction(boss._keyPatterns[1]);
					boss._keyPatterns.erase(boss._keyPatterns.begin());
					boss._keyPatterns.erase(boss._keyPatterns.begin());

					boss._keyPatterns.push_back(TimerManager::addFunction(3, "P1", [boss](float)mutable->bool {
						Patterns::directionalFire(boss, Patterns::_json["directionalFire"]);
						return false;
					}));
					boss._keyPatterns.push_back(TimerManager::addFunction(4.2f, "P2", [boss](float)mutable->bool {
						Patterns::barage(boss, Patterns::_json["barage"]);
						return false;
					}));
				}
			}
		},
		[](Boss& boss) { // Init function
			boss._keyPatterns.push_back(TimerManager::addFunction(3, "P1", [&boss](float)mutable->bool {
				Patterns::directionalFire(boss, Patterns::_json["directionalFire"]);
				return false;
			}));
			boss._keyPatterns.push_back(TimerManager::addFunction(4, "P2", [&boss](float)mutable->bool {
				if (boss._life < 0.66f * boss._maxLife)
					Patterns::barage(boss, Patterns::_json["barage"]);
				return false;
			}));
		},
		[](Boss& boss) { // UnInit function
			for (auto& k : boss._keyPatterns) {
				TimerManager::removeFunction(k);
			}
			boss._keyPatterns.clear();
		}
	);

	bosses[1] = std::make_shared<Boss>(AssetsManager::getJson(JSON_KEY)["bosses"][1],
		[](float, Boss&) { /*Update function*/ 
			
		},
		[](Boss& boss) { // Init function
			boss._keyPatterns.push_back(TimerManager::addFunction(6, "P1", [&boss](float)mutable->bool {
				Patterns::cerclique(boss, Patterns::_json["cerclique"]);
				return false;
			}));
			boss._keyPatterns.push_back(TimerManager::addFunction(3.5, "P2", [&boss](float)mutable->bool {
				if (boss._life < 0.66f * boss._maxLife) Patterns::snipe(boss, Patterns::_json["snipe"]);
				return false;
			}));
			boss._keyPatterns.push_back(TimerManager::addFunction(5, "P3", [&boss](float)mutable->bool {
				if (boss._life < 0.33f * boss._maxLife) Patterns::barage(boss, Patterns::_json["barage"]);
				return false;
			}));
		},
		[](Boss& boss) { // UnInit function
			for (auto& k : boss._keyPatterns) {
				TimerManager::removeFunction(k);
			}
			boss._keyPatterns.clear();
		}
	);

	bosses[2] = std::make_shared<Boss>(AssetsManager::getJson(JSON_KEY)["bosses"][1],
		[](float, Boss&) { /*Update function*/ },
		[](Boss& boss) { // Init function
			boss._keyPatterns.push_back(TimerManager::addFunction(3, "P1", [&boss](float)mutable->bool {
				Patterns::boomerang(boss, Patterns::_json["boomerang"]);
				return false;
			}));
			boss._keyPatterns.push_back(TimerManager::addFunction(2, "P2", [&boss](float)mutable->bool {
				if (boss._life < 0.66f * boss._maxLife) Patterns::rapprochement(boss, Patterns::_json["rapprochement"]);
				return false;
			}));
			boss._keyPatterns.push_back(TimerManager::addFunction(5, "P3", [&boss](float)mutable->bool {
				if (boss._life < 0.33f * boss._maxLife) Patterns::entonnoir(boss, Patterns::_json["entonnoir"]);
				return false;
			}));
		},
		[](Boss& boss) { // UnInit function
		for (auto& k : boss._keyPatterns) {
			TimerManager::removeFunction(k);
		}
		boss._keyPatterns.clear();
	}
	);

	bosses[3] = std::make_shared<Boss>(AssetsManager::getJson(JSON_KEY)["bosses"][1],
		[](float, Boss&) { /*Update function*/ },
		[](Boss& boss) { // Init function
			boss._keyPatterns.push_back(TimerManager::addFunction(4, "P1", [&boss](float)mutable->bool {
				Patterns::blastCosmopolitain(boss, Patterns::_json["blastCosmopolitain"]);
				return false;
			}));
			boss._keyPatterns.push_back(TimerManager::addFunction(7, "P2", [&boss](float)mutable->bool {
				if (boss._life < 0.66f * boss._maxLife) Patterns::broyeur(boss, Patterns::_json["broyeur"]);
				return false;
			}));
			boss._keyPatterns.push_back(TimerManager::addFunction(2, "P3", [&boss](float)mutable->bool {
				if (boss._life < 0.33f * boss._maxLife) {
					auto json = Patterns::_json["directionalFire"];
					json["time"] = 0.3f;
					Patterns::directionalFire(boss, json);
				}
				return false;
			}));
		},
		[](Boss& boss) { // UnInit function
		for (auto& k : boss._keyPatterns) {
			TimerManager::removeFunction(k);
		}
		boss._keyPatterns.clear();
	}
	);

	bosses[4] = std::make_shared<Boss>(AssetsManager::getJson(JSON_KEY)["bosses"][1],
		[](float, Boss&) { /*Update function*/ },
		[](Boss& boss) { // Init function
			boss._keyPatterns.push_back(TimerManager::addFunction(6, "P1", [&boss](float)mutable->bool {
				Patterns::boomerang(boss, Patterns::_json["randomFire"]);
				return false;
			}));
			boss._keyPatterns.push_back(TimerManager::addFunction(2, "P2", [&boss](float)mutable->bool {
				if (boss._life < 0.66f * boss._maxLife) Patterns::snipe(boss, Patterns::_json["snipe"]);
				return false;
			}));
			boss._keyPatterns.push_back(TimerManager::addFunction(4, "P3", [&boss](float)mutable->bool {
				if (boss._life < 0.33f * boss._maxLife) Patterns::rapprochement(boss, Patterns::_json["rapprochement"]);
				return false;
			}));
		},
		[](Boss& boss) { // UnInit function
		for (auto& k : boss._keyPatterns) {
			TimerManager::removeFunction(k);
		}
		boss._keyPatterns.clear();
	}
	);
}
void Boss::destroyBosses() {
	bosses.clear();
}

Boss::Boss(const basic_json<>& json, std::function<void(float, Boss&)> updateFunction, std::function<void(Boss&)> init, 
		   std::function<void(Boss&)> unInit)
:	_json(json),
	_update(updateFunction),
	_init(init),
	_unInit(unInit) {
}

Boss::~Boss() {
}

void Boss::enterLevel(Level* level) {
	_level = level;

	_life = _json["life"];
	_maxLife = _life;
	_pos.x = _json["startpos"]["x"];
	_pos.y = _json["startpos"]["y"];
	_radius = _json["radius"];
	_color = sf::Color::hexToColor(_json["color"]);

	const auto& str = std::string(ASSETS_PATH) + _json["sprite"]["idle"].get<std::string>();
	AssetsManager::loadTexture(str, str);
	_sprite = sf::Sprite(AssetsManager::getTexture(str));
	_sprite.setScale(2 * _radius / 1000.f, 2 * _radius / 1000.f);
	_sprite.setOrigin(500, 500);
	
	_init(*this);
}
void Boss::exitLevel() {
	for (uint32 i = _keyPatterns.size(); i != 0; --i) {
		auto& k = _keyPatterns[i - 1];
		TimerManager::removeFunction(k);

		_keyPatterns.erase(_keyPatterns.begin() + i - 1);
	}
	_unInit(*this);
	_level = nullptr;
}

void Boss::die() {
	for (auto& k : _keyPatterns) {
		TimerManager::removeFunction(k);
	}
	_keyPatterns.clear();
}


void Boss::update(float dt) {
	_update(dt, *this);
}

void Boss::render(sf::RenderTarget &target) {
	_sprite.setPosition(_pos);
	target.draw(_sprite);
	for (auto &h : _rectBox) {
		h.draw(target, sf::Color::White);
	}
}

void Boss::hit(unsigned int d) {
	_life -= d;
	_life = _life < 0 ? 0 : _life;
	_sprite.setColor(sf::Color(230, 230, 230));
	TimerManager::addFunction(0.05f, "blinkDown", [&](float)->bool {
		_sprite.setColor(_color);
		return true;
	});
}

void Boss::addProjectile(const std::shared_ptr<Projectile>& projectile) {
	if (_level && _life != 0) _level->addProjectile(projectile); //TODO: debug why the callbacks doesn't delete when the boss die
}

Vector2 Boss::getDirToFire() {
	return (game->_player->_pos - _pos).normalize();
}
