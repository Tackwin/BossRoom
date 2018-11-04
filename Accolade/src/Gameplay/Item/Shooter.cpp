#include "Shooter.hpp"

#include "Managers/AssetsManager.hpp"
#include "Gameplay/Player/Player.hpp"
#include "Gameplay/Projectile.hpp"

#define FROM(x) info.x = json.at(#x)

void from_json(const nlohmann::json& json, ShooterInfo& info) noexcept {
	FROM(fire_rate);
	FROM(recoil_force);
	
	FROM(projectile);

	FROM(texture).get<std::string>();
}

#undef FROM
#define TO(x) json[#x] = info.x

void to_json(nlohmann::json& json, const ShooterInfo& info) noexcept {
	TO(fire_rate);
	TO(recoil_force);

	TO(projectile);
	
	TO(texture);
}

#undef TO

Shooter::Shooter(const ShooterInfo& info) noexcept : info(info) {
	name = "Shooter";
}

Shooter::~Shooter() noexcept {
	assert(!mounted);
}

// >TOSEE
// I probably want to move away from this global event system, it really doesn't seems to be
// the right place for a global system. I can already see getting myself lost when i will
// not remember who subscribed to who/m. besides i already have schenanigans to do to
// remeber to unsub myself as a subscribtion is not bound to one's lifetime.
// maybe going back to a polling system where the weapon in this case will poll events from
// the wearer.
// Each frame the event queue will be reset, ... i don't know.
// 4.11.2019
void Shooter::mount() noexcept {
	subscribe_shoot_event_key = EM::subscribe(
		EM::EventType::Player_Use_Main_Weapon,
		std::bind(&Shooter::playerUseMainWeapon, this, std::placeholders::_1)
	);
	mounted = true;
}
void Shooter::unMount() noexcept {
	if (subscribe_shoot_event_key) {
		EM::unSubscribe(EM::EventType::Player_Use_Main_Weapon, subscribe_shoot_event_key);
		subscribe_shoot_event_key = UUID::zero();
	}
	mounted = false;
}


void Shooter::update(double dt) noexcept {
	fire_cooldown -= (float)dt;
}
void Shooter::render(sf::RenderTarget&) noexcept {}

ValuePtr<ItemInfo> Shooter::getInfo() const noexcept {
	return { new ShooterInfo{info} };
}

Item::Type Shooter::getItemType() const noexcept {
	return Item::Type::Main_Weapon;
}

sf::Texture& Shooter::getIconTexture() const noexcept {
	return AM::getTexture(info.texture);
}

Clonable* Shooter::clone() const noexcept {
	return new Shooter{ info };
}

void Shooter::playerUseMainWeapon(EM::EventCallbackParameter args) noexcept {
	assert(args.size() == 1);
	assert(args.begin()->type() == typeid(Player*));

	Player& player = *std::any_cast<Player*>(*args.begin());

	if (fire_cooldown > 0.0) return;
	even_shot = !even_shot;
	fire_cooldown = 1.f / info.fire_rate;

	Vector2f dir = player.getFacingDir();
	player.applyVelocity(dir * -info.recoil_force);

	float a = (float)dir.angleX();
	a += float((even_shot ? -1 : 1) * PIf / 15);

	Vector2f pos = player.support(a, 0);

	player.addProjectile(std::make_shared<Projectile>(info.projectile, pos, dir, true));
}
