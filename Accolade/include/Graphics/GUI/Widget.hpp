#pragma once
#include <memory>
#include <functional>

#include <SFML/Graphics.hpp>

#include "Math/Vector.hpp"



class Widget {
public:
	struct Callback {
		using type = std::function<void(void)>;

		static const type ZERO;

		type began;
		type ended;
		type going;
	};

	Widget();
	Widget(const Widget& copy) = delete;
	Widget(Widget* const parent);
	~Widget();

	Vector2f getSize() const;
	const Vector2f& getOrigin() const;
	Vector2f getGlobalPosition() const;
	const Vector2f& getPosition() const;
	bool isVisible() const;

	void setOnHover(const Callback& onHover);
	void setOnClick(const Callback& onClick);
	void setOnKey(const Callback& onKey);

	void setSize(const Vector2f& size);
	void setPosition(const Vector2f& pos);
	void setOrigin(const Vector2f& origin);
	void setOriginAbs(const Vector2f& origin);
	void setVisible(bool visible);

	void addChild(Widget* const child, int32_t z = 0);
	bool haveChild(const Widget* const child);
	void setParent(Widget* const parent, int32_t z = 0);
	Widget* const getParent();
	const std::vector<std::pair<int32_t, Widget*>> getChilds();

	virtual void render(sf::RenderTarget& target);
	void propagateRender(sf::RenderTarget& target);

	void input();
	void propagateInput();

protected: //god this is growing into a god class... :(

	Vector2f _pos;
	Vector2f _size;
	Vector2f _origin;

	bool _visible = true;
	bool _passThrough = false;

	Widget* _parent = nullptr; //like, why do i even bother raw pointer mean, I DON'T HAVE THE OWNERSHIP, it settles it
	std::vector<std::pair<int32_t, Widget*>> _childs;

	Callback _onHover;
	Callback _onClick;
	Callback _onKey;
};
