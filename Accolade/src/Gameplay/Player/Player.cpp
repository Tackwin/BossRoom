#include "Player.hpp"

#include "./../../Game.hpp"
#include "./../../Common.hpp"

#include "./../../Managers/InputsManager.hpp"
#include "./../../Managers/AssetsManager.hpp"
#include "./../../Managers/TimerManager.hpp"
#include "./../../Managers/EventManager.hpp"

#include "./../Projectile.hpp"
#include "./../Weapon.hpp"

Player::Player() : 
	Object(), _info(game->getPlayerInfo()), _hitSound(AssetsManager::getSound("hit"))
{
	_life = _info.maxLife;
	_sprite = AnimatedSprite(_info.sprite);
	_sprite.pushAnim("idle");

	_sprite.getSprite().setOrigin(
		_sprite.getSprite().getGlobalBounds().width / 2.f,
		_sprite.getSprite().getGlobalBounds().height / 2.f
	);
	_sprite.getSprite().setPosition(pos);

	_hitSound.setVolume(SOUND_LEVEL);

	collider = std::make_unique<Disk>();
	_hitBox = (Disk*)collider.get();
	_hitBox->userPtr = this;
	_hitBox->r = _info.radius;

	pos = { 100, 100 };
	idMask.set((size_t)Object::BIT_TAGS::PLAYER);
	collisionMask.set((size_t)Object::BIT_TAGS::FLOOR);

	if (isEquiped()) {
		swapWeapon(_info.weapon);
	}


}
void Player::enterLevel(Level* level) {
 	_level = level;

	_hitBox->onEnter = [&](Object* obj) { collision(obj); };
}
void Player::exitLevel() {
	_level = nullptr;
}

void Player::update(double dt) {
	auto& kb = game->getCurrentKeyBindings();

	bool tryingToShoot = 
		InputsManager::isMousePressed(kb.getKey(KeyBindings::ACTION)) && 
		_focus;

	if (tryingToShoot) {
		_weapon.active(*this, 0);
	}

	if (!_freeze) {
		if (_jumping && 
			!InputsManager::isKeyPressed(kb.getKey(KeyBindings::JUMP))
		) {
			_jumping = false;
		}

		if (InputsManager::isKeyPressed(kb.getKey(KeyBindings::JUMP)) && _jumping) {
			flatVelocities.push_back({ 0, -300 });
		}

		_dir.normalize();
		if (tryingToShoot)
			_dir.x *= 0.2f;
		if (InputsManager::isKeyPressed(kb.getKey(KeyBindings::JUMP)))
			_dir.x *= _info.specialSpeed;
		else
			_dir.x *= _info.speed;

		flatForces.push_back({ 0, G });
		flatVelocities.push_back(_dir);
	}

	velocity *= (float)std::pow(0.2, dt);

	const auto& projectileBuffer = _weapon.getProjectileBuffer();
	_projectilesToShoot.insert(
		_projectilesToShoot.end(),
		projectileBuffer.begin(),
		projectileBuffer.end()
	);
	_weapon.clearProjectileBuffer();
}

void Player::render(sf::RenderTarget &target) {
	_sprite.getSprite().setPosition(pos);
	_sprite.render(target);

}
void Player::shoot() {
	_sprite.pushAnim("action");
}

void Player::hit(float d) {
	if (_invincible) return;
	_invincible = true;

	if (_hitSound.getStatus() == sf::Sound::Stopped)
		_hitSound.play();

	_life -= d;
	_sprite.getSprite().setColor(sf::Color(230, 230, 230));
	TimerManager::addFunction(0.33f, "blinkDown", 
		[&, n = 0](double) mutable -> bool {
			_sprite.getSprite().setColor(
				(n++ % 2 == 0) 
					? sf::Color::White 
					: sf::Color(230, 230, 230)
			);
			if (n >= 3) {
				_invincible = false;
				return true;
			}
			return false;
		}
	);
}

void Player::startCaC() {
	TimerManager::restartFunction(_keyCdActionCaC);
	_sprite.getSprite().setColor(sf::Color::Blue);
	_freeze = true;
}

UUID Player::getWeapon() const noexcept {
	return _info.weapon;
}
bool Player::isEquiped() const noexcept {
	return _info.weapon != UUID::null;
}


void Player::swapWeapon(UUID weapon) {
	auto info = game->getPlayerInfo();
	info.weapon = weapon;
	game->setPlayerInfo(info);
	if (isEquiped()) _weapon.unEquip(*this);
	_weapon = Weapon(Weapon::_weapons[weapon]);
	_weapon.equip(*this);
	_info.weapon = weapon;
}

Vector2f Player::getDirToFire() {
	return (InputsManager::getMouseWorldPos() - pos).normalize();
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

void Player::collision(Object* obj) {
	if (auto ptr = static_cast<Projectile*>(obj); 
			ptr->idMask.test((size_t)Object::BIT_TAGS::PROJECTILE) &&
			!ptr->isFromPlayer()
	) {
		if (!_invincible) {
			hit(ptr->getDamage());
		}
		ptr->remove();
	}
}

void Player::floored() {
	_nJumpsLeft = 2u;
	_jumping = false;
	_floored = true;
}
bool Player::isFloored() const {
	return _floored;
}

void Player::jumpKeyPressed() {
	if (_nJumpsLeft > 0u) {
		if (_floored)
			_floored = false;

		_jumping = true;

		velocity.y = -sqrtf(G * 2.f * _info.jumpHeight); // derived from equation of motion, thanks Newton
		_nJumpsLeft--;
	}
}

void Player::keyPressed(sf::Keyboard::Key key) {
	auto kb = game->getCurrentKeyBindings();
	if (key == kb.getKey(KeyBindings::JUMP)) {
		jumpKeyPressed();
	}

	if (key == kb.getKey(KeyBindings::MOVE_UP)) {
		_dir.y = -1;
	}
	else if (key == kb.getKey(KeyBindings::MOVE_DOWN)) {
		_dir.y = 1;
	}

	if (key == kb.getKey(KeyBindings::MOVE_LEFT)) {
		_dir.x = -1;
	}
	else if (key == kb.getKey(KeyBindings::MOVE_RIGHT)) {
		_dir.x = 1;
	}
}

void Player::keyReleased(sf::Keyboard::Key key) {
	auto kb = game->getCurrentKeyBindings();
	if (key == kb.getKey(KeyBindings::MOVE_UP)) {
		_dir.y = std::max(_dir.y, 0.f);
	}
	else if (key == kb.getKey(KeyBindings::MOVE_DOWN)) {
		_dir.y = std::min(_dir.y, 0.f);
	}

	if (key == kb.getKey(KeyBindings::MOVE_LEFT)) {
		_dir.x = std::max(_dir.x, 0.f);
	}
	else if (key == kb.getKey(KeyBindings::MOVE_RIGHT)) {
		_dir.x = std::min(_dir.x, 0.f);
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
	velocity += v;
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

void Player::unEquip() {
	_weapon.unEquip(*this);
	_info.weapon = UUID::null;
}