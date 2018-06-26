#pragma once
#include <memory>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "./../../3rd/json.hpp"

#include "./../../Common.hpp"

#include "./../../Math/Vector.hpp"

#include "./../../Physics/Object.hpp"

#include "./../../Graphics/AnimatedSprite.hpp"

#include "./../../Managers/MemoryManager.hpp"

#include "./../Weapon.hpp"
#include "PlayerInfo.hpp"

class Zone;
class Game;
class Level;
class Projectile;
class Player : public Object {
public:
	Player();

	void enterLevel(Level* level);
	void exitLevel();

	void update(double dt);
	void render(sf::RenderTarget& target);

	void shoot() noexcept;
	void hit(float damage);
	void startCaC();

	bool isAlive() const;
	float getLife() const;

	UUID getWeapon() const noexcept;
	void swapWeapon(UUID weapon);
	bool isEquiped() const noexcept;

	template<class... Args>
	void addProjectile(Args... args) {
		(addProjectile(args), ...);
	}
	void addProjectile(const std::shared_ptr<Projectile>& projectile);

	Vector2f getPos();
	void setPos(const Vector2f& v);

	const std::vector<std::shared_ptr<Projectile>>&
		getProtectilesToShoot() const;

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

	void applyVelocity(Vector2f v);

	PlayerInfo getPlayerInfo() const;
	PlayerInfo& getPlayerInfo();

	void knockBack(Vector2f recoil) noexcept;

	Vector2f getFacingDir() const noexcept;
	void setFacingDir(float angle) noexcept;

	void keyPressed(sf::Keyboard::Key key);
	void keyReleased(sf::Keyboard::Key key);
public: //TODO: Make private

	void jumpKeyPressed();

	u32 _nJumpsLeft{ 2 };

	float _life;
	Vector2f _dir;
	sf::Color _color;

	PlayerInfo _info;

	Weapon _weapon;

	Disk* _hitBox;

	std::string _projectilesKey;

	nlohmann::basic_json<> _json;

	std::string _keyCdAction;
	std::string _keyCdActionCaC;

	bool _floored{ false };
	bool _invincible{ false };
	bool _freeze{ false };

	bool _jumping{ false };
	bool _boostingJump{ false };

	bool _focus{ true };

	float _facingDir{ 0.f };

	Level* _level{ nullptr };

	AnimatedSprite _sprite;
	sf::Sound _hitSound;

	std::vector<std::shared_ptr<Projectile>> _projectilesToShoot;
	std::vector<std::shared_ptr<Zone>> _zonesToApply;
};