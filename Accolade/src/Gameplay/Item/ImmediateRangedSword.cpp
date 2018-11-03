#include "ImmediateRangedSword.hpp"

#include "Managers/AssetsManager.hpp"
#include "Managers/TimerManager.hpp"
#include "Gameplay/Player/Player.hpp"
#include "Components/Bumpable.hpp"
#include "Gameplay/Zone.hpp"

#define FROM(x) info.x = json.at(#x)

void from_json(const nlohmann::json& json, ImmediateRangedSwordInfo& info) noexcept {
	FROM(distance);
	FROM(blast_radius);
	FROM(damage);
	FROM(remain_time);
	FROM(recoil_force);
	FROM(cooldown);

	FROM(remain_texture).get<std::string>();
	FROM(texture).get<std::string>();
}

#undef FROM
#define TO(x) json[#x] = info.x

void to_json(nlohmann::json& json, const ImmediateRangedSwordInfo& info) noexcept {
	TO(distance);
	TO(blast_radius);
	TO(damage);
	TO(remain_time);
	TO(cooldown);
	TO(recoil_force);

	TO(remain_texture);
	TO(texture);
}

#undef TO

ImmediateRangedSword::ImmediateRangedSword(const ImmediateRangedSwordInfo& info) noexcept :
	info(info)
{
	name = "IRSword";
}


void ImmediateRangedSword::mount() noexcept {
	mounted = true;
	subscribe_main_weapon_key = EM::subscribe(
		EM::EventType::Player_Use_Main_Weapon,
		std::bind(&ImmediateRangedSword::playerUseMainWeapon, this, std::placeholders::_1)
	);
}
void ImmediateRangedSword::unMount() noexcept {
	mounted = false;
	if (subscribe_main_weapon_key) {
		EM::unSubscribe(EM::EventType::Player_Use_Main_Weapon, subscribe_main_weapon_key);
		subscribe_main_weapon_key = UUID::zero();
	}
}

void ImmediateRangedSword::update(double dt) noexcept {
	use_cooldown -= (float)dt;
}
void ImmediateRangedSword::render(sf::RenderTarget&) noexcept {

}

ValuePtr<ItemInfo> ImmediateRangedSword::getInfo() const noexcept {
	return { new ImmediateRangedSwordInfo{info} };
}

sf::Texture& ImmediateRangedSword::getIconTexture() const noexcept {
	return AM::getTexture(info.texture);
}

Item::Type ImmediateRangedSword::getItemType() const noexcept {
	return Item::Type::Main_Weapon;
}

Clonable* ImmediateRangedSword::clone() const noexcept {
	return new ImmediateRangedSword{ info };
}

void ImmediateRangedSword::playerUseMainWeapon(EM::EventCallbackParameter args) noexcept {
	assert(args.size() == 1);
	assert(args.begin()->type() == typeid(Player*));

	if (use_cooldown > 0.f) return;
	use_cooldown = info.cooldown;

	Player& player = *std::any_cast<Player*>(*args.begin());
	std::shared_ptr<Zone> zone = std::make_shared<Zone>(info.blast_radius);

	bool front = player.getFacingDir().x > 0.f;

	zone->pos = player.support(front ? 0 : PIf, info.distance);

	zone->addSprite(
		info.remain_texture,
		sf::Sprite(AssetsManager::getTexture(info.remain_texture))
	);

	zone->collisionMask.set((u08)Object::BIT_TAGS::BOSS);
	zone->collisionMask.set((u08)Object::BIT_TAGS::ENNEMY);
	zone->entered = [recoil = info.recoil_force, dmg = info.damage, pos = player.getPos()]
	(Object* object) {
		if (auto hited = dynamic_cast<Hitable*>(object); hited) {
			hited->hit(dmg);
		}
		if (auto bumpee = dynamic_cast<Bumpable*>(object); bumpee) {
			bool dir_x = pos.x > object->pos.x;

			bumpee->bump({ (float)((dir_x ? -1 : 1) * recoil), 0 });
		}
	};

	TM::addFunction(info.remain_time, [z = std::weak_ptr<Zone>{ zone }](auto) -> bool {
		if (z.expired()) return true;

		z.lock()->remove();
		return true;
	});
	player.addZone(zone);
}