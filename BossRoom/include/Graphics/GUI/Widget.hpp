#pragma once
#include <memory>
#include <functional>

#include <SFML/Graphics.hpp>

#include "../../include/Math/Vector2.hpp"


struct Callback {
	using type = std::function<void(void)>;

	type began;
	type ended;
	type going;
};

class Widget {
public:
	Widget();
	Widget(Widget* const parent);
	~Widget();

	virtual Vector2 getSize() const;
	const Vector2& getOrigin() const;
	const Vector2& getPosition() const;

	const Vector2& setPosition(const Vector2& pos);
	const Vector2& setOrigin(const Vector2& origin);
	const Vector2& setOriginAbs(const Vector2& origin);

	void addChild(Widget* const child);
	bool haveChild(const Widget* const child);
	void setParent(Widget* const parent);
	Widget* const getParent();

	virtual void render(sf::RenderTarget& target);

protected:

	Vector2 _pos;
	Vector2 _origin;

	Widget* _parent; //like, why do i even bother raw pointer mean, I DON'T HAVE THE OWNERSHIP, it settles it
	std::vector<Widget*> _childs;

	Callback _onHover;
	Callback _onClick;
	Callback _onKey;
};
