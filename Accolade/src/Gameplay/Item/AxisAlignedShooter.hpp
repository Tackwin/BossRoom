#pragma once

#include "Item.hpp"

#include "3rd/json.hpp"
#include "Time/Scheduler.hpp"
#include "Managers/EventManager.hpp"

struct AxisAlignedShooterInfo {
	std::string texture{ "axis_aligned_shooter" };

	float fire_rate{ NAN };

	nlohmann::json projectile;
};

extern void from_json(const nlohmann::json& json, AxisAlignedShooterInfo& info) noexcept;
extern void to_json(nlohmann::json& json, const AxisAlignedShooterInfo& info) noexcept;

class AxisAlignedShooter : public Item {

public:
	AxisAlignedShooter(const AxisAlignedShooterInfo& info) noexcept;

	// See my definition.
	virtual ~AxisAlignedShooter() noexcept;
	AxisAlignedShooter(AxisAlignedShooter&&) = default;
	AxisAlignedShooter& operator=(AxisAlignedShooter&&) = default;

	virtual void mount() noexcept override;
	virtual void unMount() noexcept override;

	virtual void update(double dt) noexcept override;
	virtual void render(sf::RenderTarget& target) noexcept override;

	virtual void remove() noexcept override;
	virtual bool toRemove() const noexcept override;

	virtual Type getItemType() const noexcept override;
	virtual sf::Texture& getIconTexture() const noexcept override;

	virtual Clonable* clone() const noexcept override;
private:

	void playerShooted(EM::EventCallbackParameter args) noexcept;

	AxisAlignedShooterInfo info;

	UUID subscribe_shoot_event_key{ UUID::zero() };

	Scheduler scheduler;

	float fire_cooldown{ 0.f };
	bool to_remove{ false };
	bool mounted{ false };
};
