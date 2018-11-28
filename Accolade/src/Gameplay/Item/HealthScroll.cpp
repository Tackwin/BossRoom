#include "HealthScroll.hpp"

#include "Managers/AssetsManager.hpp"
#include "Gameplay/Player/Player.hpp"

#define FROM(x) info.x = json.at(#x)

void from_json(const nlohmann::json& json, HealthScrollInfo& info) noexcept {
	FROM(health_regen);

	FROM(texture).get<std::string>();
}

#undef FROM
#define TO(x) json[#x] = info.x

void to_json(nlohmann::json& json, const HealthScrollInfo& info) noexcept {
	TO(health_regen);


	TO(texture);
}

#undef TO

HealthScroll::HealthScroll(const HealthScrollInfo& info) noexcept : info(info) {
	name = "HealthScroll";
}
HealthScroll::~HealthScroll() noexcept {
	assert(!event_subscribe_key);
}

void HealthScroll::mount() noexcept {
	event_subscribe_key = EM::subscribe(
		EM::EventType::Player_Consume_Binded,
		std::bind(&HealthScroll::playerUseItem, this, std::placeholders::_1)
	);
}
void HealthScroll::unMount() noexcept {
	if (event_subscribe_key) {
		EM::unSubscribe(EM::EventType::Player_Consume_Binded, event_subscribe_key);
		event_subscribe_key.nullify();
	}
}

void HealthScroll::update(double) noexcept {}
void HealthScroll::render(sf::RenderTarget&) noexcept {}

ValuePtr<ItemInfo> HealthScroll::getInfo() const noexcept {
	return { new HealthScrollInfo{info} };
}

Item::Type HealthScroll::getItemType() const noexcept {
	return Item::Type::Conso;
}

sf::Texture& HealthScroll::getIconTexture() const noexcept {
	return AM::getTexture(info.texture);
}

Clonable* HealthScroll::clone() const noexcept {
	return new HealthScroll{ info };
}

void HealthScroll::playerUseItem(EM::EventCallbackParameter args) noexcept {
	assert(args.size() == 1);
	assert(args.begin()->type() == typeid(Player*));

	Player& player = *std::any_cast<Player*>(*args.begin());

	player.heal(info.health_regen);
	remove();
}

dyn_struct HealthScroll::get_item_desc() const noexcept {
	return {
		{"Name", "Health Scroll"},
		{"Effect", "Soigne des meres"}
	};
}
