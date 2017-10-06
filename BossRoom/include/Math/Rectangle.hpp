#pragma once
#include "Vector2.hpp"

class Rectangle {
public:
	union {
		struct {
			Vector2 pos;
			Vector2 size;
		};
		struct {
			float x;
			float y;
			float w;
			float h;
		};
	};
	float a;

	static const Rectangle ZERO;

	Rectangle();
	Rectangle(const Rectangle& other);
	Rectangle(float x, float y, float w, float h);
	Rectangle(const Vector2& pos, const Vector2& size, float a = 0);
	//C is redondant
	Rectangle(const Vector2& A, const Vector2& B, const Vector2& D);
	~Rectangle();

	bool isInside(const Vector2 &P);
	bool intersect(const Rectangle& rect);

	Vector2 getSize() const;
	void setSize(const Vector2& size);
	void setWidth(float w_) { setSize({w_, h}); }
	void setHeight(float h_) { setSize({w, h_}); }

	const void draw(sf::RenderTarget& target, sf::Color color);

	void print(const std::string pre = "", const std::string sui = "") const;

	bool operator==(const Rectangle &r);
	template<typename T>
	Rectangle& operator=(const sf::Rect<T>& other) {
		this->operator=({ static_cast<float>(other.left), static_cast<float>(other.top), static_cast<float>(other.width), static_cast<float>(other.height) });
		return *this;
	}
	Rectangle& operator=(const Rectangle& other);

	template<typename T>
	operator sf::Rect<T>() const {
		return sf::Rect<T>(
			static_cast<T>(x),
			static_cast<T>(y),
			static_cast<T>(w),
			static_cast<T>(h)
		);
	}
};

