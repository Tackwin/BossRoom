#include <iostream>

#include "Player.hpp"

#include "Game.hpp"
#include "Common.hpp"

#include "Managers/InputsManager.hpp"
#include "Managers/AssetsManager.hpp"
#include "Managers/TimerManager.hpp"
#include "Managers/EventManager.hpp"

#include "Ruins/Section.hpp"

#include "Gameplay/Projectile.hpp"
#include "Gameplay/Wearable/Wearable.hpp"

#include "Ruins/Structure/Plateforme.hpp"

#include "Events/Event.hpp"

#include "Gameplay/Item/Item.hpp"

#include "Entity/OwnId.hpp"

Player::Player() noexcept : Player(C::game->getPlayerInfo()) {}

Player::Player(const PlayerInfo& info) noexcept :
	Object(), _info(info), _hitSound(AssetsManager::getSound("hit"))
{
	_life = _info.maxLife;
	_sprite = AnimatedSprite(_info.sprite);
	_sprite.pushAnim("idle");

	origin = { 0.5, 1 };

	_sprite.getSprite().setOrigin(
		_sprite.getSprite().getGlobalBounds().width * origin.x,
		_sprite.getSprite().getGlobalBounds().height * origin.y
	);
	_sprite.getSprite().setPosition(pos);

	_hitSound.setVolume(SOUND_LEVEL);

	collider = std::make_unique<Box>();
	_hitBox = (Box*)collider.get();
	_hitBox->userPtr = this;
	_hitBox->setSize(_info.hitBox);
	_hitBox->dtPos = { -_info.hitBox.x * origin.x, -_info.hitBox.y * origin.y };
	_hitBox->onEnter = [&](Object* obj) { onEnter(obj); };
	_hitBox->onExit = [&](Object* obj) { onExit(obj); };

	pos = { 100, 100 };
	idMask.set((size_t)Object::BIT_TAGS::PLAYER);
	collisionMask.set((size_t)Object::BIT_TAGS::STRUCTURE);

	collider->filterCollide =
		std::bind(&Player::filterCollision, this, std::placeholders::_1);

	for (auto& i : _info.items) {
		OwnId<Item> item = es_instance->integrate(make_item(i));
		es_instance->get(item)->mount();
		own_items.push_back(std::move(item));
	}
}
void Player::enterLevel(Level* level) noexcept {
 	_level = level;

}
void Player::exitLevel() noexcept {
	_level = nullptr;
}

void Player::update(double dt) {
	auto& kb = game->getCurrentKeyBindings();

	if (_invincible) {
		_invincibleTime -= dt;
		if (_invincibleTime < 0.0) {
			_invincible = false;
		}
	}

	_weapon.update(dt);
	for (size_t i = own_items.size() - 1; i + 1 > 0; i--) {
		auto item = es_instance->get(own_items[i]);

		item->update(dt);
		if (item->toRemove()) {
			item->unMount();
			own_items.erase(std::begin(own_items) + i);
		}
	}

	if (!_freeze) {
		if (_knockBack) {
			if (_knockBackTime > 0.0) {
				auto& knock = _knockBack.value();
				knock *= std::pow(0.2, dt);
				flatVelocities.push_back(knock);
				_knockBackTime -= (float)dt;
			}
			else {
				_knockBack = {};
			}
		}
		else {
			if (jumpVelocity_) {
				auto& vel = jumpVelocity_.value();
				vel *= std::pow(0.2, dt);
				flatVelocities.push_back(vel);
				
				if (!downed_jump && IM::isKeyJustPressed(kb.getKey(KeyBindings::MOVE_DOWN)))
				{
					*jumpVelocity_ *= .5f;
					continueToJump_ = false;
					downed_jump = false;
				}
				if (vel.length2() < .01f) jumpVelocity_.reset();
			}

			if (continueToJump_) {
				if (!InputsManager::isKeyPressed(kb.getKey(KeyBindings::JUMP))) {
					continueToJump_ = false;
				} else {
					flatVelocities.push_back({ 0, -_info.jumpHeight });
				}

				if (IM::isKeyPressed(kb.getKey(KeyBindings::MOVE_UP))) {
					flatVelocities.push_back(
						{ 0, -_info.jumpHeight * _info.jumpBoostMultiplier }
					);
				}

			}


			_dir.normalize();
		
			if (InputsManager::isKeyPressed(kb.getKey(KeyBindings::SPECIAL_SPEED))) {
				_dir.x *= _info.specialSpeed;
			}
			else {
				_dir.x *= _info.speed;
			}
			_dir.y = 0;

			flatVelocities.push_back(_dir);
		}

		if (isBoomerangSpellAvailable()) updateBoomerangSpell(dt);
		if (isDirectionSpellAvailable()) updateDirectionSpell(dt);
	}

	flatForces.push_back({ 0, G });
	velocity *= (float)std::pow(0.2, dt);

	input();
}
void Player::input() noexcept {
	auto key_binding = game->getCurrentKeyBindings();

	_dir = { 0, 0 };

	if (IM::isKeyPressed(key_binding.getKey(KeyBindings::MOVE_UP))) {
		_dir.y = -1;
	}
	else if (IM::isKeyPressed(key_binding.getKey(KeyBindings::MOVE_DOWN))) {
		_dir.y = 1;
	}

	if (IM::isKeyPressed(key_binding.getKey(KeyBindings::MOVE_LEFT))) {
		_dir.x = -1;
	}
	else if (IM::isKeyPressed(key_binding.getKey(KeyBindings::MOVE_RIGHT))) {
		_dir.x = 1;
	}

	if (IM::isMousePressed(sf::Mouse::Left)) {
		EM::fire(EM::EventType::Player_Use_Main_Weapon, { this });
	}
	if (IM::isKeyJustReleased(key_binding.getKey(KeyBindings::CONSUME_BINDED))) {
		EM::fire(EM::EventType::Player_Consume_Binded, { this });
	}
}

void Player::render(sf::RenderTarget &target) {
	_sprite.getSprite().setPosition(pos);
	_sprite.getSprite().setScale(
		_info.hitBox.x / _sprite.getSprite().getTextureRect().width,
		_info.hitBox.y / _sprite.getSprite().getTextureRect().height
	);
	_sprite.render(target);

	sf::CircleShape mark{ 0.05f };
	mark.setOrigin(mark.getRadius(), mark.getRadius());
	mark.setPosition(support(_facingDir, 0));

	target.draw(mark);


	sf::CircleShape playerPosMark{ 0.05f };
	playerPosMark.setOrigin(0.05f, 0.05f);
	playerPosMark.setPosition(pos);
	target.draw(playerPosMark);
}

void Player::heal(float hp) noexcept {
	_life += hp;
	if (_life > _info.maxLife) _life = _info.maxLife;
}

void Player::hit(float d) noexcept {
	if (_invincible) return;
	_invincible = true;
	_invincibleTime = _info.invincibilityTime;

	if (_hitSound.getStatus() == sf::Sound::Stopped)
		_hitSound.play();

	_life -= d;
	_life = _life < 0 ? 0 : _life;
	_sprite.getSprite().setColor(sf::Color(230, 230, 230));
}

std::optional<std::string> Player::getWeapon() const noexcept {
	return _info.weapon;
}
bool Player::isEquiped() const noexcept {
	return _info.weapon.has_value();
}

void Player::swapWeapon(std::string weapon) {
	auto info = game->getPlayerInfo();
	info.weapon = weapon;
	game->setPlayerInfo(std::move(info));
	if (isEquiped()) _weapon.unmount();
	_weapon = Wearable(Wearable::GetWearableinfo(weapon));
	_weapon.mount(shared_from_this());
	_info.weapon = weapon;
}

void Player::addProjectile(const std::shared_ptr<Projectile>& projectile) {
	_projectilesToShoot.push_back(projectile);
}

Vector2f Player::getPos() {
	return pos;
}
void Player::setPos(const Vector2f& v) {
	pos = v;
}

const std::vector<std::shared_ptr<Projectile>>& Player::getProtectilesToShoot() const {
	return _projectilesToShoot;
}
void Player::clearProtectilesToShoot() {
	_projectilesToShoot.clear();
}

bool Player::isInvicible() const {
	return _invincible;
}

void Player::onEnter(Object* obj) noexcept {
	if (
		auto ptr = (Projectile*)obj; 
		obj->idMask.test((size_t)Object::BIT_TAGS::PROJECTILE) && !ptr->isFromPlayer()
	) {
		if (!_invincible) {
			hit(ptr->getDamage());
		}
		ptr->remove();
	}
	if (obj->idMask[Object::STRUCTURE]) {
		auto box = ((Box*)obj->collider.get())->getGlobalBoundingBox();
	
		if (getBoundingBox().isOnTopOf(box)) floored();
		if (getBoundingBox().isOnBotOf(box)) ceiled();
	}
}
void Player::onExit(Object* obj) noexcept {
	if (plateforme_colliding.count(obj->uuid) > 0) plateforme_colliding.erase(obj->uuid);
}

void Player::floored() {
	_nJumpsLeft = 2u;
	_floored = true;

	pass_though_semiPlateforme = false;
	downed_jump = false;
	clearKnockBack();
	clearJump();
}
void Player::ceiled() noexcept {
	clearJump();
	clearKnockBack();
}

void Player::clearJump() noexcept {
	jumpVelocity_.reset();
	continueToJump_ = false;
}

void Player::clearKnockBack() noexcept {
	_knockBack = {};
	_knockBackTime = 0.f;
}

bool Player::isFloored() const {
	return _floored;
}

void Player::jumpKeyPressed() {
	if (_freeze || _knockBack) return;

	if (_nJumpsLeft > 0u) {
		if (_floored)
			_floored = false;

		velocity.y = 0.f;

		continueToJump_ = true;
		pass_though_semiPlateforme = true;

		auto jumpVelocity = Vector2f{ 0.f, -sqrtf(G * 2 * _info.jumpHeight) };

		// derived from equation of motion, thanks Newton
		jumpVelocity_
			? jumpVelocity_.value() +=	jumpVelocity
			: jumpVelocity_			=	jumpVelocity;

		// we cap the velocity 
		if (jumpVelocity_.value().length2() > jumpVelocity.length2()) {
			jumpVelocity_ = jumpVelocity;
		}

		_nJumpsLeft--;
	}
}

void Player::keyPressed(sf::Keyboard::Key key) {
	auto kb = game->getCurrentKeyBindings();
	if (key == kb.getKey(KeyBindings::JUMP)) {
		jumpKeyPressed();
	}


	if (key == kb.getKey(KeyBindings::MOVE_DOWN)) {
		wanting_to_pass_semiPlatforme = true;
		pass_though_semiPlateforme = true;
	}
}

void Player::keyReleased(sf::Keyboard::Key key) {
	auto kb = game->getCurrentKeyBindings();

	if (key == kb.getKey(KeyBindings::MOVE_DOWN)) {
		wanting_to_pass_semiPlatforme = false;
	}
}

void Player::setFocus(bool focus) {
	_focus = focus;
}
bool Player::getFocus() const {
	return _focus;
}

void Player::addZone(const std::shared_ptr<Zone>& zone) {
	_zonesToApply.push_back(zone);
}
const std::vector<std::shared_ptr<Zone>>& Player::getZonesToApply() const {
	return _zonesToApply;
}
void Player::clearZonesToApply() {
	_zonesToApply.clear();
}

void Player::applyVelocity(Vector2f v) {
	flatVelocities.push_back(v);
}

PlayerInfo Player::getPlayerInfo() const {
	return _info;
}
PlayerInfo& Player::getPlayerInfo() {
	return _info;
}

bool Player::isAlive() const {
	return _life > 0;
}

float Player::getLife() const {
	return _life;
}

void Player::unmount() {
	_weapon.unmount();
	_info.weapon.reset();
	for (auto& i : own_items) {
		es_instance->get(i)->unMount();
	}
	own_items.clear();
}

void Player::knockBack(Vector2f recoil, float time) noexcept {
	if (_invincible) return;

	_knockBack = recoil;
	_knockBackTime = time;
}

Vector2f Player::getFacingDir() const noexcept {
	return Vector2f::createUnitVector(_facingDir);
}

void Player::setFacingDir(float dir) noexcept {
	_facingDir = dir;
}

Vector2f Player::support(float a, float d) const noexcept {
	return pos + Vector2f::createUnitVector(a) * (d + _info.hitBox.length());
}

PlayerInfo::PlayerInfo() {

}

PlayerInfo::PlayerInfo(nlohmann::json json) :
	maxLife(json.at("max_life")),
	speed(json.at("speed")),
	specialSpeed(json.at("special_speed")),
	invincibilityTime(json.at("invincibility_time")),
	jumpHeight(json.at("jump_height")),
	name(json.at("name").get<std::string>()),
	sprite(json.at("sprite").get<std::string>()),
	jumpBoostMultiplier(json.at("jump_boost_multiplier"))
{
	auto vec = json.at("hit_box").get<std::vector<double>>();
	hitBox = { (float)vec[0], (float)vec[1] };

	if (auto json_items = json.find("items"); json_items != std::end(json)) {
		assert(json_items->is_array());

		for (auto json_item : json_items->get<nlohmann::json::array_t>()) {
			items.push_back((ValuePtr<ItemInfo>)json_item);
		}
	}
}

void Player::enter(Section* section) noexcept {
	section_ = section;
}
void Player::leave() noexcept {
	section_ = nullptr;
	unmount();
}

// Magic spell stuff.

void Player::giveSpell(SpellTargetInfo info) noexcept {
	spellBoomerang_ =
		std::make_shared<SpellTarget>(section_, section_->getObject(uuid), info);
	section_->addSpell(spellBoomerang_);
}
void Player::giveSpell(SpellDirectionInfo info) noexcept {
	spellDirection_ =
		std::make_shared<SpellDirection>(section_, section_->getObject(uuid), info);
}

bool Player::isBoomerangSpellAvailable() const noexcept {
	return spellBoomerang_ != nullptr;
}
bool Player::isDirectionSpellAvailable() const noexcept {
	return spellDirection_ != nullptr;
}

void Player::updateBoomerangSpell(double) noexcept {
	if (IM::isMouseJustPressed(sf::Mouse::Left)) {
		auto target = section_->getTargetEnnemyFromMouse();
		if (!target) return;


		auto info = spellBoomerang_->getInfo();

		if ((target->pos - pos).length2() <= info.range * info.range) {
			spellBoomerang_->launch(target);

			// Now the object only live in the Section object.
			spellBoomerang_.reset();
		}
	}
}
void Player::updateDirectionSpell(double) noexcept {
	if (IM::isMouseJustPressed(sf::Mouse::Left)) {
		// launch
		
		auto dir = (IM::getMousePosInView(section_->getCameraView()) - pos).normalize();
		std::bitset<Object::SIZE> targetMask;
		targetMask.set(Object::ENNEMY);

		spellDirection_->launch(dir, targetMask);
		spellDirection_->setPos(pos);
		section_->addSpell(spellDirection_);
		spellDirection_.reset();
	}

}


void Player::remove() noexcept {
	remove_ = true;
}

bool Player::toRemove() const noexcept {
	return remove_;
}

Rectangle2f Player::getBoundingBox() const noexcept {
	return Rectangle2f{ collider->getGlobalPos(), ((Box*)collider.get())->getSize() };
}

bool Player::filterCollision(Object& obj) noexcept {
	if (auto ptr = dynamic_cast<Plateforme*>(&obj); ptr && ptr->isPassable()) {
		// If the player is moving up we say go for it.
		if (get_summed_velocities(*this).y < 0) return false;
		auto it = getBoundingBox();
		auto that = ptr->getBoundingBox();

		// if the player is on top of the box like we block him, that means it fall back
		// BUT
		// If the user explicitly asked to pass through, then what can we do, the player
		// shall have what s.he desire.
		if (
			!wanting_to_pass_semiPlatforme &&
			getBoundingBox().isFullyOnTopOf(ptr->getBoundingBox(), 0.1f)
		) return true;

		if (pass_though_semiPlateforme) return false;
	}
	return true;
}

bool Player::canMount(const OwnId<Item>& item) noexcept {
	auto i = es_instance->get(item);
	switch (i->getItemType())
	{
	case Item::Type::Main_Weapon:
		for (auto& x : own_items) {
			if (es_instance->get(x)->getItemType() == Item::Type::Main_Weapon)
				return false;
		}
		break;
	default:
		break;
	}
	return true;
}

void Player::mountItem(OwnId<Item>&& item) noexcept {
	es_instance->get(item)->mount();
	auto p_info = game->getPlayerInfo();
	p_info.items.push_back(std::move(es_instance->get(item)->getInfo()));
	game->setPlayerInfo(std::move(p_info));
	own_items.push_back(std::move(item));
}

const std::vector<OwnId<Item>>& Player::getItems() noexcept {
	return own_items;
}

