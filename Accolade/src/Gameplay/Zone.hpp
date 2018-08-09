#pragma once
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>

#include <SFML/Graphics.hpp>

#include "./../Physics/Object.hpp"
#include "Components/Removable.hpp"

class Projectile;
class Zone : public Object, public Removable {
public:
	static std::shared_ptr<Zone> buildExplosion(
		const std::shared_ptr<Projectile>& p
	);

	using Callback = std::function<void(Object*)>;

	Callback entered = [](Object*) {};
	Callback exited = [](Object*) {};
	Callback inside = [](Object*) {};

	Zone(float r = 0.f);

	void render(sf::RenderTarget& target);

	void addSprite(const std::string& key, const sf::Sprite& sprite);
	void removeSprite(const std::string& key);
	sf::Sprite& getSprite(const std::string& key);
	void cleanSprites();

	void setRadius(float r);
	float getRadius() const;

	Zone& operator=(const Zone& other);

	virtual void remove() noexcept override;
	virtual bool toRemove() const noexcept override;

protected:
	std::vector<Object*> _objectsColliding;

	Disk* _disk;

	std::unordered_map<std::string, sf::Sprite> _sprites;

	float _radius = 0.f;
	bool _remove{ false };

	void collision(Object* obj);
};