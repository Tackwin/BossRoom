#include "Gameplay/Patterns.hpp"

#include "Const.hpp"

#include "Math/Vector.hpp"

#include "Managers/TimerManager.hpp"

#include "Gameplay/Level.hpp"
#include "Gameplay/Projectile.hpp"

auto Patterns::getValue(std::string pattern, nlohmann::json json, std::string str) {
	return json[str].is_null() ? _json[pattern][str] : json[str];
}

nlohmann::json Patterns::_json;

void Patterns::randomFire(Boss& boss, nlohmann::json json) {
	randomFire(
		boss,
		getValue("randomFire", json, "nOrbs"),
		getValue("randomFire", json, "a"),
		getValue("randomFire", json, "dtA"),
		getValue("randomFire", json, "eTime"),
		getValue("randomFire", json, "projectile")
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

		TimerManager::addFunction(sumDelay, "random fire_", 
			[&boss, projectile, pos, dir](double) -> bool {
				boss.shoot(projectile, pos, dir);
				return true;
			}
		);
		sumDelay += rngDelay(RD);
	}
}
std::string Patterns::directionalFire(Boss& boss, nlohmann::json json) {
	return directionalFire(
		boss,
		getValue("directionalFire", json, "fireRate"),
		getValue("directionalFire", json, "time"),
		getValue("directionalFire", json, "projectile")
	);
}
std::string Patterns::directionalFire(
	Boss& boss, 
	float fireRate, 
	float time, 
	nlohmann::json projectile
) {
	const auto& lambda = 
		[&boss, projectile, N = time * fireRate](double)mutable->bool {
			float a = (float)boss.getPos().angleTo(boss.getLevel()->getPlayerPos());

			Vector2f dir = Vector2f::createUnitVector(a);

			Vector2f pos = 
				boss.getPos() + 
				Vector2f::createUnitVector(a + PIf / 9) * (boss.getRadius() + 10);
			boss.shoot(projectile, pos, dir);

			pos = 
				boss.getPos() + 
				Vector2f::createUnitVector(a - PIf / 9) * (boss.getRadius() + 10);
			boss.shoot(projectile, pos, dir);

			return --N <= 0;
	};

	return TimerManager::addFunction(1 / fireRate, "directive fire", lambda);
}

std::string Patterns::barage(Boss& boss, nlohmann::json json) {
	return barage(
		boss,
		getValue("barage", json, "nOrbs"),
		getValue("barage", json, "nWaves"),
		getValue("barage", json, "iTime"),
		getValue("barage", json, "projectile")
	);
}
std::string Patterns::barage(
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

			const auto& pr = std::make_shared<Projectile>(projectile, pos, dir, false);
			boss.addProjectile(pr);
		}

		return --nWaves == 0;
	};
	
	return TimerManager::addFunction(iTime, "barage", barage);
}

std::string Patterns::snipe(Boss& boss, nlohmann::json json) {
	return snipe(
		boss,
		getValue("snipe", json, "nShots"),
		getValue("snipe", json, "aimTime"),
		getValue("snipe", json, "iTime"),
		getValue("snipe", json, "projectile")
	);
}
std::string Patterns::snipe(Boss&, u32, float, float, nlohmann::json) {
	/*auto lambda = std::shared_ptr<std::function<void()>>(new std::function<void()>);

	*lambda = [&boss, aimTime, projectile, nShots, iTime, lambda]()mutable->void {
		boss._level->startAim();
		TimerManager::addFunction(aimTime, "delay", [&boss, projectile, nShots, iTime, lambda](auto)mutable->bool {
			boss._level->stopAim();

			Vector2f pos = boss.getPos();
			Vector2f dir = Vector2f::createUnitVector((float)pos.angleTo(game->_player->getPos()));
			pos = pos + dir * (boss._radius + projectile["radius"].get<float>() + 1);

			boss.shoot(projectile, pos, dir);

			if (--nShots != 0) {
				TimerManager::addFunction(iTime, "snipe", [lambda](auto)mutable->bool {
					(*lambda)();
					return true;
				});
			}
			return true;
		});
	};
	(*lambda)();
	*/
	return "nil";
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