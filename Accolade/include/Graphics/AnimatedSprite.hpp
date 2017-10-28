#pragma once
#include <SFML/Graphics.hpp>

#include <string>
#include <vector>
#include <stack>
#include <map>

#include "3rd/json.hpp"

#include "Math/Vector.hpp"

class AnimatedSprite{
public:
	AnimatedSprite();
	AnimatedSprite(const AnimatedSprite& sprite);
	AnimatedSprite(const std::string& json);

	void pushAnim(const std::string& key, u32 offset = 0);
	void popAnim();

	void render(sf::RenderTarget& target);

	sf::Sprite& getSprite();
	const Vector2f getSize();

	AnimatedSprite& operator=(const AnimatedSprite& other);
private:
	struct Animation {
		std::string keyCallback = "";
		u32 col = 0;
		u32 row = 0;
		u32 w = 0;
		u32 h = 0;
		u32 i = 0;
	};

	std::stack<Animation> _stackAnim;
	nlohmann::json _json;

	sf::Sprite _sprite;
};