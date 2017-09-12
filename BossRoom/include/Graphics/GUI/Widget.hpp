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

	Vector2 getSize() const;
	const Vector2& getOrigin() const;
	Vector2 getGlobalPosition() const;
	const Vector2& getPosition() const;
	bool isVisible() const;

	void setSize(const Vector2& size);
	void setPosition(const Vector2& pos);
	void setOrigin(const Vector2& origin);
	void setOriginAbs(const Vector2& origin);
	void setVisible(bool visible);

	void addChild(Widget* const child);
	bool haveChild(const Widget* const child);
	void setParent(Widget* const parent);
	Widget* const getParent();
	const std::vector<Widget*> getChilds();

	virtual void render(sf::RenderTarget& target);
	void propagateRender(sf::RenderTarget& target);

	void input();
	void propagateInput();

protected: //god this is growing into a god class... :(

	Vector2 _pos;
	Vector2 _size;
	Vector2 _origin;

	bool _visible = true;
	bool _passThrough = false;

	Widget* _parent = nullptr; //like, why do i even bother raw pointer mean, I DON'T HAVE THE OWNERSHIP, it settles it
	std::vector<Widget*> _childs;

	Callback _onHover;
	Callback _onClick;
	Callback _onKey;
};