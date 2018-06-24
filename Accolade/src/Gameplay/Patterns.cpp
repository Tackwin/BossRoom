#include "Patterns.hpp"

#include "./../Common.hpp"

#include "./../Math/Vector.hpp"

#include "./../Managers/TimerManager.hpp"
#include "./../Managers/MemoryManager.hpp"

#include "./../Game.hpp"

#include "Level.hpp"
#include "Projectile.hpp"
#include "Player/Player.hpp"

template<typename T>
T Patterns::getValue(
	std::string pattern, nlohmann::json json, std::string str
) {
	if constexpr (std::is_same<T, nlohmann::json>::value) {
		return json[str].is_null()
			? _json[pattern][str]
			: json[str];
	}
	else {
		return json[str].is_null()
			? getJsonValue<T>(_json[pattern], str) 
			: getJsonValue<T>(json, str);
	}
}


nlohmann::json Patterns::_json;

void Patterns::randomFire(Boss& boss, nlohmann::json json) {
	randomFire(
		boss,
		getValue<u32>("randomFire", json, "nOrbs"),
		getValue<float>("randomFire", json, "a"),
		getValue<float>("randomFire", json, "dtA"),
		getValue<float>("randomFire", json, "eTime"),
		getValue<nlohmann::json>("randomFire", json, "projectile")
	);
}
void Patterns::randomFire(
	Boss& boss, 
	u32 nOrbs, 
	float a, 
	float dtA, 
	float time, 
	nlohmann::json projectile
) {
	std::uniform_real_distribution<float> rngA(a - dtA * 0.5f, a + dtA * 0.5f);
	std::uniform_real_distribution<float> rngDelay(0, 2 * time / nOrbs); //nOrbs * E(rngDt) = time

	float sumDelay = 0.f;
	
	for (size_t i = 0u; i < nOrbs; ++i) {
		float ca = rngA(RD);
		Vector2f dir = Vector2f::createUnitVector(ca);
		Vector2f pos = boss.getPos() + dir * (boss.getRadius() + 10);

		const auto& key = TimerManager::addFunction(sumDelay, "random fire_", 
			[&boss, projectile, pos, dir](double) mutable -> bool {
				boss.shoot(projectile, pos, dir);
				return true;
			}
		);
		boss.addKeyTimer(key);
		sumDelay += rngDelay(RD);
	}
}
void Patterns::directionalFire(Boss& boss, nlohmann::json json) {
	directionalFire(
		boss,
		getValue<float>("directionalFire", json, "fireRate"),
		getValue<float>("directionalFire", json, "time"),
		getValue<nlohmann::json>("directionalFire", json, "projectile")
	);
}
void Patterns::directionalFire(
	Boss& boss, 
	float fireRate, 
	float time, 
	nlohmann::json projectile
) {
	const auto& key = TimerManager::addFunction(1 / fireRate, "directive fire", 
		[&boss, projectile, N = time * fireRate](double) mutable -> bool {
			float a = 
				(float)boss.getPos().angleTo(boss.getLevel()->getPlayerPos());

			Vector2f dir = Vector2f::createUnitVector(a);

			Vector2f pos =
				boss.getPos() +
				Vector2f::createUnitVector(a + PIf / 9) * 
				(boss.getRadius() + 10);
			boss.shoot(projectile, pos, dir);

			pos =
				boss.getPos() +
				Vector2f::createUnitVector(a - PIf / 9) * 
				(boss.getRadius() + 10);
			boss.shoot(projectile, pos, dir);

			return --N <= 0;
		}
	);
	boss.addKeyTimer(key);
}

void Patterns::barage(Boss& boss, nlohmann::json json) {
	barage(
		boss,
		getValue<u32>("barage", json, "nOrbs"),
		getValue<u32>("barage", json, "nWaves"),
		getValue<float>("barage", json, "iTime"),
		getValue<nlohmann::json>("barage", json, "projectile")
	);
}
void Patterns::barage(
	Boss& boss, 
	u32 nOrbs, 
	u32 nWaves, 
	float iTime, 
	nlohmann::json projectile
) {
	const auto& barage = [&boss, nWaves, nOrbs, projectile](double)mutable->bool {
		
		for (u32 i = 0; i < nOrbs; ++i) {
			Vector2f pos((float)WIDTH, (float)HEIGHT * i / nOrbs);
			Vector2f dir = Vector2f::createUnitVector(PIf);

			const auto& pr = 
				std::make_shared<Projectile>(projectile, pos, dir, false);
			boss.shoot(pr);
		}

		return --nWaves == 0;
	};
	
	boss.addKeyTimer(TimerManager::addFunction(iTime, "barage", barage));
}

void Patterns::snipe(Boss& boss, nlohmann::json json) {
	return snipe(
		boss,
		getValue<u32>("snipe", json, "nShots"),
		getValue<float>("snipe", json, "aimTime"),
		getValue<float>("snipe", json, "iTime"),
		getValue<nlohmann::json>("snipe", json, "projectile")
	);
}
void Patterns::snipe(
	Boss& boss, 
	u32 nShots, 
	float aimTime, 
	float iTime, 
	nlohmann::json projectile
) {
	const auto& key = TM::addFunction(aimTime + iTime, 
		[&boss, aimTime, projectile, nShots](double) mutable -> bool {
			boss.getLevel()->startAim();

			const auto& key = TM::addFunction(aimTime, 
				[&boss, projectile](double) mutable -> bool {
					boss.getLevel()->stopAim();

					Vector2f pos = boss.getPos();
					auto dir = 
						Vector2f::createUnitVector(
							(float)boss.getPos().angleTo(boss.getLevel()->getPlayerPos())
						);
					pos = pos + dir *
						(
							boss.getRadius() + 
							projectile["radius"].get<float>() + 
							1
						);

					boss.shoot(projectile, pos, dir);
					return true;
				}
			);
			boss.addKeyTimer(key);

			return --nShots == 0;
		}
	);
	boss.addKeyTimer(key);
}
/*

std::string Patterns::broyeur(Boss& boss, const nlohmann::json& json) {
	return broyeur(
		boss,
		getValue("broyeur", json, "nOrbs"),
		getValue("broyeur", json, "projectile")
	);
}
std::string Patterns::broyeur(Boss& boss, u32 nOrbs, nlohmann::json projectile) {
	return TimerManager::addFunction(0, "broyeur", [boss, nOrbs, projectile](auto)mutable->bool {
		for (u32 i = 0; i < nOrbs; ++i) {
			Vector2 pos(
				(float)WIDTH * i / (nOrbs - 1) + WIDTH / (2 * nOrbs),
				(float)((i & 1) ? 0 : HEIGHT)
			);
			Vector2 dir(0, (float)((i & 1) ? 1 : -1));

			boss.addProjectile(std::make_shared<Projectile>(projectile, pos, dir, false));
		}
		return true;
	});
}

std::string Patterns::cerclique(Boss& boss, const nlohmann::json& json) {
	return cerclique(
		boss,
		getValue("cerclique", json, "radius"),
		getValue("cerclique", json, "vA"),
		getValue("cerclique", json, "height"),
		getValue("cerclique", json, "dtHeight"),
		getValue("cerclique", json, "nOrbs"),
		getValue("cerclique", json, "iTime"),
		getValue("cerclique", json, "projectile")
	);
}
std::string Patterns::cerclique(Boss& boss, float radius, float vA, float height, float dtHeight,
	u32 nOrbs, float iTime, nlohmann::json projectile) {
	return TimerManager::addFunction(iTime, "cerclique",
		[boss, radius, vA, height, dtHeight, projectile, nOrbs](auto)mutable->bool {
		float h = height * HEIGHT;
		float dtH = dtHeight * HEIGHT;

		Vector2 pos((float)WIDTH, (unitaryRng(RD) * 2 - 1) * dtH + h);
		Vector2 cPos = pos;
		Vector2 dir(-1, 0);

		float a = 0;
		int dirA = unitaryRng(RD) > 0.5f ? 1 : -1;

		boss.addProjectile(std::make_shared<Projectile>(projectile, pos, dir, false,
			[cPos, a, radius, vA, dirA](Projectile& me, auto dt) mutable {
			cPos += me._dir * me._speed * dt;
			a += (float)fmod(vA * dt * PIf * dirA, vA);

			//TODO: sort this out for the new physic system !!!!
			me.setDir(Vector2::createUnitVector(a) * radius);
		}));
		return --nOrbs == 0;
	});
}

std::string Patterns::rapprochement(Boss& boss, const nlohmann::json& json) {
	return rapprochement(
		boss,
		getValue("rapprochement", json, "nOrbs"),
		getValue("rapprochement", json, "eTime"),
		getValue("rapprochement", json, "inSpawnRadius"),
		getValue("rapprochement", json, "outSpawnRadius"),
		getValue("rapprochement", json, "projectile")
	);
}
std::string Patterns::rapprochement(Boss& boss, u32 nOrbs, float eTime, float inSpawnRadius, float outSpawnRadius,
	nlohmann::json projectile) {
	float sumS = 0;
	for (u32 i = 0; i < nOrbs; ++i) {
		TimerManager::addFunction(sumS, "rapprochement",
			[boss, nOrbs, eTime, inSpawnRadius, outSpawnRadius, projectile](auto)mutable->bool {
			Vector2 pos = game->_player->getPos() + Vector2::createUnitVector(unitaryRng(RD) * 2 * PIf) *
				(unitaryRng(RD) * (outSpawnRadius - inSpawnRadius) + inSpawnRadius);

			auto ptrProjectile = std::make_shared<Projectile>(projectile, pos, Vector2::ZERO, false);
			boss.addProjectile(ptrProjectile);

			TimerManager::addFunction(1, "",
				[ptrProjectile](auto)->bool {
				ptrProjectile->_dir = (game->_player->getPos() - ptrProjectile->getPos()).normalize();
				return true;
			});
			return true;
		});

		sumS += unitaryRng(RD) * 2 * eTime / nOrbs;
	}
	return "nil";
}

std::string Patterns::boomerang(Boss& boss, const nlohmann::json& json) {
	return boomerang(
		boss,
		getValue("boomerang", json, "nBooms"),
		getValue("boomerang", json, "overShoot"),
		getValue("boomerang", json, "waitTime"),
		getValue("boomerang", json, "projectile")
	);
}
std::string Patterns::boomerang(Boss& boss, u32 nBooms, float overShoot, float waitTime, nlohmann::json projectile) {
	Vector2 pos = boss.getPos();
	Vector2 dir = (game->_player->getPos() - boss.getPos()).normalize();

	auto ptrProjectile = std::make_shared<Projectile>(projectile, pos, dir, false);
	boss.addProjectile(ptrProjectile);

	std::shared_ptr<std::function<bool()>> lambda(new std::function<bool()>);
	*lambda = [lambda, ptrProjectile, nBooms, waitTime, overShoot, speed = projectile["speed"].get<float>()]()mutable->bool {
		ptrProjectile->_dir = Vector2::ZERO;
		TimerManager::addFunction(waitTime, "",
			[ptrProjectile](auto)mutable->bool {
			ptrProjectile->_dir = (game->_player->getPos() - ptrProjectile->getPos()).normalize();
			return true;
		});
		if (--nBooms != 0) {
			float shootTime = ((game->_player->getPos() - ptrProjectile->getPos()).length() + overShoot) / speed;
			TimerManager::addFunction(shootTime + waitTime, "changeDir",
				[lambda](auto)mutable->bool {
				(*lambda)();
				return true;
			});
		}
		return true;
	};

	float shootTime = ((game->_player->getPos() - pos).length() + overShoot) / projectile["speed"].get<float>();
	return TimerManager::addFunction(shootTime, "changeDir",
		[lambda](auto)mutable->bool {
		(*lambda)();
		return true;
	});
}

std::string Patterns::entonnoir(Boss& boss, const nlohmann::json& json) {
	return entonnoir(
		boss,
		getValue("entonnoir", json, "nOrbs"),
		getValue("entonnoir", json, "x"),
		getValue("entonnoir", json, "projectile")
	);
}
std::string Patterns::entonnoir(Boss& boss, u32 nOrbs, float x, nlohmann::json projectile) {
	for (u32 i = 0; i < nOrbs; ++i) {
		Vector2 pos(WIDTH * (1 - x), (float)HEIGHT * i / nOrbs);
		Vector2 dir = (game->_player->getPos() - Vector2(200, 0)) - pos;

		boss.addProjectile(std::make_shared<Projectile>(projectile, pos, dir, false));
	}
	return "nil";
}

std::string Patterns::blastCosmopolitain(Boss& boss, const nlohmann::json& json) { //Pourquoi j'avais appelé ça comme ça déjà ??
	return blastCosmopolitain(
		boss,
		getValue("blastCosmopolitain", json, "nBlasts"),
		getValue("blastCosmopolitain", json, "nOrbs"),
		getValue("blastCosmopolitain", json, "r"),
		getValue("blastCosmopolitain", json, "iTime"),
		getValue("blastCosmopolitain", json, "waitTime"),
		getValue("blastCosmopolitain", json, "projectile")
	);
}
std::string Patterns::blastCosmopolitain(Boss& boss, u32 nBlasts, u32 nOrbs, float r, float iTime, float waitTime, const nlohmann::json& projectile) {
	return TimerManager::addFunction(iTime, "blastCosomoplitain",
		[projectile, boss, nBlasts, nOrbs, r, waitTime](auto)mutable->bool {
		float x = unitaryRng(RD) * WIDTH;
		float y = unitaryRng(RD) * HEIGHT;
		float a = unitaryRng(RD) * 2 * PIf;

		for (u32 i = 0; i < nOrbs; ++i) {
			float dtA = 2 * PIf * i / nOrbs;
			Vector2 pos = Vector2::createUnitVector(a + dtA) * r + Vector2(x, y);

			auto ptrProjectile = std::make_shared<Projectile>(projectile, pos, Vector2::ZERO, false);
			ptrProjectile->_damageable = false;
			boss.addProjectile(ptrProjectile);
			TimerManager::addFunction(waitTime, "",
				[ptrProjectile, A = a + dtA](auto)mutable->bool {
				ptrProjectile->_dir = Vector2::createUnitVector(A);
				ptrProjectile->_damageable = true;
				return true;
			});
		}

		return --nBlasts == 0;
	});
}*/