#pragma once
#include <map>
#include <memory>
#include <SFML/Graphics.hpp>

#include "../../include/Math/Vector2.hpp"

class Panel {
public:

	Panel(const std::string& textureKey, const std::string& fontKey = "consola");

	void render(sf::RenderTarget& target);
	
	void addLabel(const std::string& key);
	sf::Text& getLabel(const std::string& key);

	Vector2 getSize();

	std::map<std::string, sf::Text>& getLabels();
private:
	float getLabelsHeight();

	Vector2 _padding = Vector2::ZERO;

	sf::Sprite _backgroundSprite;

	std::shared_ptr<const sf::Font> _font;
	std::map<std::string, sf::Text> _labels;
};