#pragma once
#ifdef ECS
#include <any>
#include <memory>

#include <SFML/System.hpp>
#include <enTT/registry.hpp>

#include "Vector2.hpp"

namespace Comp {
	struct Transform;
	struct Velocity;
	struct MovementInput;
	struct Data;
	struct Script;
	struct Sprite;
	struct CircleCollider;
	struct Player;
	struct Projectile;
	struct Boss;
	struct Remove;
};


#define ALL_COMPS \
Comp::Transform,\
Comp::Velocity,\
Comp::MovementInput,\
Comp::Data,\
Comp::Script,\
Comp::Sprite,\
Comp::CircleCollider,\
Comp::Player,\
Comp::Projectile,\
Comp::Boss,\
Comp::Remove
using Ecs = entt::DefaultRegistry<ALL_COMPS>;


namespace Comp {

#pragma region TAGS
	struct Player {}; struct Projectile {}; struct Remove {}; struct Boss {};
#pragma endregion


struct Transform {
	Vector2 pos;
	Vector2 factor;
	float rotation;

	Transform(Vector2 pos = { 0, 0 }, Vector2 factor = { 1, 1 }, float rotation = 0) :
		pos(pos), factor(factor), rotation(rotation) {};
};

struct Velocity {
	Vector2 dir;

	float v;
	float vA;

	Velocity(Vector2 dir = { 1, 0 }, float v = 0, float vA = 0) :
		dir(dir), v(v), vA(vA) {};
};

struct MovementInput {
	sf::Keyboard::Key up{ sf::Keyboard::Z };
	sf::Keyboard::Key left{ sf::Keyboard::Q };
	sf::Keyboard::Key down{ sf::Keyboard::S };
	sf::Keyboard::Key right{ sf::Keyboard::D };
	sf::Keyboard::Key special{ sf::Keyboard::LShift };

	MovementInput(sf::Keyboard::Key up = sf::Keyboard::Z, sf::Keyboard::Key left = sf::Keyboard::Q,
					sf::Keyboard::Key down = sf::Keyboard::S, sf::Keyboard::Key right = sf::Keyboard::D,
					sf::Keyboard::Key special = sf::Keyboard::LShift) :
		up(up), left(left), down(down), right(right), special(special) {}
};
	
struct Data {
	using Key = std::string;
	using Value = std::any;

	std::unordered_map<std::string, std::any> datas;

	Data(std::unordered_map<std::string, std::any> datas = {}) :
		datas(datas) {};

	std::any operator[](const std::string& str) {
		return datas[str];
	}
};

struct Script {
	using Key = std::string;
	using Value = std::function<std::any(std::any)>;

	std::unordered_map<Key, Value> scripts;
	std::function<void(std::any, float)> update;

	Script(std::function<void(std::any, float)> update, std::unordered_map<Key, Value> scripts = {}) :
		update(update), scripts(scripts) {};
};

struct Sprite {
	sf::Sprite sprite;
	std::string view;

	Sprite(const sf::Sprite& sprite, std::string view = "default") :
		sprite(sprite), view(view) {};
};

struct CircleCollider {
	using Callback = std::function<void(Ecs::entity_type, Ecs::entity_type)>;

	float r;
	bool solid;
	bool sensor;
	bool colliding = false;
	bool justCollided = false;
	
	Ecs::entity_type collideWith;

	Callback onEnter;

	CircleCollider(float r, bool solid = true, bool sensor = true, Callback onEnter = [](Ecs::entity_type, Ecs::entity_type){}) : 
		r(r), solid(solid), sensor(sensor), onEnter(onEnter) {};
};

};
#endif