#pragma once

#include <vector>

#include <SFML/Graphics.hpp>

#include "Graphics/GUI/Widget.hpp"
#include "Entity/EntityStore.hpp"
#include "Entity/Eid.hpp"
#include "Gameplay/Item/Item.hpp"
#include "Math/Vector.hpp"

class Panel;
class Inventory : private Widget {
public:

	Inventory() noexcept;

	virtual void render(sf::RenderTarget& target) noexcept override;

	void populateItems(std::vector<Eid<Item>> items) noexcept;
	void clearItems() noexcept;
	void addItem(Eid<Item> item) noexcept;

	bool needToQuit() const noexcept;
	void open() noexcept;
	void quit() noexcept;

	using Widget::propagateRender;
	using Widget::postOrderInput;

	virtual void setFocus(bool v) noexcept override;

private:

	bool onClickBegan() noexcept;
	bool onClickGoing() noexcept;
	bool onClickEnded() noexcept;

	bool onKeyBegan() noexcept;
	bool onKeyGoing() noexcept;
	bool onKeyEnded() noexcept;

	bool onFocusBegan() noexcept;
	bool onFocusGoing() noexcept;
	bool onFocusEnded() noexcept;

	void setFocusedItem(size_t index) noexcept;

	struct ItemLine {
		Widget* widget{ nullptr };
		Eid<Item> item;
	};

	std::vector<ItemLine> items;
	std::optional<size_t> focused_item;

	Widget* item_list{ nullptr };
	Panel* item_focus_mask{ nullptr };
	Panel* main_panel{ nullptr };

	bool need_to_quit{ false };
};

