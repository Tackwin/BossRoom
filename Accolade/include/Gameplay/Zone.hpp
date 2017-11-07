#pragma once
#include <string>
#include <functional>
#include <unordered_map>

#include <SFML/Graphics.hpp>

#include "Physics/Object.hpp"

class Zone : public Object {
public:
	using Callback = std::function<void(Object*)>;

	Callback entered = [](Object*) {};
	Callback exited = [](Object*) {};
	Callback inside = [](Object*) {};


	Zone(float r = 0.f);
	Zone(const Zone& other);

	void render(sf::RenderTarget& target);

	void addSprite(const std::string& key, const sf::Sprite& sprite);
	void removeSprite(const std::string& key);
	sf::Sprite& getSprite(const std::string& key);
	void cleanSprites();

	void setRadius(float r);
	float getRadius() const;

	void setRemove(bool remove = true);
	bool toRemove() const;

	Zone& operator=(const Zone& other);
protected:
	std::vector<Object*> _objectsColliding;

	std::shared_ptr<Disk> _disk;

	std::unordered_map<std::string, sf::Sprite> _sprites;

	bool _toRemove = false;

	float _radius = 0.f;

	void collision(Object* obj);
};