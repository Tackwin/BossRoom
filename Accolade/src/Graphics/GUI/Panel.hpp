#pragma once
#include "Widget.hpp"

#include <SFML/Graphics.hpp>

class Panel : public Widget {
public:

	static constexpr auto NAME = "Panel";

public:

	Panel() = default;
	Panel(nlohmann::json json) noexcept;

	void setTexture(std::string texture);
	std::string getTexture() const noexcept;
	sf::Sprite& getSprite();

	float getSizeRatio() const noexcept;

	void computeSize();

	void render(sf::RenderTarget& target);

protected:
	std::string _texture;
	sf::Sprite _backSprite;
};