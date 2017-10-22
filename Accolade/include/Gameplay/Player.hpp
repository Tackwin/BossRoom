#pragma once
#include <memory>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "3rd/json.hpp"

#include "Const.hpp"

#include "Math/Vector.hpp"

#include "Physics/Object.hpp"

#include "Graphics/AnimatedSprite.hpp"
#include "Gameplay/Weapon.hpp"

class Level;
class Projectile;
class Player : public Object {
public:
	Player() {};
	Player(const nlohmann::json& json);
	~Player();

	void initializeJson();

	void enterLevel(Level* level);
	void exitLevel();

	void update(double dt);
	void render(sf::RenderTarget& target);

	void shoot();
	void hit(unsigned int damage);
	void startCaC();

	Vector2f getDirToFire();

	const Weapon* getWeapon() const;
	void swapWeapon(Weapon* weapon);
	void swapWeapon(std::shared_ptr<Weapon> weapon);
	
	void addProjectile(const std::shared_ptr<Projectile>& projectile);

	Vector2f getPos();
	void setPos(const Vector2f& v);

	const std::vector<std::shared_ptr<Projectile>>& getProtectilesToShoot() const;
	void clearProtectilesToShoot();

	bool isInvicible() const;

	void collision(Object* obj); 

	void dropWeapon();

	void floored();
	bool isFloored() const;

	void toggleFocus();

	void unEquip();
public: //TODO: Make private

	void keyPressed(sf::Keyboard::Key key);
	void keyReleased(sf::Keyboard::Key key);
	void jumpKeyPressed();

	uint32_t _nJumpsLeft = 2;

	int32_t _AK;
	int32_t _upK;
	int32_t _leftK;
	int32_t _downK;
	int32_t _rightK;
	int32_t _slowK;
	int32_t _dashK;
	int32_t _jumpK;

	int32_t _life;
	int32_t _maxLife;
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
	Weapon _weapon;

	AnimatedSprite _sprite;
	sf::Sound _hitSound;

	std::vector<std::shared_ptr<Projectile>> _projectilesToShoot;
};