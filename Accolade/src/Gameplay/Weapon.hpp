#pragma once
#include <array>
#include <vector>
#include <bitset>
#include <memory>
#include <functional>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "./../3rd/json.hpp"

#include "./../Common.hpp"
#include "./../Utils/UUID.hpp"

#include "./../Math/Vector.hpp"

class Player;
class Projectile;
class Weapon {
public:
	Weapon();
	Weapon(const Weapon& other);
	Weapon(nlohmann::json json);

	void equip(Player& player);
	void unEquip(Player& player);

	void active(Player& player, u32 id);
	void passive(Player& player, u32 id);

	void update(Player& player, float dt);

	void render(sf::RenderTarget& target);
	void renderGui(sf::RenderTarget& target);

	void setName(const std::string& name);
	std::string getName() const;
	
	void setCost(const u32& cost);
	u32 getCost() const;

	void addProjectile(const std::shared_ptr<Projectile>& projectile);
	const 
		std::vector<std::shared_ptr<Projectile>>& getProjectileBuffer() const;
	void clearProjectileBuffer();

	const sf::Sprite& getUiSprite() const;
	void setUiSpriteSize(const Vector2f& size);
	void setUiSpritePos(const Vector2f& pos);
	std::string getStringSoundActive(u32 n) const;

	const nlohmann::json& getJson() const;

	Weapon& operator=(const Weapon& other);

	const UUID& getUUID() const noexcept;

	static std::map<UUID, Weapon> _weapons;
	static void createWeapons();
private: 
	void swap(Weapon& other);

	UUID _uuid;

	std::string _name;
	u32 _cost;

	sf::Sprite _uiSprite;

	float _radius;

	std::vector<std::string> _keys;
	std::bitset<32u> _flags = 0;

	nlohmann::json _json;

	std::vector<sf::Sound> _activeSounds;
	std::vector<std::shared_ptr<Projectile>> _projectileBuffer;

	std::function<void(Weapon&, Player&)> _equip = [](Weapon&, Player&) {};
	std::function<void(Weapon&, Player&)> _unEquip = [](Weapon&, Player&) {};

	std::function<void(Weapon&, Player&, u32)> _active = [](Weapon&, Player&, u32) {};
	std::function<void(Weapon&, Player&, u32)> _passive = [](Weapon&, Player&, u32) {};

	std::function<void(Weapon&, Player&, float)> _update = [](Weapon&, Player&, float) {};
};

