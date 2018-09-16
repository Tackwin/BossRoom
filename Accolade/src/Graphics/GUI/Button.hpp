#pragma once
#include "Widget.hpp"

#include "Panel.hpp"
#include "Label.hpp"

#include "./../../Math/Vector.hpp"

class Button : public Widget {
public:
	Button();

	sf::Sprite& getSprite();
	sf::Sprite& getHoldSprite();
	Label& getLabel();

	void setStdString(const std::string& label);
	std::string getString() const;
	
	void setSize(const Vector2f& size);
	void setOrigin(const Vector2f& origin);

	void setTexture(std::string texture);
	void setHoldSprite(std::string sprite);

	void computeSize();

	virtual void render(sf::RenderTarget& target);
private:

	bool onClickBegan();
	bool onClickEnded();
	bool onClickGoing();

	Panel* _normal;
	Panel* _hold;

	Label* _label;
};