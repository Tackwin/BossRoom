#include "Gameplay/Player.hpp"

#include "Game.hpp"
#include "Const.hpp"

#include "Managers/InputsManager.hpp"
#include "Managers/AssetsManager.hpp"
#include "Managers/TimerManager.hpp"
#include "Managers/EventManager.hpp"

#include "Gameplay/Projectile.hpp"
#include "Gameplay/Weapon.hpp"
#include "Gameplay/Level.hpp"

//vraiment ? 8 PUTAIN DE CARACTERES pour un PUTAIN de namespace, 'tain......
using namespace nlohmann;

Player::Player(const nlohmann::json& json) :
	Object(),
	_json(json),
	_hitSound(AssetsManager::getSound("hit"))
{
	_hitSound.setVolume(SOUND_LEVEL);

	_hitBox.userPtr = this;
	collider = &_hitBox;

	pos = { 100, 100 };
	idMask.set((size_t)Object::BIT_TAGS::PLAYER);
	collisionMask.set((size_t)Object::BIT_TAGS::FLOOR);

	EventManager::subscribe("keyPressed", [&](EventManager::EventCallbackParameter args) -> void {
		auto key = std::any_cast<sf::Keyboard::Key>(*args.begin());

		keyPressed(key);
	});
	EventManager::subscribe("keyReleased", [&](EventManager::EventCallbackParameter args) -> void {
		auto key = std::any_cast<sf::Keyboard::Key>(*args.begin());
		keyReleased(key);
	});
}

void Player::initializeJson() {
	_life = _json["life"];
	_maxLife = _life;
	_speed = _json["speed"];
	_radius = _json["radius"];
	_dashRange = _json["dash"];
	pos.x = _json["startpos"]["x"];
	pos.y = _json["startpos"]["y"];
	_slowSpeed = _json["specialSpeed"];
	_AK = _json["actionButton"];
	_upK = _json["upKey"];
	_leftK = _json["leftKey"];
	_downK = _json["downKey"];
	_rightK = _json["rightKey"];
	_slowK = _json["slowKey"];
	_dashK = _json["dashKey"];
	_jumpK = _json["jumpKey"];
	_jumpHeight = getJsonValue<float>(_json, "jumpHeight");

	_invincibilityTime = _json["invincibilityTime"].get<float>();
	
	_sprite = AnimatedSprite(_json["sprite"]);
	_sprite.pushAnim("idle");

	_sprite.getSprite().setOrigin(_sprite.getSprite().getGlobalBounds().width / 2.f, _sprite.getSprite().getGlobalBounds().height / 2.f);
	_sprite.getSprite().setPosition(pos);

	_hitBox.r = _radius;
}


void Player::enterLevel(Level* level) {
	_weapon->equip();

 	_level = level;

	_hitBox.onEnter = [&](Object* obj) { collision(obj); };
}
void Player::exitLevel() {
	_weapon->unEquip();
	_level = nullptr;
}

void Player::update(double) {
	bool tryingToShoot = game->_distance && InputsManager::isMousePressed(_AK) && _focus;
	if (tryingToShoot) {
		_weapon->active(0);
	}

	if (!_freeze) {
		if (_jumping && !InputsManager::isKeyPressed(_jumpK)) {
			_jumping = false;
		}

		if (InputsManager::isKeyPressed(_jumpK) && _jumping) {
			flatVelocities.push_back({ 0, -300 });
		}

		_dir.normalize();
		if (tryingToShoot) _dir.x *= 0.2f;
		if (InputsManager::isKeyPressed(_slowK))	_dir.x *= _slowSpeed;
		else										_dir.x *= _speed;

		flatForces.push_back({ 0, G });
		flatVelocities.push_back(_dir);
	}

	const auto& projectileBuffer = _weapon->getProjectileBuffer();
	_projectilesToShoot.insert(
		_projectilesToShoot.end(),
		projectileBuffer.begin(),
		projectileBuffer.end()
	);
	_weapon->clearProjectileBuffer();
}

void Player::render(sf::RenderTarget &target) {
	_sprite.getSprite().setPosition(pos);
	_sprite.render(target);

}
void Player::shoot() {
	_sprite.pushAnim("action");
}

void Player::hit(unsigned int d) {
	_invincible = true;

	if (_hitSound.getStatus() == sf::Sound::Stopped)
		_hitSound.play();

	_life -= d;
	_sprite.getSprite().setColor(sf::Color(230, 230, 230));
	TimerManager::addFunction(0.33f, "blinkDown", [&, n = 0](double) mutable -> bool {
		_sprite.getSprite().setColor((n++ % 2 == 0) ? sf::Color::White : sf::Color(230, 230, 230));
		if (n >= 3) {
			_invincible = false;
			return true;
		}
		return false;
	});
}

void Player::startCaC() {
	TimerManager::restartFunction(_keyCdActionCaC);
	_sprite.getSprite().setColor(sf::Color::Blue);
	_freeze = true;
}

void Player::swapWeapon(std::shared_ptr<Weapon> weapon) {
	if (_weapon)
		_weapon->unEquip();
	
	_weapon = weapon;
	_weapon->equip();
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
	if (auto ptr = static_cast<Projectile*>(obj); ptr->idMask.test((size_t)Object::BIT_TAGS::PROJECTILE) && !ptr->isFromPlayer()) {
		if (!_invincible) {
			hit(ptr->getDamage());
		}
		ptr->remove();
	}
}

void Player::dropWeapon() {

	TimerManager::addFunction(0.5f, "ActivateLoot", [&w = _weapon](double)mutable->bool {
		return true;
	});
}

std::shared_ptr<Weapon> Player::getWeapon() const {
	return _weapon;
}

void Player::floored() {
	static u32 i = 0u;
	if (++i > 1000u) {
		printf("floored\n");
		i = 0u;
	}

	_nJumpsLeft = 2u;
	_jumping = false;
	_floored = true;
}
bool Player::isFloored() const {
	return _floored;
}

void Player::unEquip() {
	_weapon->unEquip();
}

void Player::jumpKeyPressed() {
	if (_nJumpsLeft > 0u) {
		if (_floored)
			_floored = false;

		_jumping = true;

		velocity.y = -sqrtf(G * 2.f * _jumpHeight); // derived from equation of motion, thanks Newton
		_nJumpsLeft--;
	}
}

void Player::keyPressed(sf::Keyboard::Key key) {
	if (key == _jumpK) {
		jumpKeyPressed();
	}

	if (key == _upK) {
		_dir.y = -1;
	}
	else if (key == _downK) {
		_dir.y = 1;
	}

	if (key == _leftK) {
		_dir.x = -1;
	}
	else if (key == _rightK) {
		_dir.x = 1;
	}
}

void Player::keyReleased(sf::Keyboard::Key key) {
	if (key == _upK) {
		_dir.y = std::max(_dir.y, 0.f);
	}
	else if (key == _downK) {
		_dir.y = std::min(_dir.y, 0.f);
	}

	if (key == _leftK) {
		_dir.x = std::max(_dir.x, 0.f);
	}
	else if (key == _rightK) {
		_dir.x = std::min(_dir.x, 0.f);
	}
}

void Player::setFocus(bool focus) {
	_focus = focus;
}
bool Player::getFocus() const {
	return _focus;
}