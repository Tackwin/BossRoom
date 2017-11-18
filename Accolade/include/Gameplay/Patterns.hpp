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

	static std::string directionalFire(Boss& boss, nlohmann::json json);
	static std::string directionalFire(Boss& boss, float fireRate, float time, nlohmann::json projectile);

	static std::string barage(Boss& boss, nlohmann::json json);
	static std::string barage(Boss& boss, u32 nOrbs, u32 nWaves, float iTime, nlohmann::json projectile);

	static std::string snipe(Boss& boss, nlohmann::json json);
	static std::string snipe(Boss& boss, u32 nShots, float aimTime, float iTime, nlohmann::json projectile);

	static std::string broyeur(Boss& boss, const nlohmann::json& json) { return""; }
	static std::string broyeur(Boss& boss, u32 nOrbs, nlohmann::json projectile) { return""; }

	static std::string cerclique(Boss& boss, const nlohmann::json& json) { return""; }
	static std::string cerclique(Boss& boss, float radius, float vA, float height, float dtHeight,
		u32 nOrbs, float iTime, nlohmann::json projectile) {
		return"";
	}

	static std::string rapprochement(Boss& boss, const nlohmann::json& json) { return""; }
	static std::string rapprochement(Boss& boss, u32 nOrbs, float eTime, float inSpawnRadius, float outSpawnRadius,
		nlohmann::json projectile) {
		return"";
	}

	static std::string boomerang(Boss& boss, const nlohmann::json& json) { return""; }
	static std::string boomerang(Boss& boss, u32 nBooms, float overShoot, float waitTime, nlohmann::json projectile) { return""; }

	static std::string entonnoir(Boss& boss, const nlohmann::json& json) { return""; }
	static std::string entonnoir(Boss& boss, u32 nOrbs, float x, nlohmann::json projectile) { return""; }

	static std::string blastCosmopolitain(Boss& boss, const nlohmann::json& json) { return""; }
	static std::string blastCosmopolitain(Boss& boss, u32 nBlasts, u32 nOrbs, float r, float iTime, float waitTime, const nlohmann::json& projectile) { return""; }
};
#pragma warning(pop)
