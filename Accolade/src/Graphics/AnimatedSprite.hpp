#pragma once
#include <SFML/Graphics.hpp>

#include <string>
#include <vector>
#include <stack>
#include <map>

#include "./../Common.hpp"

#include "./../3rd/json.hpp"

#include "./../Math/Vector.hpp"

#include "./../Utils/UUID.hpp"

class AnimatedSprite{
public:
	AnimatedSprite() noexcept;
	AnimatedSprite(const std::string& json) noexcept;
	~AnimatedSprite();

	AnimatedSprite(AnimatedSprite&) = delete;
	AnimatedSprite& operator=(AnimatedSprite&) = delete;

	AnimatedSprite(AnimatedSprite&& other) noexcept;
	AnimatedSprite& operator=(AnimatedSprite&& other) noexcept;

	void pushAnim(const std::string& key, u32 offset = 0) noexcept;
	void popAnim();

	void render(sf::RenderTarget& target);

	sf::Sprite& getSprite();
	const Vector2f getSize();
private:
	struct Animation {
		UUID keyCallback{ UUID::zero() };
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