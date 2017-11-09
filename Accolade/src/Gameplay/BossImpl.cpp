#include "Gameplay/Boss.hpp"

#include "Game.hpp"
#include "Common.hpp"

#include "Managers/AssetsManager.hpp"
#include "Managers/TimerManager.hpp"

#include "Gameplay/Projectile.hpp"
#include "Gameplay/Patterns.hpp"
#include "Gameplay/Player.hpp"

void Boss::createBosses() {
	bosses[0] = std::make_shared<Boss>(AssetsManager::getJson(JSON_KEY)["bosses"][0],
		[](double, Boss&) {}, //update function
		[](Boss&) { // Init function
			/*auto sound = sf::Sound(AssetsManager::getSound("shoot2"));
			sound.setVolume(SOUND_LEVEL);
			boss._sounds.push_back(sound);

			boss._keyPatterns.push_back(TimerManager::addFunction(3, "P1", [&boss](double)mutable->bool {
				Patterns::directionalFire(boss, Patterns::_json["directionalFire"]);
				boss._sprite.pushAnim("action");
				boss._sounds[0].play();
				return false;
			}));
			boss._keyPatterns.push_back(TimerManager::addFunction(4, "P2", [&boss](double)mutable->bool {
				if (boss._life < 0.66f * boss._maxLife) {
					Patterns::barage(boss, Patterns::_json["barage"]);
					boss._sprite.pushAnim("action");
				}
				return false;
			}));*/
		},
		[](Boss& boss) { // UnInit function
			for (auto& k : boss._keyPatterns) {
				TimerManager::removeFunction(k);
			}
			boss._keyPatterns.clear();
		}
	);

	bosses[1] = std::make_shared<Boss>(AssetsManager::getJson(JSON_KEY)["bosses"][1],
		[](double, Boss&) {},
		[](Boss& boss) { // Init function
		boss._keyPatterns.push_back(TimerManager::addFunction(6, "P1", [&boss](double)mutable->bool {
			Patterns::cerclique(boss, Patterns::_json["cerclique"]);
			return false;
		}));
		boss._keyPatterns.push_back(TimerManager::addFunction(3.5, "P2", [&boss](double)mutable->bool {
			if (boss._life < 0.66f * boss._maxLife) Patterns::snipe(boss, Patterns::_json["snipe"]);
			return false;
		}));
		boss._keyPatterns.push_back(TimerManager::addFunction(5, "P3", [&boss](double)mutable->bool {
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
		[](double, Boss&) {},
		[](Boss& boss) { // Init function
		boss._keyPatterns.push_back(TimerManager::addFunction(3, "P1", [&boss](double)mutable->bool {
			Patterns::boomerang(boss, Patterns::_json["boomerang"]);
			return false;
		}));
		boss._keyPatterns.push_back(TimerManager::addFunction(2, "P2", [&boss](double)mutable->bool {
			if (boss._life < 0.66f * boss._maxLife) Patterns::rapprochement(boss, Patterns::_json["rapprochement"]);
			return false;
		}));
		boss._keyPatterns.push_back(TimerManager::addFunction(5, "P3", [&boss](double)mutable->bool {
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
		[](double, Boss&) {},
		[](Boss& boss) { // Init function
		boss._keyPatterns.push_back(TimerManager::addFunction(4, "P1", [&boss](double)mutable->bool {
			Patterns::blastCosmopolitain(boss, Patterns::_json["blastCosmopolitain"]);
			return false;
		}));
		boss._keyPatterns.push_back(TimerManager::addFunction(7, "P2", [&boss](double)mutable->bool {
			if (boss._life < 0.66f * boss._maxLife) Patterns::broyeur(boss, Patterns::_json["broyeur"]);
			return false;
		}));
		boss._keyPatterns.push_back(TimerManager::addFunction(2, "P3", [&boss](double)mutable->bool {
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
		[](double, Boss&) {},
		[](Boss& boss) { // Init function
		boss._keyPatterns.push_back(TimerManager::addFunction(6, "P1", [&boss](double)mutable->bool {
			Patterns::boomerang(boss, Patterns::_json["randomFire"]);
			return false;
		}));
		boss._keyPatterns.push_back(TimerManager::addFunction(2, "P2", [&boss](double)mutable->bool {
			if (boss._life < 0.66f * boss._maxLife) Patterns::snipe(boss, Patterns::_json["snipe"]);
			return false;
		}));
		boss._keyPatterns.push_back(TimerManager::addFunction(4, "P3", [&boss](double)mutable->bool {
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