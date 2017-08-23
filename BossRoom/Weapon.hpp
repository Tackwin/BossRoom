#pragma once
#include <vector>
#include <memory>
#include <functional>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Vector2.hpp"
#include "Const.hpp"
#include "json.hpp"

class Player;
class Weapon {
public: //TODO: Make this private
	std::shared_ptr<Player> _player;

	Vector2 _lootedPos;
	sf::Sprite _uiSprite;
	sf::Sprite _lootedSprite;

	float _radius;

	std::vector<std::string> _keys;
	uint64 _flags = 0;

	nlohmann::json _json;

	bool _loot = false;
	bool _lootable = false;

	std::vector<sf::Sound> _activeSounds;

	std::function<void(Weapon&)> _equip;
	std::function<void(Weapon&)> _unEquip;

	std::function<void(Weapon&, uint32)> _active;
	std::function<void(Weapon&, uint32)> _passive;

	std::function<void(Weapon&, float)> _update;
public:
	Weapon(const Weapon& other);
	Weapon(std::shared_ptr<Player> player, nlohmann::json json);
	~Weapon();

	void equip();
	void unEquip();

	void active(uint32 id);
	void passive(uint32 id);

	void update(float dt);

	void render(sf::RenderTarget& target);

	void loot(Vector2 pos);

	static std::vector<std::shared_ptr<Weapon>> _weapons;
	static void createWeapons(std::shared_ptr<Player> player);
	static void destroyWeapons();
};

