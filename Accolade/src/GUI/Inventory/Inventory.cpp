#include "Inventory.hpp"

#include "Common.hpp"

#include "Graphics/GUI/Panel.hpp"
#include "Graphics/GUI/Label.hpp"
#include "Managers/InputsManager.hpp"

constexpr struct {
	float icon = 5.f;
	float name = 50.f;
	float cost = 300.f;
} MARGINS;

std::unique_ptr<Widget> construct_item_line(Eid<Item> item_ptr, float width) noexcept {
	auto widget = std::make_unique<Widget>(nlohmann::json{{"size", {width, 30.f} } });
	auto item = es_instance->get(item_ptr);
	assert(item);

	auto icon_panel = widget->makeChild<Panel>({
		{"name", "icon_panel"},
		{"pos", {MARGINS.icon, 5}},
		{"size", {20, 20}}
	});
	icon_panel->setTexture(item->getIconTexture());

	widget->makeChild<Label>({
		{"name", "label_name"},
		{"pos", {MARGINS.name, 5}},
		{"text", item->getName().c_str()},
		{"font", "consola"},
		{"charSize", 15}
	});

	widget->makeChild<Label>({
		{"name", "label_cost"},
		{"origin", {1, 0}},
		{"pos", {MARGINS.cost, 5}},
		{"text", std::to_string(item->getCost()).c_str()},
		{"font", "consola"},
		{"charSize", 15}
	});

	return widget;
}

Inventory::Inventory() noexcept : Widget() {
	main_panel = makeChild<Panel>({
		{"pos", {C::WIDTH / 6.f, 0} },
		{"resizable", true },
		{"collapsable", false },
		{"draggable", false },
		{"sprite", "panel_a"},
		{"size", {2/3.f * C::WIDTH, C::HEIGHT} }
	});
	main_panel->getSprite().setColor(sf::Color{ 65, 71, 76, 255 });

	item_focus_mask = makeChild<Panel>({
		{"size", {main_panel->getSize().y + 5, 35} },
		{"origin", {2.5f / main_panel->getSize().y, 2.5f / 35.f}}
	});
	item_focus_mask->getSprite().setColor(sf::Color{ 255, 255, 255, 125 });
	item_focus_mask->setVisible(false);

	Widget::Callback on_click;
	on_click.began = std::bind(&Inventory::onClickBegan, this);
	on_click.going = std::bind(&Inventory::onClickGoing, this);
	on_click.ended = std::bind(&Inventory::onClickEnded, this);
	main_panel->setOnClick(on_click);

	Widget::Callback on_key;
	on_key.began = std::bind(&Inventory::onKeyBegan, this);
	on_key.going = std::bind(&Inventory::onKeyGoing, this);
	on_key.ended = std::bind(&Inventory::onKeyEnded, this);
	main_panel->setOnKey(on_key);

	Widget::Callback on_focus;
	on_focus.began = std::bind(&Inventory::onFocusBegan, this);
	on_focus.going = std::bind(&Inventory::onFocusGoing, this);
	on_focus.ended = std::bind(&Inventory::onFocusEnded, this);
	main_panel->setOnFocus(on_focus);

	item_list = main_panel->makeChild<Widget>({});
}

bool Inventory::needToQuit() const noexcept {
	return quit;
}

void Inventory::addItem(Eid<Item> item) noexcept {
	auto widget = construct_item_line(item, main_panel->getSize().x);
	ItemLine line{ widget.release(), item };
	items.push_back(line);
	item_list->addChild(line.widget, 1);

	for (size_t i = 0; i < items.size(); ++i) {
		items[i].widget->setPosition({ items[i].widget->getPosition().x, 30.f * i });
	}
}

void Inventory::render(sf::RenderTarget& target) noexcept {
	Widget::render(target);
}

void Inventory::populateItems(std::vector<Eid<Item>> items) noexcept {
	for (auto& x : items) addItem(x);
}

void Inventory::clearItems() noexcept {
	item_list->killEveryChilds();
	items.clear();
}

bool Inventory::onClickBegan() noexcept {return false;}
bool Inventory::onClickGoing() noexcept {return false;}
bool Inventory::onClickEnded() noexcept {return false;}

bool Inventory::onKeyBegan() noexcept {return false;}
bool Inventory::onKeyGoing() noexcept {return false;}
bool Inventory::onKeyEnded() noexcept {
	if (IM::isKeyJustReleased(sf::Keyboard::Escape)) {
		quit = true;
		return true;
	}
	return false;
}

bool Inventory::onFocusBegan() noexcept {return false;}
bool Inventory::onFocusGoing() noexcept {return false;}
bool Inventory::onFocusEnded() noexcept {return false;}

void Inventory::open() noexcept {
	quit = true;
}

