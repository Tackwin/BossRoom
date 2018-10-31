#include "AxisAlignedShooter.hpp"
#include "Gameplay/Player/Player.hpp"
#include "Gameplay/Projectile.hpp"
#include "Managers/AssetsManager.hpp"


#define FROM(x) info.x = json.at(#x)

void from_json(const nlohmann::json& json, AxisAlignedShooterInfo& info) noexcept {
	if (json.count("texture")) FROM(texture).get<std::string>();
	FROM(fire_rate);
	FROM(projectile);
}

#undef FROM
#define TO(x) json[#x] = info.x

void to_json(nlohmann::json& json, const AxisAlignedShooterInfo& info) noexcept {
	TO(texture);
	TO(fire_rate);
	TO(projectile);
}

#undef TO

using AAShooter = AxisAlignedShooter;

AAShooter::AxisAlignedShooter(const AxisAlignedShooterInfo& info) noexcept
	: info(info)
{
	name = "AAShooter";
}

AAShooter::~AxisAlignedShooter() noexcept {
	// I'm a little defenssive here, but i really want to make sure that in the future
	// Nothing bad happens.
	assert(!mounted);
}

void AAShooter::mount() noexcept {
	subscribe_shoot_event_key = EM::subscribe(
		EM::EventType::Player_Use_Main_Weapon,
		std::bind(&AAShooter::playerShooted, this, std::placeholders::_1)
	);

	mounted = true;
}

void AAShooter::unMount() noexcept {
	if (subscribe_shoot_event_key)
		EM::unSubscribe(EM::EventType::Player_Use_Main_Weapon, subscribe_shoot_event_key);
	mounted = false;
}

void AAShooter::update(double dt) noexcept {
	fire_cooldown -= (float)dt;
}

void AAShooter::render(sf::RenderTarget&) noexcept {

}

void AAShooter::remove() noexcept {
	to_remove = true;
}
bool AAShooter::toRemove() const noexcept {
	return to_remove;
}

void AAShooter::playerShooted(EM::EventCallbackParameter args) noexcept {
	assert(args.size() == 1);
	assert(args.begin()->type() == typeid(Player*));

	if (fire_cooldown > 0.f) return;
	fire_cooldown = 1.f / info.fire_rate;

	Player& player = *std::any_cast<Player*>(*args.begin());
	
	float angle = (float)player.getFacingDir().angleX();

	angle /= PIf / 4.f;
	angle = std::roundf(angle);
	angle *= PIf / 4.f;

	auto projectile = std::make_shared<Projectile>(
		info.projectile,
		player.support(angle, info.projectile["radius"]),
		Vector2f::createUnitVector(angle),
		true
	);
	
	player.addProjectile(projectile);
}

Item::Type AAShooter::getItemType() const noexcept {
	return Type::Main_Weapon;
}

Clonable* AAShooter::clone() const noexcept {
	return new AAShooter{ info };
}

sf::Texture& AAShooter::getIconTexture() const noexcept {
	return AM::getTexture(info.texture);
}

ValuePtr<ItemInfo> AAShooter::getInfo() const noexcept {
	return { new AxisAlignedShooterInfo(info) };
}