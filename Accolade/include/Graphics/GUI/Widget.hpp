#pragma once
#include <array>
#include <memory>
#include <functional>

#include <SFML/Graphics.hpp>

#include "Const.hpp"

#include "Math/Vector.hpp"
#include "Math/Rectangle.hpp"

class Widget {
public:
	struct Callback {
		using type = std::function<bool(void)>;

		static const type ZERO;

		type began = ZERO;
		type ended = ZERO;
		type going = ZERO;
	};

	Widget();
	Widget(const Widget& copy) = delete;
	Widget(Widget* const parent);
	~Widget();

	Vector2f getSize() const;
	const Vector2f& getOrigin() const;
	Vector2f getGlobalPosition() const;
	const Vector2f& getPosition() const;
	Rectangle2f getGlobalBoundingBox() const;
	bool isVisible() const;

	void setOnHover(const Callback& onHover);
	void setOnClick(const Callback& onClick);
	void setOnKey(const Callback& onKey);

	void setSize(const Vector2f& size);
	void setPosition(const Vector2f& pos);
	void setOrigin(const Vector2f& origin);
	void setOriginAbs(const Vector2f& origin);
	void setVisible(bool visible);

	void emancipate();
	void denyChild(Widget* const child);
	void addChild(Widget* const child, i32 z = 0);
	bool haveChild(const Widget* const child);
	void setParent(Widget* const parent, i32 z = 0);
	Widget* const getParent();
	const std::vector<std::pair<i32, Widget*>> getChilds();

	virtual void render(sf::RenderTarget& target);
	void propagateRender(sf::RenderTarget& target);

	std::array<bool, 9> input(const std::array<bool, 9>& mask);
	void propagateInput();
	std::array<bool, 9> postOrderInput(std::array<bool, 9> mask);

protected: //god this is growing into a god class... :(

	Vector2f _pos;
	Vector2f _size;
	Vector2f _origin;

	bool _visible = true;
	bool _passThrough = false;

	Widget* _parent = nullptr; //like, why do i even bother raw pointer mean, I DON'T HAVE THE OWNERSHIP, it settles it
	std::vector<std::pair<i32, Widget*>> _childs;

	Callback _onHover;
	Callback _onClick;
	Callback _onKey;
};
