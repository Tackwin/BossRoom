#pragma once
#ifdef ECS

#include "Const.hpp"
#include "InputsManager.hpp"
#include <unordered_map>
#include <tuple>

class System{
public:
	static std::unordered_map<std::string, sf::View> views;
	static std::string currentView;

	static void updateVelocity(float dt) { using namespace Comp;
		
		for (auto entity : ecs.view<
			Transform,
			Velocity
		>()) {
			auto& transform = ecs.get<Transform>(entity);
			auto& velocity = ecs.get<Velocity>(entity);

			Vector2 nPos = transform.pos + velocity.dir * velocity.v * dt;
			transform.rotation += velocity.vA * dt;

			if (ecs.has<CircleCollider>(entity)) {
				auto& circleCollider = ecs.get<CircleCollider>(entity);
				circleCollider.justCollided = false;

				bool flag = false;
				for (auto entity2 : ecs.view<
					Transform,
					CircleCollider
				>()) {
					auto& transform2 = ecs.get<Transform>(entity2);
					auto& circleCollider2 = ecs.get<CircleCollider>(entity2);

					if (Vector2::equal(nPos, transform2.pos, circleCollider.r + circleCollider2.r)) {
						if (!circleCollider.colliding) {
							circleCollider.justCollided = true;
						}

						nPos = transform.pos;
						if(circleCollider.sensor) 
							circleCollider.onEnter(entity, entity2);
						flag = true;
						break;
					}
				}
				circleCollider.colliding = flag;
			}

			transform.pos = nPos;
		}
	}

	static void updateInputMovement(float) { using namespace Comp;

		for (auto entity : ecs.view<
			MovementInput,
			Velocity
		>()) {
			auto& movementInput = ecs.get<MovementInput>(entity);
			auto& velocity = ecs.get<Velocity>(entity);

			velocity.dir = Vector2::ZERO;
			if (InputsManager::isKeyPressed(movementInput.up)) {
				velocity.dir.y -= 1;
			}
			if (InputsManager::isKeyPressed(movementInput.down)) {
				velocity.dir.y += 1;
			}
			if (InputsManager::isKeyPressed(movementInput.left)) {
				velocity.dir.x -= 1;
			}
			if (InputsManager::isKeyPressed(movementInput.right)) {
				velocity.dir.x += 1;
			}

		}
	}

	static void updateRender(sf::RenderTarget& target) { using namespace Comp;
		currentView = "default";
		target.setView(views[currentView]);

		for (auto entity : ecs.view<
			Sprite,
			Transform
		>()) {
			auto& sprite = ecs.get<Sprite>(entity);
			auto& transform = ecs.get<Transform>(entity);

			sprite.sprite.setPosition(transform.pos);
			sprite.sprite.setRotation(transform.rotation * 180 / PIf);
			sprite.sprite.setScale(transform.factor);

			if (sprite.view != currentView) {	//TODO: test if i should not batch the sprite by view first, then render them
												//to avoid to change view every draw
				currentView = sprite.view;
				target.setView(views[currentView]);
			}
			target.draw(sprite.sprite);
		}
	}

	static void updateRemove() { using namespace Comp;
		std::vector<Ecs::entity_type> entToRemove;
		for (auto entity : ecs.view<
			Remove
		>()) {
			entToRemove.push_back(entity);
		}

		for (uint32_t i = entToRemove.size(); i > 0; --i) {
			//ecs.destroy(entToRemove[i - 1]);
			entToRemove.erase(entToRemove.begin() + i - 1);
		}
	}

	static void updateProjectile() { using namespace Comp;
		
		for(auto entity : ecs.view<
			Comp::Projectile
		>()){
			entity;
		}
		
	}
};
#endif
