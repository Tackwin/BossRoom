#pragma once
#include "Widget.hpp"

#include "Panel.hpp"
#include "Label.hpp"

#include "./../../Math/Vector.hpp"

class Button : public Widget {
public:
	Button();
	~Button();

	sf::Sprite& getSprite();
	sf::Sprite& getHoldSprite();
	Label& getLabel();

	void setString(const std::string& label);
	std::string getString() const;
	
	void setSize(const Vector2f& size);
	void setOrigin(const Vector2f& origin);

	void setSprite(const sf::Sprite& sprite);
	void setHoldSprite(const sf::Sprite& sprite);

	void computeSize();

	virtual void render(sf::RenderTarget& target);
private:

	bool onClickBegan();
	bool onClickEnded();
	bool onClickGoing();

	Panel _normal;
	Panel _hold;

	Label _label;
};