#pragma once

#include "Item.hpp"

#include "3rd/json.hpp"
#include "Utils/UUID.hpp"
#include "Managers/EventManager.hpp"

struct HealthScrollInfo : public ItemInfo {
	static constexpr auto JSON_ID = "HealthScrollInfo";

	float health_regen{ NAN };

	std::string texture{ "__" };
};

extern void from_json(const nlohmann::json& json, HealthScrollInfo& info) noexcept;
extern void to_json(nlohmann::json& json, const HealthScrollInfo& info) noexcept;


class HealthScroll : public Item {

public:
	HealthScroll(const HealthScrollInfo& info) noexcept;
	virtual ~HealthScroll() noexcept;

	virtual void mount() noexcept override;
	virtual void unMount() noexcept override;

	virtual void update(double dt) noexcept override;
	virtual void render(sf::RenderTarget& target) noexcept override;

	virtual ValuePtr<ItemInfo> getInfo() const noexcept override;
	virtual Type getItemType() const noexcept override;
	virtual sf::Texture& getIconTexture() const noexcept override;

	virtual Clonable* clone() const noexcept override;

	virtual dyn_struct get_item_desc() const noexcept override;
private:

	void playerUseItem(EM::EventCallbackParameter args) noexcept;

	HealthScrollInfo info;

	UUID event_subscribe_key{ UUID::zero() };
};

