#include "Gameplay/Weapon.hpp"

#include "Game.hpp"
#include "Const.hpp"

#include "Managers/AssetsManager.hpp"
#include "Managers/InputsManager.hpp"
#include "Managers/TimerManager.hpp"

#include "Physics/Collider.hpp"

#include "Gameplay/Projectile.hpp"
#include "Gameplay/Player.hpp"
#include "Gameplay/Level.hpp"

std::vector<std::shared_ptr<Weapon>> Weapon::_weapons;

void Weapon::createWeapons(std::shared_ptr<Player> player) {
	{
		_weapons.push_back(std::make_shared<Weapon>(player, AssetsManager::getJson(JSON_KEY)["weapons"][0]));

		_weapons[0]->_equip = [](Weapon& me)mutable->void {
			me._loot = false;

			me._keys.push_back(
				TimerManager::addFunction(me._json["cooldowns"][0], "1-cd", [&me](double)mutable->bool {
				me._flags |= 1;
				TimerManager::pauseFunction(me._keys[0]);
				return false;
			}));
		};
		_weapons[0]->_unEquip = [](Weapon& me)mutable->void {
			for (auto& k : me._keys) {
				if (TimerManager::functionsExist(k))
					TimerManager::removeFunction(k);
			}
			me._keys.clear();
		};

		_weapons[0]->_active = [](Weapon& me, uint32)mutable->void {
			if ((me._flags & 1) == 1) {
				me._flags ^= 1;
				me._player->shoot();
				//TimerManager::resumeFunction(me._keys[0]);

				Vector2f dir = (InputsManager::getMouseWorldPos() - me._player->getPos()).normalize();
				float a = (float)dir.angleX();
				static bool evenShot = false;
				a += static_cast<float>((evenShot ? -1 : 1) * PIf / 6);
				Vector2f pos = me._player->getPos() + Vector2f(cos(a), sin(a)) * (me._player->_radius + 5);

				evenShot = !evenShot;

				//if (me._activeSounds[0].getStatus() != sf::Sound::Playing) {
				me._activeSounds[0].play();
				//}
				me._player->addProjectile(std::make_shared<Projectile>(me._json["projectile"], pos, dir, true));
			}
		};
		std::string sound1Str = _weapons[0]->_json["sounds"]["active"][0];
		AssetsManager::loadSound(sound1Str, ASSETS_PATH + sound1Str);
		auto sound = sf::Sound(AssetsManager::getSound(sound1Str));
		sound.setVolume(SOUND_LEVEL);
		_weapons[0]->_activeSounds.push_back(sound);
	}
	{
		_weapons.push_back(std::make_shared<Weapon>(player, AssetsManager::getJson(JSON_KEY)["weapons"][1]));

		_weapons[1]->_equip = [](Weapon& me)mutable->void {
			me._loot = false;

			me._keys.push_back(TimerManager::addFunction(me._json["cooldowns"][0], "1-cd", [&me](double)mutable->bool {
				me._flags |= 1;
				TimerManager::pauseFunction(me._keys[0]);
				return false;
			}));
		};
		_weapons[1]->_unEquip = [](Weapon& me)mutable->void {
			for (auto& k : me._keys) {
				if (TimerManager::functionsExist(k))
					TimerManager::removeFunction(k);
			}
			me._keys.clear();
		};

		_weapons[1]->_active = [](Weapon& me, uint32)mutable->void {
			if ((me._flags & 1) == 1) {
				me._flags ^= 1;
				TimerManager::resumeFunction(me._keys[0]);

				Vector2f dir = (InputsManager::getMouseWorldPos() - me._player->getPos()).normalize();
				float a = (float)dir.angleX();
				static bool evenShot = false;
				a += static_cast<float>((evenShot ? -1 : 1) * PIf / 6);
				Vector2f pos = me._player->getPos() + Vector2f(cos(a), sin(a)) * (me._player->_radius + 5);

				evenShot = !evenShot;
				me._activeSounds[0].play();
				me._player->addProjectile(std::make_shared<Projectile>(me._json["projectile"], pos, dir, true));
			}
		};
		std::string sound1Str = _weapons[1]->_json["sounds"]["active"][0].get<std::string>();
		AssetsManager::loadSound(sound1Str, ASSETS_PATH + sound1Str);
		auto sound = sf::Sound(AssetsManager::getSound(sound1Str));
		sound.setVolume(SOUND_LEVEL);
		_weapons[1]->_activeSounds.push_back(sound);
	}
	{
		_weapons.push_back(std::make_shared<Weapon>(player, AssetsManager::getJson(JSON_KEY)["weapons"][2]));
		auto& weapon = _weapons[2];
		weapon->_equip = [](Weapon& me)mutable->void {
			me._loot = false;

			me._keys.push_back(TimerManager::addFunction(me._json["cooldowns"][0], "1-cd", [&me](double)mutable->bool {
				me._flags |= 1;
				TimerManager::pauseFunction(me._keys[0]);
				return false;
			}));
		};
		weapon->_unEquip = [](Weapon& me)mutable->void {
			for (auto& k : me._keys) {
				if (TimerManager::functionsExist(k))
					TimerManager::removeFunction(k);
			}
			me._keys.clear();
		};

		weapon->_active = [](Weapon& me, uint32)mutable->void {
			if ((me._flags & 1) == 1) {
				me._flags ^= 1;
				TimerManager::resumeFunction(me._keys[0]);

				float A = (float)me._player->getDirToFire().angleX();
				float spray = me._json["spray"];
				for (uint32_t i = 0; i < 3; ++i) {
					float dtA = spray * (i / 2.f - 0.5f);
					float a = A + dtA;
					Vector2f dir = Vector2f::createUnitVector(a);
					Vector2f pos = me._player->getPos();

					auto projectile = me._json["projectile"];
					projectile["speed"] = projectile["speed"].get<float>() * ((i == 1) ? 1 : tanf(dtA) / sinf(dtA));
					me._player->addProjectile(std::make_shared<Projectile>(projectile, pos, dir, true));
				}
			}
		};
		std::string sound1Str = weapon->_json["sounds"]["active"][0].get<std::string>();
		AssetsManager::loadSound(sound1Str, ASSETS_PATH + sound1Str);
		weapon->_activeSounds.push_back(sf::Sound(AssetsManager::getSound(sound1Str)));
	}
	{
	_weapons.push_back(std::make_shared<Weapon>(player, AssetsManager::getJson(JSON_KEY)["weapons"][3]));
	auto& weapon = _weapons[3];
	weapon->_equip = [](Weapon& me)mutable->void {
		me._loot = false;

		me._keys.push_back(TimerManager::addFunction(me._json["cooldowns"][0], "1-cd", [&me](double)mutable->bool {
			me._flags |= 1;
			TimerManager::pauseFunction(me._keys[0]);
			return false;
		}));
	};
	weapon->_unEquip = [](Weapon& me)mutable->void {
		for (auto& k : me._keys) {
			if (TimerManager::functionsExist(k))
				TimerManager::removeFunction(k);
		}
		me._keys.clear();
	};

	weapon->_active = [](Weapon&, uint32)mutable->void {};
	weapon->_activeSounds.push_back(sf::Sound(AssetsManager::getSound(weapon->_json["sound"])));
	}
}
void Weapon::destroyWeapons() {
	_weapons.clear();
}