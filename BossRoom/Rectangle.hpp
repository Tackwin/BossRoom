#pragma once
#include "Vector2.hpp"

class Rectangle {
public:
	Vector2 A, B, D;

	Rectangle();
	Rectangle(const Vector2& pos, const Vector2& size, float a = 0);
	//C is redondant
	Rectangle(const Vector2& A, const Vector2& B, const Vector2& D);
	~Rectangle();

	bool isInside(const Vector2 &P);

	Vector2 getSize() const;
	float getWidth() const;
	float getHeight() const;
	void setSize(const Vector2& size);
	void setWidth(float w) { setSize({w, getHeight()}); }
	void setHeight(float h) { setSize({getWidth(), h}); }

	const void draw(sf::RenderTarget& target, sf::Color color);

	bool operator==(const Rectangle &r);
};

