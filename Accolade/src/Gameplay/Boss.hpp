#pragma once
#include <memory>
#include <functional>
#include <unordered_set>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "./../3rd/json.hpp"

#include "./../Math/Vector.hpp"

#include "./../Graphics/AnimatedSprite.hpp"
#include "./../Graphics/ParticleGenerator.hpp"

#include "./../Physics/Object.hpp"

#include "./../Common.hpp"

#include "Components/Hitable.hpp"

class Section;
class Projectile;
class Boss : public Object, public Hitable {
public:
	Boss() {};
	//TODO
	Boss(Boss&);
	Boss(const nlohmann::basic_json<>& json,
		 std::function<void(double, Boss&)> updateFunction,
		 std::function<void(Boss&)> initFunction,
		 std::function<void(Boss&)> unInitFunction);

	~Boss() noexcept;
	
	void enterSection(Section* section) noexcept;
	void leaveSection() noexcept;

	void die();
	
	void update(double dt);
	void render(sf::RenderTarget& target);

	virtual void hit(float damage) noexcept override;
	void shoot(const std::shared_ptr<Projectile>& projectile);
	void shoot(
		const nlohmann::json& json, 
		const Vector2f& pos, 
		const Vector2f& dir
	);

	Vector2f getPos() const;

	float getRadius() const;

	float getLife() const;
	float getMaxLife() const;

	Section* getSection() const;

	void addKeyTimer(UUID key);
	void delKeyTimer(UUID key);

	const std::vector<std::shared_ptr<Projectile>>& getProtectilesToShoot() const;
	void clearProtectilesToShoot();

	static std::vector<std::shared_ptr<Boss>> bosses;
	static void createBosses();
	static void destroyBosses();

private:
	nlohmann::basic_json<> _json;

	float _life;
	float _maxLife;
	float _radius;
	sf::Color _color;

	Disk* _disk;

	std::vector<std::shared_ptr<Projectile>> _projectilesToShoot;

	UUID _blinkDownKey{ UUID::zero() };

	std::vector<UUID> _keyPatterns;

	std::function<void(Boss&)> _init;
	std::function<void(double, Boss&)> _update;
	std::function<void(Boss&)> _unInit;

	AnimatedSprite _sprite;

	std::vector<sf::Sound> _sounds;

	Section* section{ nullptr };

	std::vector<std::unique_ptr<ParticleGenerator>> _particleEffects;
private:
	void collision(Object* obj);
};