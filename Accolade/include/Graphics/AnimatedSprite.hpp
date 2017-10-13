#pragma once
#include <SFML/Graphics.hpp>

#include <string>
#include <vector>
#include <stack>
#include <map>

#include "3rd/json.hpp"

#include "Math/Rectangle.hpp"


class AnimatedSprite{
public:
	AnimatedSprite();
	AnimatedSprite(const AnimatedSprite& sprite);
	AnimatedSprite(const std::string& json);

	void pushAnim(const std::string& key, uint32_t offset = 0);
	void popAnim();

	void render(sf::RenderTarget& target);

	sf::Sprite& getSprite();
	const Vector2 getSize();

	AnimatedSprite& operator=(const AnimatedSprite& other);
private:
	struct Animation {
		std::string keyCallback = "";
		uint32_t col = 0;
		uint32_t row = 0;
		uint32_t w = 0;
		uint32_t h = 0;
		uint32_t i = 0;
	};

	std::stack<Animation> _stackAnim;
	nlohmann::json _json;

	sf::Sprite _sprite;
};