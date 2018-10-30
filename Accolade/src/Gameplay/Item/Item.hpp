#pragma once

/*

*/
#include <SFML/Graphics.hpp>


#include "Components/Removable.hpp"
#include "Components/Clonable.hpp"

class Item : public Removable, public Clonable {
public:
	enum class Type : size_t {
		Main_Weapon = 0,
		Generic,
		Count
	};

	virtual ~Item() noexcept = default;

	virtual void mount() noexcept = 0;
	virtual void unMount() noexcept = 0;

	virtual void update(double dt) noexcept = 0;
	virtual void render(sf::RenderTarget& target) noexcept = 0;

	virtual void remove() noexcept override = 0;
	virtual bool toRemove() const noexcept override = 0;

	virtual sf::Texture& getIconTexture() const noexcept = 0;
	virtual std::string getName() const noexcept;
	virtual size_t getCost() const noexcept;

	virtual Type getItemType() const noexcept;
protected:

	std::string name;
	size_t cost{ 0 };
};