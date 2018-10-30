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
	struct Input_Mask { enum {
		Mouse_Began,
		Mouse_Going,
		Mouse_Ended,
		Hover_Began,
		Hover_Going,
		Hover_Ended,
		Key_Began,
		Key_Going,
		Key_Ended,
		Count
	}; };

	using ID_t = UUID;

	struct Callback {
		using type = std::function<bool(void)>;

		static const type FALSE;
		static const type TRUE;

		type began = FALSE;
		type ended = FALSE;
		type going = FALSE;
	};

	Widget();
	Widget(nlohmann::json json) noexcept;
	Widget(const Widget& copy) = delete;
	Widget& operator=(const Widget&) = delete;
	Widget(Widget&&) = default;
	Widget& operator=(Widget&&) = default;
	Widget(Widget* const parent);
	virtual ~Widget();

	virtual Vector2f getSize() const;
	const Vector2f& getOrigin() const;
	Vector2f getGlobalPosition() const;
	const Vector2f& getPosition() const;
	Rectangle2f getGlobalBoundingBox() const;
	bool isVisible() const;

	void setOnFocus(const Callback& onFocus) noexcept;
	void setOnHover(const Callback& onHover);
	void setOnClick(const Callback& onClick);
	void setOnKey(const Callback& onKey);

	virtual void setSize(const Vector2f& size);
	void setPosition(const Vector2f& pos);
	void setGlobalPosition(const Vector2f& pos) noexcept;
	void setOrigin(const Vector2f& origin);
	void setOriginAbs(const Vector2f& origin);
	void setVisible(bool visible);

	void emancipate();
	// Just to provide safety over the alternative
	// (allocating ourself the widget and not forgetting to make it child)
	// You do _not_ own this, the sole reason it's not a weak_ptr is to not let you think
	// it's somehow shared by multiple person.
	template<typename T>
	std::enable_if_t<
		std::is_base_of_v<Widget, T>,
		T*
	> makeChild/*<3*/(const nlohmann::json& json, i32 z = 0) noexcept {
		auto c = new T{ json };
		addChild(c, z);
		return c;
	}
	void denyChild(Widget* const child);
	void addChild(Widget* const child, i32 z = 0);
	bool haveChild(const Widget* const child);
	bool haveChild(const std::string& name) noexcept;
	void setParent(Widget* const parent, i32 z = 0);
	Widget* const getParent();
	const std::vector<std::pair<i32, Widget*>>& getChilds() const noexcept;
	Widget* findChild(UUID id) const noexcept;
	Widget* findChild(std::string name) const noexcept;

	void killEveryChilds() noexcept;
	void killDirectChild(std::string_view name) noexcept;

	virtual void render(sf::RenderTarget& target);
	virtual void propagateRender(sf::RenderTarget& target);

	std::bitset<9u> input(const std::bitset<9u>& mask);
	std::bitset<9u> propagateInput();
	std::bitset<9u> postOrderInput(const std::bitset<9u>& mask);

	UUID getUuid() const noexcept;

	std::string getName() const noexcept;
	void setName(std::string name) noexcept;

	virtual void setFocus(bool v) noexcept;
	bool isFocus() const noexcept;

protected: //god this is growing into a god class... :(

	Vector2f _pos;
	Vector2f _size;
	Vector2f _origin;

	ID_t _uuid;

	bool _visible = true;
	bool _passThrough = false;

	bool _focused{ false };
	bool _hovered{ false };

	std::string _name{ "" };

	Widget* _parent = nullptr;	//like, why do i even bother raw pointer mean, 
								//I DON'T HAVE THE OWNERSHIP, it settles it
	std::vector<std::pair<i32, Widget*>> _childs;

	Callback _onHover;
	Callback _onClick;
	Callback _onKey;
	Callback onFocus;
};
