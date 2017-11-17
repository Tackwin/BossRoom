#include "Gameplay/Player.hpp"

#include "Game.hpp"
#include "Common.hpp"

#include "Managers/InputsManager.hpp"
#include "Managers/AssetsManager.hpp"
#include "Managers/TimerManager.hpp"
#include "Managers/EventManager.hpp"

#include "Gameplay/Projectile.hpp"
#include "Gameplay/Weapon.hpp"

//vraiment ? 8 PUTAIN DE CARACTERES pour un PUTAIN de namespace, 'tain......

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

	EventManager::subscribe("keyPressed", 
		[&](EventManager::EventCallbackParameter args) -> void {
			auto key = std::any_cast<sf::Keyboard::Key>(*args.begin());

			keyPressed(key);
		}
	);
	EventManager::subscribe("keyReleased", 
		[&](EventManager::EventCallbackParameter args) -> void {
			auto key = std::any_cast<sf::Keyboard::Key>(*args.begin());
			keyReleased(key);
		}
	);
}

void Player::initializeJson() {
	const auto& jsonInfo = 
		AM::getJson(JSON_KEY)
		["characters"]
		[_json["character"].get<std::string>()];

	_info.maxLife = jsonInfo["life"];
	_info.speed = jsonInfo["speed"];
	_info.radius = jsonInfo["radius"];
	_info.dashRange = jsonInfo["dash"];
	_info.specialSpeed = jsonInfo["specialSpeed"];
	_info.name = jsonInfo["name"].get<std::string>();
	_info.jumpHeight = getJsonValue<float>(jsonInfo, "jumpHeight");
	_info.invincibilityTime = jsonInfo["invincibilityTime"].get<float>();
	_info.sprite = jsonInfo["sprite"].get<std::string>();

	_life = _info.maxLife;

	pos.x = _json["startpos"]["x"];
	pos.y = _json["startpos"]["y"];
	_AK = _json["actionButton"];
	_upK = _json["upKey"];
	_leftK = _json["leftKey"];
	_downK = _json["downKey"];
	_rightK = _json["rightKey"];
	_slowK = _json["slowKey"];
	_dashK = _json["dashKey"];
	_jumpK = _json["jumpKey"];
	
	_sprite = AnimatedSprite(_info.sprite);
	_sprite.pushAnim("idle");

	_sprite.getSprite().setOrigin(
		_sprite.getSprite().getGlobalBounds().width / 2.f, 
		_sprite.getSprite().getGlobalBounds().height / 2.f
	);
	_sprite.getSprite().setPosition(pos);

	_hitBox.r = _info.radius;
}


void Player::enterLevel(Level* level) {
	_weapon->equip();

 	_level = level;

	_hitBox.onEnter = [&](Object* obj) { collision(obj); };
}
void Player::exitLevel() {
	if (_weapon)
		_weapon->unEquip();
	_level = nullptr;
}

void Player::update(double dt) {
	bool tryingToShoot = 
		InputsManager::isMousePressed(_AK) && 
		_focus;

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
		if (tryingToShoot)							_dir.x *= 0.2f;
		if (InputsManager::isKeyPressed(_slowK))	_dir.x *= _info.specialSpeed;
		else										_dir.x *= _info.speed;

		flatForces.push_back({ 0, G });
		flatVelocities.push_back(_dir);
	}

	velocity *= (float)std::pow(0.2, dt);

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

void Player::hit(float d) {
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

std::shared_ptr<Weapon> Player::getWeapon() const {
	return _weapon;
}

void Player::floored() {
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

		velocity.y = -sqrtf(G * 2.f * _info.jumpHeight); // derived from equation of motion, thanks Newton
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

