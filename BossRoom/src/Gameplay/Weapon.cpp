#include "Gameplay/Weapon.hpp"

#include "Global/Const.hpp"
#include "Managers/AssetsManager.hpp"
#include "Managers/InputsManager.hpp"
#include "Managers/TimerManager.hpp"
#include "Gameplay/Projectile.hpp"
#include <Physics/Collider.hpp>
#include "Gameplay/Player.hpp"
#include "Gameplay/Level.hpp"
#include "Gameplay/Game.hpp"

std::vector<std::shared_ptr<Weapon>> Weapon::_weapons;

void Weapon::createWeapons(std::shared_ptr<Player> player) {
	{
		_weapons.push_back(std::make_shared<Weapon>(player, AssetsManager::getJson(JSON_KEY)["weapons"][0]));

		_weapons[0]->_equip = [](Weapon& me)mutable->void{
			me._loot = false;

			me._keys.push_back(
			TimerManager::addFunction(me._json["cooldowns"][0], "1-cd", [&me](float)mutable->bool {
				me._flags |= 1;
				TimerManager::pauseFunction(me._keys[0]);
				return false;
			}));
		};
		_weapons[0]->_unEquip = [](Weapon& me)mutable->void{
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

				Vector2 dir = (InputsManager::getMouseWorldPos() - me._player->getPos()).normalize();
				float a = dir.angleX();
				static bool evenShot = false;
				a += static_cast<float>((evenShot ? -1 : 1) * PIf / 6);
				Vector2 pos = me._player->getPos() + Vector2(cos(a), sin(a)) * (me._player->_radius + 5);

				evenShot = !evenShot;
			
				//if (me._activeSounds[0].getStatus() != sf::Sound::Playing) {
					me._activeSounds[0].play();
				//}
				me._player->addProjectile(std::make_shared<Projectile>(me._json["projectile"], pos, dir, true));
			}
		};
		std::string sound1Str = _weapons[0]->_json["sounds"]["active"][0];
		AssetsManager::loadSound(sound1Str, ASSETS_PATH + sound1Str);
		_weapons[0]->_activeSounds.push_back(sf::Sound(AssetsManager::getSound(sound1Str)));
	}
	{
		_weapons.push_back(std::make_shared<Weapon>(player, AssetsManager::getJson(JSON_KEY)["weapons"][1]));

		_weapons[1]->_equip = [](Weapon& me)mutable->void {
			me._loot = false;

			me._keys.push_back(TimerManager::addFunction(me._json["cooldowns"][0], "1-cd", [&me](float)mutable->bool {
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

				Vector2 dir = (InputsManager::getMouseWorldPos() - me._player->getPos()).normalize();
				float a = dir.angleX();
				static bool evenShot = false;
				a += static_cast<float>((evenShot ? -1 : 1) * PIf / 6);
				Vector2 pos = me._player->getPos() + Vector2(cos(a), sin(a)) * (me._player->_radius + 5);

				evenShot = !evenShot;
				me._activeSounds[0].play();
				me._player->addProjectile(std::make_shared<Projectile>(me._json["projectile"], pos, dir, true));
			}
		};
		std::string sound1Str = _weapons[1]->_json["sounds"]["active"][0].get<std::string>();
		AssetsManager::loadSound(sound1Str, ASSETS_PATH + sound1Str);
		_weapons[1]->_activeSounds.push_back(sf::Sound(AssetsManager::getSound(sound1Str)));
	}
	{
		_weapons.push_back(std::make_shared<Weapon>(player, AssetsManager::getJson(JSON_KEY)["weapons"][2]));
		auto& weapon = _weapons[2];
		weapon->_equip = [](Weapon& me)mutable->void {
			me._loot = false;

			me._keys.push_back(TimerManager::addFunction(me._json["cooldowns"][0], "1-cd", [&me](float)mutable->bool {
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

				float A = me._player->getDirToFire().angleX();
				float spray = me._json["spray"];
				for (uint32_t i = 0; i < 3; ++i) {
					float dtA = spray * (i / 2.f - 0.5f);
					float a = A + dtA;
					Vector2 dir = Vector2::createUnitVector(a);
					Vector2 pos = me._player->getPos();

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

			me._keys.push_back(TimerManager::addFunction(me._json["cooldowns"][0], "1-cd", [&me](float)mutable->bool {
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

		weapon->_active = [](Weapon&, uint32)mutable->void {
		};
		weapon->_activeSounds.push_back(sf::Sound(AssetsManager::getSound(weapon->_json["sound"])));
	}
}
void Weapon::destroyWeapons() {
	_weapons.clear();
}


Weapon::Weapon(std::shared_ptr<Player> player, nlohmann::json json)
	: _player(player),
	_json(json),
	_radius(json["radius"]){

	_disk.r = _radius;

	_lootedSprite = sf::Sprite(AssetsManager::getTexture(_json["sprite"]));
	_uiSprite = sf::Sprite(AssetsManager::getTexture(_json["sprite"]));
	_uiSprite.setScale(2, 2);
	_uiSprite.setOrigin(
		(float)_uiSprite.getTextureRect().width, 
		(float)_uiSprite.getTextureRect().height
	);
	_uiSprite.setPosition((float)WIDTH, (float)HEIGHT);
}

Weapon::Weapon(const Weapon& other) :
	_player(other._player),
	_json(other._json),
	_lootable(other._lootable),
	_loot(other._loot),
	_flags(other._flags),
	_keys(other._keys),

	_radius(other._json["radius"]),
	_equip(other._equip),
	_unEquip(other._unEquip),
	_active(other._active),
	_passive(other._passive),
	_update(other._update),
	
	_activeSounds(other._activeSounds),
	_lootedPos(other._lootedPos) {

	_disk.r = _radius;

	const std::string& str = _json["sprite"];

	_lootedSprite = sf::Sprite(AssetsManager::getTexture(str));
	_uiSprite = sf::Sprite(AssetsManager::getTexture(str));
	_uiSprite.setScale(2, 2);
	_uiSprite.setOrigin(
		(float)_uiSprite.getTextureRect().width / 2.f,
		(float)_uiSprite.getTextureRect().height / 2.f
	);
	_uiSprite.setPosition((float)WIDTH, (float)HEIGHT);
}


Weapon::~Weapon() {

}

void Weapon::render(sf::RenderTarget& target) {
	if (_loot)
		target.draw(_lootedSprite);
}
void Weapon::renderGui(sf::RenderTarget& target) {
	target.draw(_uiSprite);
}
void Weapon::loot(Vector2 pos) {
	_loot = true;
	_lootedPos = pos;
	_lootable = true;
	_lootedSprite.setPosition(pos);
}

void Weapon::equip() {
	_equip(*this);
}
void Weapon::unEquip() {
	_unEquip(*this);
}

void Weapon::active(uint32 id) {
	_active(*this, id);
}
void Weapon::passive(uint32 id) {
	_passive(*this, id);
}

void Weapon::update(float dt) {
	_update(*this, dt);
}
