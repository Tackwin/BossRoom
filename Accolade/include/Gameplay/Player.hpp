#pragma once
#include <memory>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "3rd/json.hpp"

#include "Common.hpp"

#include "Math/Vector.hpp"

#include "Physics/Object.hpp"

#include "Graphics/AnimatedSprite.hpp"
#include "Gameplay/Weapon.hpp"


class Zone;
class Level;
class Projectile;
class Player : public Object {
public:
	Player(const nlohmann::json& json);

	void initializeJson();

	void enterLevel(Level* level);
	void exitLevel();

	void update(double dt);
	void render(sf::RenderTarget& target);

	void shoot();
	void hit(float damage);
	void startCaC();

	Vector2f getDirToFire();

	std::shared_ptr<Weapon> getWeapon() const;
	void swapWeapon(std::shared_ptr<Weapon> weapon);

	template<class... Args>
	void addProjectile(Args... args) {
		(addProjectile(args), ...);
	}
	void addProjectile(const std::shared_ptr<Projectile>& projectile);

	Vector2f getPos();
	void setPos(const Vector2f& v);

	const std::vector<std::shared_ptr<Projectile>>& getProtectilesToShoot() const;
	void clearProtectilesToShoot();

	template<class... Args>
	void addZone(Args... args) {
		(addZone(args), ...);
	}
	void addZone(const std::shared_ptr<Zone>& zone);
	const std::vector<std::shared_ptr<Zone>>& getZonesToApply() const;
	void clearZonesToApply();

	bool isInvicible() const;

	void collision(Object* obj); 

	void floored();
	bool isFloored() const;

	void setFocus(bool focus = true);
	bool getFocus() const;

	void unEquip();
public: //TODO: Make private

	void keyPressed(sf::Keyboard::Key key);
	void keyReleased(sf::Keyboard::Key key);
	void jumpKeyPressed();

	u32 _nJumpsLeft = 2;

	//TODO: make this a map for fuck sake
	i32 _AK;
	i32 _upK;
	i32 _leftK;
	i32 _downK;
	i32 _rightK;
	i32 _slowK;
	i32 _dashK;
	i32 _jumpK;

	float _life;
	float _maxLife;
	Vector2f _dir;
	float _speed;
	float _radius;
	sf::Color _color;
	float _dashRange;
	float _slowSpeed;
	float _invincibilityTime;
	float _jumpHeight;

	Disk _hitBox;

	std::string _projectilesKey;

	nlohmann::basic_json<> _json;

	std::string _keyCdAction;
	std::string _keyCdActionCaC;

	bool _floored = false;
	bool _invincible = false;
	bool _freeze = false;

	bool _jumping = false;
	bool _boostingJump = false;

	bool _focus = true;

	Level* _level = nullptr;
	std::shared_ptr<Weapon> _weapon;

	AnimatedSprite _sprite;
	sf::Sound _hitSound;

	std::vector<std::shared_ptr<Projectile>> _projectilesToShoot;
	std::vector<std::shared_ptr<Zone>> _zonesToApply;
};