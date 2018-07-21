#pragma once
#include <bitset>
#include <memory>
#include <functional>

#include <SFML/Graphics.hpp>

#include "Utils/UUID.hpp"
#include "./../../Common.hpp"

#include "./../../Math/Vector.hpp"
#include "./../../Math/Rectangle.hpp"

class Widget {
public:
	using ID_t = UUID;

	struct Callback {
		using type = std::function<bool(void)>;

		static const type ZERO;

		type began = ZERO;
		type ended = ZERO;
		type going = ZERO;
	};

	Widget();
	Widget(nlohmann::json json) noexcept;
	Widget(const Widget& copy) = delete;
	Widget& operator=(const Widget&) = delete;
	Widget(Widget&&) = default;
	Widget& operator=(Widget&&) = default;
	Widget(Widget* const parent);
	virtual ~Widget();

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
	const std::vector<std::pair<i32, Widget*>>& getChilds() const noexcept;
	Widget* findChild(UUID id) const noexcept;
	Widget* findChild(std::string name) const noexcept;

	virtual void render(sf::RenderTarget& target);
	void propagateRender(sf::RenderTarget& target);

	std::bitset<9u> input(const std::bitset<9u>& mask);
	void propagateInput();
	std::bitset<9u> postOrderInput(const std::bitset<9u>& mask);

	UUID getUuid() const noexcept;

	std::string getName() const noexcept;
	void setName(std::string name) noexcept;

protected: //god this is growing into a god class... :(

	Vector2f _pos;
	Vector2f _size;
	Vector2f _origin;

	ID_t _uuid;

	bool _visible = true;
	bool _passThrough = false;

	std::string _name{ "" };

	Widget* _parent = nullptr;	//like, why do i even bother raw pointer mean, 
								//I DON'T HAVE THE OWNERSHIP, it settles it
	std::vector<std::pair<i32, Widget*>> _childs;

	Callback _onHover;
	Callback _onClick;
	Callback _onKey;
};
