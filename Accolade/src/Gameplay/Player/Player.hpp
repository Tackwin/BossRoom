#pragma once
#include <memory>
#include <vector>
#include <unordered_set>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "./../../3rd/json.hpp"

#include "./../../Common.hpp"

#include "./../../Math/Vector.hpp"

#include "./../../Physics/Object.hpp"

#include "./../../Graphics/AnimatedSprite.hpp"

#include "./../../Managers/MemoryManager.hpp"

#include "./../Wearable/Wearable.hpp"
#include "PlayerInfo.hpp"

class Zone;
class Game;
class Level;
class Projectile;
class Player : public Object, public std::enable_shared_from_this<Player> {
public:
	Player() noexcept;
	Player(PlayerInfo info) noexcept;

	void enterLevel(Level* level);
	void exitLevel();

	void update(double dt);
	void render(sf::RenderTarget& target);

	void shoot() noexcept;
	void hit(float damage);

	bool isAlive() const;
	float getLife() const;

	std::optional<std::string> getWeapon() const noexcept;
	void swapWeapon(std::string weapon);
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

	void unmount();

	void applyVelocity(Vector2f v);

	PlayerInfo getPlayerInfo() const;
	PlayerInfo& getPlayerInfo();

	void knockBack(Vector2f recoil) noexcept;

	Vector2f getFacingDir() const noexcept;
	void setFacingDir(float angle) noexcept;

	void keyPressed(sf::Keyboard::Key key);
	void keyReleased(sf::Keyboard::Key key);

	Vector2f support(float a, float d) const noexcept;

	bool eventFired(const std::string& name) const noexcept;
public: //TODO: Make private

	void jumpKeyPressed();

	u32 _nJumpsLeft{ 2 };

	float _life;
	Vector2f _dir;
	sf::Color _color;

	PlayerInfo _info;

	Wearable _weapon;

	Disk* _hitBox;

	nlohmann::basic_json<> _json;

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

	std::unordered_set<std::string> _events;

	std::vector<std::shared_ptr<Projectile>> _projectilesToShoot;
	std::vector<std::shared_ptr<Zone>> _zonesToApply;
};