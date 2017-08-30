#pragma once
#include <SFML/Graphics.hpp>

#include <string>
#include <vector>

#include "Math/Rectangle.hpp"

class AnimatedSprite{
public:
	AnimatedSprite();
	AnimatedSprite(const AnimatedSprite& sprite);
	AnimatedSprite(std::vector<uint32_t> nFrames_, Vector2 frameRectangle_ = { 0, 0 });
	AnimatedSprite(const sf::Texture& texture_, std::vector<uint32_t> nFrames_, Vector2 frameRectangle_ = { 0, 0 });

	void startAnim(uint32_t i_, float speed_);

	void render(sf::RenderTarget& target);

	sf::Sprite& getSprite();
	const Vector2& getSize();

	AnimatedSprite& operator=(const AnimatedSprite& other);
private:
	sf::Sprite _sprite;

	Rectangle _frameRectangle;

	std::string _keyAnim;
	std::vector<uint32_t> _nFrames;
};