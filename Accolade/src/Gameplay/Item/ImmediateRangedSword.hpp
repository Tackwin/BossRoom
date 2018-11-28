#pragma once

#include "Item.hpp"

#include "3rd/json.hpp"
#include "Utils/UUID.hpp"
#include "Time/Scheduler.hpp"
#include "Managers/EventManager.hpp"

struct ImmediateRangedSwordInfo : public ItemInfo {
	static constexpr auto JSON_ID = "ImmediateRangedSwordInfo";

	float distance{ NAN };
	float blast_radius{ NAN };
	float damage{ NAN };
	float remain_time{ NAN };
	float cooldown{ NAN };
	float recoil_force{ NAN };

	std::string remain_texture;
	std::string texture;

};

extern void from_json(const nlohmann::json& json, ImmediateRangedSwordInfo& info) noexcept;
extern void to_json(nlohmann::json& json, const ImmediateRangedSwordInfo& info) noexcept;

class Zone;
class ImmediateRangedSword : public Item {
public:

	ImmediateRangedSword(const ImmediateRangedSwordInfo& info) noexcept;

	virtual void mount() noexcept override;
	virtual void unMount() noexcept override;

	virtual void update(double dt) noexcept override;
	virtual void render(sf::RenderTarget& target) noexcept override;

	virtual ValuePtr<ItemInfo> getInfo() const noexcept override;

	virtual sf::Texture& getIconTexture() const noexcept override;

	virtual Type getItemType() const noexcept;
	
	virtual Clonable* clone() const noexcept override;

	virtual dyn_struct get_item_desc() const noexcept override;

private:

	void playerUseMainWeapon(EM::EventCallbackParameter args) noexcept;

	ImmediateRangedSwordInfo info;

	Scheduler scheduler;

	UUID subscribe_main_weapon_key{ UUID::zero() };

	bool mounted{ false };

	float use_cooldown{ 0.f };
};

