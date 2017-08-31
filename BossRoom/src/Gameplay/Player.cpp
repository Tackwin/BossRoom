#include "Gameplay/Player.hpp"

#include "Global/Const.hpp"
#include "Managers/InputsManager.hpp"
#include "Managers/AssetsManager.hpp"
#include "Managers/TimerManager.hpp"
#include "Gameplay/Weapon.hpp"
#include "Gameplay/Level.hpp"
#include "Gameplay/Game.hpp"

//vraiment ? 8 PUTAIN DE CARACTERES pour un PUTAIN de namespace, 'tain......
using namespace nlohmann;

Player::Player(const nlohmann::json& json) :
	_json(json),
	_hitSound(AssetsManager::getSound("hit")){
}

Player::~Player() {
}

void Player::enterLevel(Level* level) {
	_life = _json["life"];
	_maxLife = _life;
	_speed = _json["speed"];
	_radius = _json["radius"];
	_dashRange = _json["dash"];
	_pos.x = _json["startpos"]["x"];
	_pos.y = _json["startpos"]["y"];
	_slowSpeed = _json["specialSpeed"];

	_AK = _json["actionButton"];
	_upK = _json["upKey"];
	_leftK = _json["leftKey"];
	_downK = _json["downKey"];
	_rightK = _json["rightKey"];
	_slowK = _json["slowKey"];
	_dashK = _json["dashKey"];

	_invincibilityTime = _json["invincibilityTime"].get<float>();
	
	_sprite = AnimatedSprite(_json["sprite"]);
	_sprite.pushAnim("idle");

	_sprite.getSprite().setOrigin(_sprite.getSprite().getGlobalBounds().width / 2.f, _sprite.getSprite().getGlobalBounds().height / 2.f);
	_sprite.getSprite().setPosition(_pos);

	_weapon->equip();

 	_level = level;
}
void Player::exitLevel() {
	_weapon->unEquip();
	_level = nullptr;
}

void Player::update(float dt) {
	bool tryingToShoot = game->_distance && InputsManager::isMousePressed(_AK);

	if (!_freeze) {
		_dir = Vector2::ZERO;
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
		_dir.normalize();

		if (tryingToShoot)
			_dir *= .2f;

		if (InputsManager::isKeyJustPressed(_dashK)) {
			_pos += getDirToFire() * _dashRange;
		}
		_pos += _dir * (float)(InputsManager::isKeyPressed(_slowK) ? _slowSpeed : _speed) * dt;
	}

	if (tryingToShoot) {
		_weapon->active(0);
	}
}

void Player::render(sf::RenderTarget &target) {
	_sprite.getSprite().setPosition(_pos);
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
	TimerManager::addFunction(0.33f, "blinkDown", [&, n = 0](float) mutable -> bool {
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

Vector2 Player::getDirToFire() {
	return (InputsManager::getMouseWorldPos() - _pos).normalize();
}
void Player::addProjectile(const std::shared_ptr<Projectile>& projectile) {
	_level->addProjectile(projectile);
}