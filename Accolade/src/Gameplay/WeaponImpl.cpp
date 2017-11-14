#include "Gameplay/Weapon.hpp"

#include "Game.hpp"
#include "Common.hpp"

#include "Managers/AssetsManager.hpp"
#include "Managers/InputsManager.hpp"
#include "Managers/MemoryManager.hpp"
#include "Managers/TimerManager.hpp"

#include "Physics/Collider.hpp"

#include "Gameplay/Projectile.hpp"
#include "Gameplay/Player.hpp"
#include "Gameplay/Level.hpp"
#include "Gameplay/Boss.hpp"

std::array<Weapon, 4> Weapon::_weapons;

void Weapon::createWeapons(std::shared_ptr<Player> player) {
	{
		_weapons[0] = Weapon(player, AssetsManager::getJson(JSON_KEY)["weapons"][0]);

		_weapons[0]._equip = [](Weapon& me)mutable->void {
			me._keys.push_back(
				TimerManager::addFunction(me._json["cooldowns"][0], "1-cd", [&me](double)mutable->bool {
					me._flags.set(0);
					TimerManager::pauseFunction(me._keys[0]);
					return false;
				}
			));
		};
		_weapons[0]._unEquip = [](Weapon& me)mutable->void {
			for (auto& k : me._keys) {
				if (TimerManager::functionsExist(k))
					TimerManager::removeFunction(k);
			}
			me._keys.clear();
		};

		_weapons[0]._active = [](Weapon& me, u32)mutable->void {
			static bool evenShot = false;
			
			if (me._flags[0]) {
				me._flags.reset(0);
				me._player->shoot();
				//TimerManager::resumeFunction(me._keys[0]);

				Vector2f dir = 
					(InputsManager::getMouseWorldPos() - me._player->getPos())
					.normalize();
			
				float a = (float)dir.angleX();
				a += static_cast<float>((evenShot ? -1 : 1) * PIf / 6);
				Vector2f pos = 
					me._player->getPos() + 
					Vector2f(cos(a), sin(a)) * 
					(me._player->getPlayerInfo().radius + 5);

				evenShot = !evenShot;

				me._activeSounds[0].play();

				me.addProjectile(MM::make_shared<Projectile>(me._json["projectile"], pos, dir, true));
			}
		};

		auto sound = sf::Sound(AssetsManager::getSound(_weapons[0].getStringSoundActive(0)));
		sound.setVolume(SOUND_LEVEL);
		
		_weapons[0]._activeSounds.push_back(sound);
		_weapons[0].setName("La sulfateuse");
		_weapons[0].setCost(1u);
	}
	{
		_weapons[1] = Weapon(player, AssetsManager::getJson(JSON_KEY)["weapons"][1]);

		_weapons[1]._equip = [](Weapon& me)mutable->void {
			me._keys.push_back(TimerManager::addFunction(me._json["cooldowns"][0], "1-cd", [&me](double)mutable->bool {
				me._flags.set(0);
				TimerManager::pauseFunction(me._keys[0]);
				return false;
			}));
		};
		_weapons[1]._unEquip = [](Weapon& me)mutable->void {
			for (auto& k : me._keys) {
				if (TimerManager::functionsExist(k))
					TimerManager::removeFunction(k);
			}
			me._keys.clear();
		};

		_weapons[1]._active = [](Weapon& me, u32)mutable->void {
			static bool evenShot = false;
			
			if (me._flags[0]) {
				me._flags.reset(0);
				TimerManager::resumeFunction(me._keys[0]);

				Vector2f dir = 
					(InputsManager::getMouseWorldPos() - me._player->getPos())
					.normalize();
			
				float a = (float)dir.angleX();
				a += static_cast<float>((evenShot ? -1 : 1) * PIf / 6);
				Vector2f pos = 
					me._player->getPos() +
					Vector2f(cos(a), sin(a)) * 
					(me._player->getPlayerInfo().radius + 5);

				evenShot = !evenShot;
				
				me._activeSounds[0].play();
				me._player->addProjectile(
					MM::make_shared<Projectile>(
						me._json["projectile"], 
						pos, 
						dir, 
						true
					)
				);
			}
		};
		
		auto sound = sf::Sound(AssetsManager::getSound(_weapons[1].getStringSoundActive(0)));
		sound.setVolume(SOUND_LEVEL);

		_weapons[1]._activeSounds.push_back(sound);
		_weapons[1].setName("La sulfateuse excit�");
		_weapons[1].setCost(5u);
	}
	{
		_weapons[2] = Weapon(player, AssetsManager::getJson(JSON_KEY)["weapons"][2]);
		auto& weapon = _weapons[2];
		weapon._equip = [](Weapon& me)mutable->void {
			me._keys.push_back(TimerManager::addFunction(me._json["cooldowns"][0], "1-cd", [&me](double)mutable->bool {
				me._flags.set(0);
				TimerManager::pauseFunction(me._keys[0]);
				return false;
			}));
		};
		weapon._unEquip = [](Weapon& me)mutable->void {
			for (auto& k : me._keys) {
				if (TimerManager::functionsExist(k))
					TimerManager::removeFunction(k);
			}
			me._keys.clear();
		};

		weapon._active = [](Weapon& me, u32)mutable->void {
			if (me._flags[0]) {
				me._flags.reset(0);
				TimerManager::resumeFunction(me._keys[0]);

				float A = (float)me.getPlayer()->getDirToFire().angleX();
				float spray = me._json["spray"];
				for (u32 i = 0; i < 3; ++i) {
					float dtA = spray * (i / 2.f - 0.5f);
					float a = A + dtA;
					Vector2f dir = Vector2f::createUnitVector(a);
					Vector2f pos = me._player->getPos();

					auto projectile = me._json["projectile"];
					projectile["speed"] = projectile["speed"].get<float>() * ((i == 1) ? 1 : tanf(dtA) / sinf(dtA));
					me._player->addProjectile(MM::make_shared<Projectile>(projectile, pos, dir, true));
				}
			}
		};
		auto sound = sf::Sound(AssetsManager::getSound(_weapons[2].getStringSoundActive(0)));
		sound.setVolume(SOUND_LEVEL);

		_weapons[2]._activeSounds.push_back(sound);
		_weapons[2].setName("l'�clabousseur");
		_weapons[2].setCost(10u);
	}
	{
		_weapons[3] = Weapon(player, AssetsManager::getJson(JSON_KEY)["weapons"][3]);
		auto& weapon = _weapons[3];
		weapon._equip = [](Weapon& me)mutable->void {
			me._flags.set();
		};
		weapon._unEquip = [](Weapon& me)mutable->void {
			for (auto& k : me._keys) {
				if (TimerManager::functionsExist(k))
					TimerManager::removeFunction(k);
			}
			me._keys.clear();
		};

		weapon._active = [](Weapon& me, u32 id)mutable->void {
			switch (id) {
			case 0:
				if (me._flags[0]) {
					me._flags.reset(0);
					TimerManager::addFunction(me.getJson()["cooldowns"][0], 
						[&me](double) mutable -> bool {
							me._flags |= 1;
							return true;
						}
					);
				}
				else {
					break;
				}

				float r = getJsonValue<float>(me.getJson(), "radius");
				std::shared_ptr<Player> player = me.getPlayer();
				std::shared_ptr<Zone> zone = MM::make_shared<Zone>(r);

				zone->pos = 
					player->getPos() + 
					Vector2f(
						getJsonValue<float>(me.getJson(), "offsetX"),
						0
					);
				zone->addSprite(
					"slash",
					sf::Sprite(AssetsManager::getTexture("slash"))
				);
				zone->collisionMask.set((u08)Object::BIT_TAGS::BOSS);
				zone->entered = [&me](Object* boss_) { // boss is Boss*
					auto boss = static_cast<Boss*>(boss_);
					boss->hit(getJsonValue<float>(me.getJson(), "damage"));
				};

				player->addZone(zone);

				TimerManager::addFunction(
					getJsonValue<float>(me.getJson(), "remainTime"), 
					[zone](double) -> bool {
						zone->setRemove(true);
						return true;
					}
				);

				break;
			}
		};

		auto sound = sf::Sound(AssetsManager::getSound(_weapons[3].getStringSoundActive(0)));
		sound.setVolume(SOUND_LEVEL);

		_weapons[3]._activeSounds.push_back(sound);
	}
}
