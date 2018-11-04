#pragma once

#include "Item.hpp"

#include "3rd/json.hpp"
#include "Utils/UUID.hpp"
#include "Managers/EventManager.hpp"

struct ShooterInfo : public ItemInfo {
	static constexpr auto JSON_ID = "ShooterInfo";

	float fire_rate{ NAN };
	float recoil_force{ NAN };
	
	nlohmann::json projectile;

	std::string texture{ "__" };
};

extern void from_json(const nlohmann::json& json, ShooterInfo& info) noexcept;
extern void to_json(nlohmann::json& json, const ShooterInfo& info) noexcept;


class Shooter : public Item {

public:
	Shooter(const ShooterInfo& info) noexcept;

	// See my definition.
	virtual ~Shooter() noexcept;
	Shooter (Shooter&&) = default;
	Shooter & operator=(Shooter&&) = default;

	virtual void mount() noexcept override;
	virtual void unMount() noexcept override;

	virtual void update(double dt) noexcept override;
	virtual void render(sf::RenderTarget& target) noexcept override;

	virtual ValuePtr<ItemInfo> getInfo() const noexcept override;
	virtual Type getItemType() const noexcept override;
	virtual sf::Texture& getIconTexture() const noexcept override;

	virtual Clonable* clone() const noexcept override;
private:

	void playerUseMainWeapon(EM::EventCallbackParameter args) noexcept;

	ShooterInfo info;

	UUID subscribe_shoot_event_key{ UUID::zero() };

	float fire_cooldown{ 0.f };

	bool mounted{ false };
	bool even_shot{ false };
};

