#pragma once
#ifdef ECS

#include "json.hpp"

#include "Const.hpp"

struct Entity {
	static Ecs::entity_type createProjectile(nlohmann::json json, Vector2 pos, Vector2 dir, bool player);
	static Ecs::entity_type createProjectile(nlohmann::json json, Vector2 pos, Vector2 dir, bool player,
		std::function<void(std::any, float)> update);
};
#endif