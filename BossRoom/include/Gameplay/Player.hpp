#pragma once
#include <memory>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "3rd/json.hpp"

#include "Graphics/AnimatedSprite.hpp"
#include <Physics/Object.hpp>
#include "Global/Const.hpp"
#include "Math/Vector2.hpp"

class Level;
class Weapon;
class Projectile;
class Player : public Object {
public:
	Player() {};
	Player(const nlohmann::json& json);
	~Player();

	void initializeJson();

	void enterLevel(Level* level);
	void exitLevel();

	void update(float dt);
	void render(sf::RenderTarget& target);

	void shoot();
	void hit(unsigned int damage);
	void startCaC();

	Vector2 getDirToFire();

	void swapWeapon(std::shared_ptr<Weapon> weapon);
	void addProjectile(const std::shared_ptr<Projectile>& projectile);

	Vector2 getPos();
	void setPos(const Vector2& v);

	const std::vector<std::shared_ptr<Projectile>>& getProtectilesToShoot() const;
	void clearProtectilesToShoot();

	bool isInvicible() const;

public: //TODO: Make private

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
	Vector2 _dir;
	float _speed;
	float _radius;
	sf::Color _color;
	float _dashRange;
	float _slowSpeed;
	float _invincibilityTime;

	Box _hitBox;

	std::string _projectilesKey;

	nlohmann::basic_json<> _json;

	std::string _keyCdAction;
	std::string _keyCdActionCaC;

	bool _invincible = false;
	bool _freeze = false;

	Level* _level = nullptr;
	std::shared_ptr<Weapon> _weapon;

	AnimatedSprite _sprite;
	sf::Sound _hitSound;

	std::vector<std::shared_ptr<Projectile>> _projectilesToShoot;
};