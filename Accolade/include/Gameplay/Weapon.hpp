#pragma once
#include <vector>
#include <memory>
#include <functional>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "3rd/json.hpp"

#include "Const.hpp"

#include "Math/Vector.hpp"

#include "Physics/Object.hpp"
#include "Gameplay/Zone.hpp"

class Player;
class Projectile;
class Weapon : public Object {
public:
	Weapon();
	Weapon(const Weapon& other);
	Weapon(std::shared_ptr<Player> player, nlohmann::json json);
	~Weapon();

	void equip();
	void unEquip();

	void active(u32 id);
	void passive(u32 id);

	void update(float dt);

	void render(sf::RenderTarget& target);
	void renderGui(sf::RenderTarget& target);

	void setName(const std::string& name);
	std::string getName() const;
	
	void setCost(const u32& cost);
	u32 getCost() const;

	void addProjectile(const std::shared_ptr<Projectile>& projectile);
	const std::vector<std::shared_ptr<Projectile>>& getProjectileBuffer() const;
	void clearProjectileBuffer();

	const sf::Sprite& getUiSprite() const;
	std::string getStringSoundActive(u32 n) const;

	Weapon& operator=(const Weapon& other);

	static std::vector<std::shared_ptr<Weapon>> _weapons;
	static void createWeapons(std::shared_ptr<Player> player);
	static void destroyWeapons();
private: //TODO: Make this private
	void swap(Weapon& other);

	std::string _name;
	u32 _cost;

	std::shared_ptr<Player> _player;

	sf::Sprite _uiSprite;

	float _radius;

	std::vector<std::string> _keys;
	u64 _flags = 0;

	nlohmann::json _json;

	std::vector<sf::Sound> _activeSounds;
	std::vector<std::shared_ptr<Projectile>> _projectileBuffer;

	std::function<void(Weapon&)> _equip = [](Weapon&) {};
	std::function<void(Weapon&)> _unEquip = [](Weapon&) {};

	std::function<void(Weapon&, u32)> _active = [](Weapon&, u32) {};
	std::function<void(Weapon&, u32)> _passive = [](Weapon&, u32) {};

	std::function<void(Weapon&, float)> _update = [](Weapon&, float) {};
};

