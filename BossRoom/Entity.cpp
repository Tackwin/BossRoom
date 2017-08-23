#ifdef ECS

#include "Entity.hpp"
#include "TimerManager.hpp"
#include "AssetsManager.hpp"

Ecs::entity_type Entity::createProjectile(nlohmann::json json, Vector2 pos, Vector2 dir, bool player) {
	return createProjectile(json, pos, dir, player, [](std::any, float)->void {});
}
Ecs::entity_type Entity::createProjectile(nlohmann::json json, Vector2 pos, Vector2 dir, bool player,
	std::function<void(std::any, float)> update) { using namespace Comp;

	const std::string idleSprite = json["sprites"]["idle"];
	AssetsManager::loadTexture(idleSprite, std::string(ASSETS_PATH) + idleSprite);

	std::uniform_int_distribution<int32> dist(0, 2);

	sf::Sprite sprite = sf::Sprite(AssetsManager::getTexture(idleSprite));
	const int32 whole = sprite.getTextureRect().width / 3;
	sprite.setTextureRect({
		dist(RNG) * whole	, dist(RNG) * whole,
		whole				,	whole
	});
	sprite.setOrigin(json["radius"], json["radius"]);

	auto entity = ecs.create();
	ecs.assign<Transform>(entity, pos);
	ecs.assign<Velocity>(entity, dir, json["speed"]);
	ecs.assign<CircleCollider>(entity, json["radius"], false, true, [](Ecs::entity_type A, Ecs::entity_type B) {
		auto& data = ecs.get<Data>(A);
		if ((std::any_cast<bool>(data["player"]) && ecs.has<Comp::Boss>(B)) ||
			(!std::any_cast<bool>(data["player"]) && ecs.has<Comp::Player>(B))) 
		{
			ecs.assign<Remove>(A);
		}
	});
	ecs.assign<Script>(entity, update);
	ecs.assign<Data>(entity, std::unordered_map<Data::Key, Data::Value>{
		{ "player", player },
		{ "damage", json["damage"] },
		{ "lifespan", json["lifespan"] }
	});
	ecs.assign<Sprite>(entity, sprite);
	ecs.assign<Projectile>(entity);

	TimerManager::addFunction(json["lifespan"], "destroy", [&](float)->bool {
		if (ecs.valid(entity))
			ecs.destroy(entity);
		return true;
	});

	return entity;
}

#endif