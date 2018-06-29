#include "Wearable.hpp"
#include "WearableInfo.hpp"

#include <unordered_map>
#include <unordered_map>

#include "./../../Game.hpp"
#include "./../../Common.hpp"

#include "./../../Managers/AssetsManager.hpp"
#include "./../../Managers/InputsManager.hpp"
#include "./../../Managers/MemoryManager.hpp"
#include "./../../Managers/TimerManager.hpp"

#include "./../../Managers/EventManager.hpp"

#include "./../../Physics/Collider.hpp"

#include "./../Projectile.hpp"
#include "./../Player/Player.hpp"
#include "./../Level.hpp"
#include "./../Boss.hpp"

std::unordered_map<std::string, WearableInfo> Wearable::_wearables;

WearableInfo Wearable::GetWearableinfo(std::string wearable) noexcept {
	return _wearables.at(wearable);
}
void Wearable::InitWearable() {
// Fire ball
{
	WearableInfo fireBall;

	fireBall.name = Wearable::FIRE_BALL;
	fireBall.configuration = 
		AssetsManager::getJson(JSON_KEY)["weapons"][fireBall.name];
	//fireBall.uiSprite.setTexture(
	//	AssetsManager::getTexture(fireBall.configuration["sprite"])
	//);
	fireBall.onMount = [](Wearable& me) {
		auto json = me.getInfo().configuration;

		me._properties["countdown"] = 0.0;
		me._properties["evenShot"] = false;
		me._properties["projectile"] = json.at("projectile");
		me._properties["recoil"] = json.at("recoil").get<double>();
		me._properties["countdownTime"] = json.at("countdownTime").get<double>();
	};
	fireBall.onUnmount = [](Wearable& me) {
		for (const auto& k : me._keys) {
			if (TM::functionsExist(k)) TM::removeFunction(k);
		}
		me._keys.clear();
		me._properties.clear();
	};
	fireBall.onUpdate = [](double dt, Wearable& me) {
		if (me._player.expired()) return;
		auto player = me._player.lock();

		double& countdown = me.getPropsRef<double>("countdown");

		countdown -= dt;

		if (countdown > 0.0 || !player->eventFired(Event::FIRE)) return;

		bool& evenShot = me.getPropsRef<bool>("evenShot");
		double& recoil = me.getPropsRef<double>("recoil");
		double& countdownTime = me.getPropsRef<double>("countdownTime");
		nlohmann::json projectile = me.getPropsRef<nlohmann::json>("projectile");


		evenShot = !evenShot;
		countdown = countdownTime;

		Vector2f dir = player->getFacingDir();
		player->knockBack(dir * -recoil);

		float a = (float)dir.angleX();
		a += static_cast<float>((evenShot ? -1 : 1) * PIf / 6);

		Vector2f pos = player->support(a, 5);

		evenShot = !evenShot;

		player->addProjectile(std::make_shared<Projectile>(projectile, pos, dir, true));
	};
	_wearables.emplace(fireBall.name, fireBall);
}	
// Sword
{
	WearableInfo sword;

	sword.name = Wearable::SWORD;
	sword.configuration =
		AssetsManager::getJson(JSON_KEY)["weapons"][sword.name];
	//sword.uiSprite.setTexture(AssetsManager::getTexture(sword.configuration["sprite"]));
	sword.onMount = [](Wearable& me) {
		auto json = me.getInfo().configuration;

		me.setProp("countdown", 0.0);
		me.setProp("remainCountdown", 0.0);
		me.setProp("damage", json["damage"].get<float>());
		me.setProp("radius", json["radius"].get<float>());
		me.setProp("offset", json["offset"].get<float>());
		me.setProp("remainTime", json["remainTime"].get<double>());
		me.setProp("spriteZone", json["spriteZone"].get<std::string>());
		me.setProp("countdownTime", json["countdownTime"].get<double>());
	};
	sword.onUnmount = [](Wearable& me) {
		for (auto& k : me._keys) {
			if (TimerManager::functionsExist(k))
				TimerManager::removeFunction(k);
		}
		me._keys.clear();
		me._properties.clear();
	};
	sword.onUpdate = [](double dt, Wearable& me) {

		if (me._player.expired()) return;
		auto player = me._player.lock();

		double& remainCountdown = me.getPropsRef<double>("remainCountdown");
		if (me.hasProp("zone")) {
			remainCountdown -= dt;

			if (remainCountdown < 0.0) {
				std::weak_ptr<Zone>& zone = me.getPropsRef<std::weak_ptr<Zone>>("zone");
				if (!zone.expired()) {
					zone.lock()->setRemove();
				}
				me.delProp("zone");
			}
		}

		double& countdown = me.getPropsRef<double>("countdown");

		countdown -= dt;
		if (countdown > 0.0 || !player->eventFired(Event::FIRE)) return;
		
		float& damage = me.getPropsRef<float>("damage");
		float& radius = me.getPropsRef<float>("radius");
		float& offset = me.getPropsRef<float>("offset");
		double& remainTime = me.getPropsRef<double>("remainTime");
		double& countdownTime = me.getPropsRef<double>("countdownTime");
		std::string& spriteZone = me.getPropsRef<std::string>("spriteZone");

		countdown = countdownTime;
		remainCountdown = remainTime;

		std::shared_ptr<Zone> zone = std::make_shared<Zone>(radius);

		zone->pos = player->support((float)player->getFacingDir().angleX(), offset);

		zone->addSprite(
			spriteZone,
			sf::Sprite(AssetsManager::getTexture(spriteZone))
		);

		zone->collisionMask.set((u08)Object::BIT_TAGS::BOSS);
		zone->entered = [damage](Object* boss_) { // boss is Boss*
			auto boss = static_cast<Boss*>(boss_);
			boss->hit(damage);
		};

		player->addZone(zone);

		std::weak_ptr zonePtr = zone;
		me.setProp("zone", zonePtr);

	};
	_wearables.emplace(sword.name, sword);
}
}
