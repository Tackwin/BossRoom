#pragma once

#include <SFML/Graphics.hpp>

#include "3rd/json.hpp"

#include "Graphics/AnimatedSprite.hpp"

#include "Physics/Object.hpp"

#include "Math/RandomData.hpp"

#include "Components/Hitable.hpp"
#include "Components/Removable.hpp"

struct FlyInfo {
	static constexpr auto JSON_ID = "FlyInfo";

	Vector2f startPos;

	RandomData<float> timeToDive;

	float maxHealth{ NAN };
	float minDistance{ NAN };
	float damage{ NAN };
	float radius{ NAN };
	float speed{ NAN };
	float divingSpeed{ NAN };
	float timeToDecide{ NAN };

	static nlohmann::json saveJson(FlyInfo info) noexcept;
	static FlyInfo loadJson(nlohmann::json json) noexcept;

};

class Section;
class Fly : public Object, public Removable, public Hitable {

public:

	Fly(FlyInfo info) noexcept;

	Fly(Fly&) = delete;
	Fly& operator=(Fly&) = delete;

	void enterSection(Section* section) noexcept;
	void leaveSection() noexcept;

	void render(sf::RenderTarget& target) noexcept;
	void update(double dt) noexcept;

	virtual bool toRemove() const noexcept override;
	virtual void remove() noexcept override;

	virtual void hit(float damage) noexcept;
private:

	void onEnter(Object*) noexcept;
	void onExit(Object*) noexcept;

	void updateDiving(double dt) noexcept;
	void updateDeciding(double dt) noexcept;
	void updatePreparingToDive(double dt) noexcept;

	FlyInfo info;

	float health{ NAN };
	float angleToPlayer{ 0.f };
	float timeToDive{ 0.f };
	float timeToDecide{ 0.f };

	std::optional<Vector2f> posToDiveTo;

	bool diving{ false };

	bool removed{ false };

	AnimatedSprite aSprite;

	Section* section{ nullptr };

	std::optional<Vector2f> divingVel;
	std::optional<Vector2f> rotatingVel;
};









