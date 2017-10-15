#include "Gameplay/Player.hpp"

#include "Game.hpp"
#include "Const.hpp"

#include "Managers/InputsManager.hpp"
#include "Managers/AssetsManager.hpp"
#include "Managers/TimerManager.hpp"

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
	idMask |= Object::BIT_TAGS::PLAYER;
	collisionMask |= Object::BIT_TAGS::FLOOR;
}

Player::~Player() {
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
	bool tryingToShoot = game->_distance && InputsManager::isMousePressed(_AK);
	if (tryingToShoot) {
		_weapon->active(0);
	}

	if (!_freeze) {
		_dir = { 0, 0 };
		if (InputsManager::isKeyPressed(_upK)) {
			_dir.y = -1;
		}
		else if (InputsManager::isKeyPressed(_downK)) {
			_dir.y = 1;
		}
		if (InputsManager::isKeyPressed(_leftK)) {
			_dir.x = -1;
		}
		else if (InputsManager::isKeyPressed(_rightK)) {
			_dir.x = 1;
		}

		if (InputsManager::isKeyJustPressed(_jumpK) && _nJumpsLeft > 0u) {
			if (_floored)
				_floored = false;

			_jumping = true;

			velocity.y = -sqrtf(G * 2.f * _jumpHeight); // derived from equation of motion, thanks Newton
			_nJumpsLeft--;
		}
		else if (InputsManager::isKeyPressed(_jumpK) && _jumping) {
			flatVelocities.push_back({ 0, -300 });
		}
		else if (_jumping && !InputsManager::isKeyPressed(_jumpK)) {
			_jumping = false;
		}
		_dir.normalize();
	
		if (tryingToShoot) _dir *= 0.2f;

		flatForces.push_back({ 0, G });
		flatVelocities.push_back(_dir * (float)(InputsManager::isKeyPressed(_slowK) ? _slowSpeed : _speed));
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
	if(_weapon) _weapon->unEquip();
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
	if (auto ptr = dynamic_cast<Projectile*>(obj); ptr && !ptr->isFromPlayer()) {
		if (!_invincible) {
			hit(ptr->getDamage());
		}
		ptr->remove();
	}
}

void Player::dropWeapon() {
	_weapon->loot(getPos());
	_weapon->setLootable(false);

	TimerManager::addFunction(0.5f, "ActivateLoot", [w = _weapon](double)mutable->bool {
		if (w) {
			w->setLootable(true);
		}

		return true;
	});
}

std::shared_ptr<Weapon> Player::getWeapon() const {
	return _weapon;
}

void Player::floored() {
	printf("Floored\n");
	_nJumpsLeft = 2u;
	_jumping = false;
	_floored = true;
}
bool Player::isFloored() const {
	return _floored;
}