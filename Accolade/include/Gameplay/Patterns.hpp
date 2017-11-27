#pragma once
#include "3rd/json.hpp"

#include "Gameplay/Boss.hpp"

#pragma warning(push)
#pragma warning(disable:4100)
class Patterns {
private:

	template<typename T>
	static T getValue(std::string pattern, nlohmann::json json, std::string str);

public:

	static nlohmann::json _json;

	static void randomFire(Boss& boss, nlohmann::json json);
	static void randomFire(Boss& boss, u32 nOrbs, float a, float dtA, float time, nlohmann::json projectile);

	static void directionalFire(Boss& boss, nlohmann::json json);
	static void directionalFire(Boss& boss, float fireRate, float time, nlohmann::json projectile);

	static void barage(Boss& boss, nlohmann::json json);
	static void barage(Boss& boss, u32 nOrbs, u32 nWaves, float iTime, nlohmann::json projectile);

	static void snipe(Boss& boss, nlohmann::json json);
	static void snipe(Boss& boss, u32 nShots, float aimTime, float iTime, nlohmann::json projectile);

	static void broyeur(Boss& boss, const nlohmann::json& json) { return; }
	static void broyeur(Boss& boss, u32 nOrbs, nlohmann::json projectile) {
		return; 
	}

	static void cerclique(Boss& boss, const nlohmann::json& json) { return; }
	static void cerclique(
		Boss& boss, float radius, float vA, float height, float dtHeight,
		u32 nOrbs, float iTime, nlohmann::json projectile
	) {
		return;
	}

	static void rapprochement(Boss& boss, const nlohmann::json& json) { return; }
	static void rapprochement(Boss& boss, u32 nOrbs, float eTime, float inSpawnRadius, float outSpawnRadius,
		nlohmann::json projectile) {
		return;
	}

	static void boomerang(Boss& boss, const nlohmann::json& json) { return; }
	static void boomerang(Boss& boss, u32 nBooms, float overShoot, float waitTime, nlohmann::json projectile) { return; }

	static void entonnoir(Boss& boss, const nlohmann::json& json) { return; }
	static void entonnoir(Boss& boss, u32 nOrbs, float x, nlohmann::json projectile) { return; }

	static void blastCosmopolitain(Boss& boss, const nlohmann::json& json) { return; }
	static void blastCosmopolitain(Boss& boss, u32 nBlasts, u32 nOrbs, float r, float iTime, float waitTime, const nlohmann::json& projectile) { return; }
};
#pragma warning(pop)
