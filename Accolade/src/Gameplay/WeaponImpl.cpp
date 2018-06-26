#include "Weapon.hpp"

#include <map>

#include "./../Game.hpp"
#include "./../Common.hpp"

#include "./../Managers/AssetsManager.hpp"
#include "./../Managers/InputsManager.hpp"
#include "./../Managers/MemoryManager.hpp"
#include "./../Managers/TimerManager.hpp"

#include "./../Physics/Collider.hpp"

#include "Projectile.hpp"
#include "Player/Player.hpp"
#include "Level.hpp"
#include "Boss.hpp"

std::map<UUID, Weapon> Weapon::_weapons;

void Weapon::createWeapons() {
	{
		auto weapon = Weapon(AssetsManager::getJson(JSON_KEY)["weapons"][0]);

		weapon._equip = [](Weapon& me, Player&)mutable->void {
			me._keys.push_back(
				TimerManager::addFunction(me._json["cooldowns"][0], [&me](double)mutable->bool {
					me._flags.set(0);
					TimerManager::pauseFunction(me._keys[0]);
					return false;
				}
			));
		};
		weapon._unEquip = [](Weapon& me, Player&)mutable->void {
			for (auto& k : me._keys) {
				if (TimerManager::functionsExist(k))
					TimerManager::removeFunction(k);
			}
			me._keys.clear();
		};

		weapon._active = [](Weapon& me, Player& player, u32)mutable->void {
			static bool evenShot = false;
			
			if (me._flags[0]) {
				me._flags.reset(0);
				player.shoot();
				//TimerManager::resumeFunction(me._keys[0]);

				Vector2f dir = player.getFacingDir();

				if (auto json = me.getJson(); json.find("recoil_force") != json.end()) {
					player.knockBack(dir * -json["recoil_force"].get<float>());
				}

				float a = (float)dir.angleX();
				a += static_cast<float>((evenShot ? -1 : 1) * PIf / 6);
				Vector2f pos = 
					player.getPos() + 
					Vector2f(cos(a), sin(a)) * 
					(player.getPlayerInfo().radius + 5);

				evenShot = !evenShot;

				me._activeSounds[0].play();

				me.addProjectile(std::make_shared<Projectile>(me._json["projectile"], pos, dir, true));
			}
		};

		auto sound = sf::Sound(AssetsManager::getSound(weapon.getStringSoundActive(0)));
		sound.setVolume(SOUND_LEVEL);
		
		weapon._activeSounds.push_back(sound);
		weapon.setName("La sulfateuse");
		weapon.setCost(1u);

		_weapons[weapon.getUUID()] = weapon;
	}
	{
		auto weapon = Weapon(AssetsManager::getJson(JSON_KEY)["weapons"][1]);

		weapon._equip = [](Weapon& me, Player&)mutable->void {
			me._keys.push_back(TimerManager::addFunction(me._json["cooldowns"][0], [&me](double)mutable->bool {
				me._flags.set(0);
				TimerManager::pauseFunction(me._keys[0]);
				return false;
			}));
		};
		weapon._unEquip = [](Weapon& me, Player&)mutable->void {
			for (auto& k : me._keys) {
				if (TimerManager::functionsExist(k))
					TimerManager::removeFunction(k);
			}
			me._keys.clear();
		};

		weapon._active = [](Weapon& me, Player& player, u32)mutable->void {
			static bool evenShot = false;
			
			if (me._flags[0]) {
				me._flags.reset(0);
				TimerManager::resumeFunction(me._keys[0]);

				Vector2f dir = player.getFacingDir();

				if (auto json = me.getJson(); json.find("recoil_force") != json.end()) {
					player.knockBack(dir * -json["recoil_force"].get<float>());
				}
			
				float a = (float)dir.angleX();
				a += static_cast<float>((evenShot ? -1 : 1) * PIf / 6);
				Vector2f pos = 
					player.getPos() +
					Vector2f(cos(a), sin(a)) * 
					(player.getPlayerInfo().radius + 5);

				evenShot = !evenShot;
				
				me._activeSounds[0].play();
				player.addProjectile(
					std::make_shared<Projectile>(
						me._json["projectile"], 
						pos, 
						dir, 
						true
					)
				);
			}
		};
		
		auto sound = sf::Sound(AssetsManager::getSound(weapon.getStringSoundActive(0)));
		sound.setVolume(SOUND_LEVEL);

		weapon._activeSounds.push_back(sound);
		weapon.setName("La sulfateuse excité");
		weapon.setCost(5u);

		_weapons[weapon.getUUID()] = weapon;
	}
	{
		auto weapon = Weapon(AssetsManager::getJson(JSON_KEY)["weapons"][2]);

		weapon._equip = [](Weapon& me, Player&)mutable->void {
			me._keys.push_back(TimerManager::addFunction(me._json["cooldowns"][0], [&me](double)mutable->bool {
				me._flags.set(0);
				TimerManager::pauseFunction(me._keys[0]);
				return false;
			}));
		};
		weapon._unEquip = [](Weapon& me, Player&)mutable->void {
			for (auto& k : me._keys) {
				if (TimerManager::functionsExist(k))
					TimerManager::removeFunction(k);
			}
			me._keys.clear();
		};

		weapon._active = [](Weapon& me, Player& player, u32)mutable->void {
			if (me._flags[0]) {
				me._flags.reset(0);
				TimerManager::resumeFunction(me._keys[0]);

				float A = (float)player.getFacingDir().angleX();

				Vector2f dir = player.getFacingDir();

				if (auto json = me.getJson(); json.find("recoil_force") != json.end()) {
					player.knockBack(dir * -json["recoil_force"].get<float>());
				}

				float spray = me._json["spray"];
				for (u32 i = 0; i < 3; ++i) {
					float dtA = spray * (i / 2.f - 0.5f);
					float a = A + dtA;
					dir = Vector2f::createUnitVector(a);
					Vector2f pos = player.getPos();

					auto projectile = me._json["projectile"];
					projectile["speed"] = projectile["speed"].get<float>() * ((i == 1) ? 1 : tanf(dtA) / sinf(dtA));
					player.addProjectile(std::make_shared<Projectile>(projectile, pos, dir, true));
				}
			}
		};
		auto sound = sf::Sound(AssetsManager::getSound(weapon.getStringSoundActive(0)));
		sound.setVolume(SOUND_LEVEL);

		weapon._activeSounds.push_back(sound);
		weapon.setName("l'éclabousseur");
		weapon.setCost(10u);

		_weapons[weapon.getUUID()] = weapon;
	}
	{
		auto weapon = Weapon(AssetsManager::getJson(JSON_KEY)["weapons"][3]);

		weapon._equip = [](Weapon& me, Player&)mutable->void {
			me._flags.set();
		};
		weapon._unEquip = [](Weapon& me, Player&)mutable->void {
			for (auto& k : me._keys) {
				if (TimerManager::functionsExist(k))
					TimerManager::removeFunction(k);
			}
			me._keys.clear();
		};

		weapon._active = [](Weapon& me, Player& player, u32 id)mutable->void {
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
				std::shared_ptr<Zone> zone = std::make_shared<Zone>(r);

				zone->pos = 
					player.getPos() + 
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

				player.addZone(zone);

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

		auto sound = sf::Sound(AssetsManager::getSound(weapon.getStringSoundActive(0)));
		sound.setVolume(SOUND_LEVEL);

		weapon._activeSounds.push_back(sound);
		_weapons[weapon.getUUID()] = weapon;
	}
}
