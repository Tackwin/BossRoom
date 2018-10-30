#pragma once

#include <map>

#include "Gameplay/Wearable/Wearable.hpp"
#include "Gameplay/Player/Player.hpp"

#include "Graphics/GUI/Widget.hpp" 
#include "Graphics/GUI/Button.hpp"
#include "Graphics/GUI/Label.hpp"
#include "Graphics/GUI/Panel.hpp" 

#include "Entity/EntityStore.hpp"
#include "Entity/OwnId.hpp"

#include "Utils/UUID.hpp"

// >TODO it's such a mess...

class Shop : public Widget {
public:
	Shop();

	void setPlayer(std::weak_ptr<Player> player);

	void addItem(OwnId<Item>&& item) noexcept;
	void addWeapon(const std::string& weapon);

	void focusItem(UUID i);
	void unFocus();

	void enter();
	void leave();

	bool isIn() const;
private:
	static constexpr auto item_per_row = 7;

	struct _ItemPanel : public Panel {
		static constexpr float PANEL_SIZE = 60.f;

		_ItemPanel() {};
		_ItemPanel(Shop* shop, const std::string& weapon, UUID id);
		_ItemPanel(Shop* shop, OwnId<Item> item, UUID id);

		bool onClickBegan();
		bool onClickEnded();
		bool onClickGoing();

		Panel* sprite;
		Label* label;

		std::string weapon;
		OwnId<Item> item;
	private:
		Shop* _shop;
		UUID _id;
	};
	struct _InfoPanel : public Panel {
	public:
		_InfoPanel() {};
		_InfoPanel(Shop* shop);

		void populateBy(const _ItemPanel& item_panel);

		bool onClickBegan();
		bool onClickEnded();
		bool onClickGoing();

		std::map<std::string, Panel*> panels;
		std::map<std::string, Label*> labels;
		std::map<std::string, Button*> buttons;
	private:
		
		Shop* _shop;
		_ItemPanel const* item_panel{ nullptr };
	};

	bool onClickBegan();
	bool onClickEnded();
	bool onClickGoing();

	Panel* _merchantPanel{ nullptr };
	Panel* _focusPanel{ nullptr };
	Panel* _quitPanel{ nullptr };

	_InfoPanel* _infoPanel;
	std::map<UUID, _ItemPanel*> _itemPanels;

	bool _in = false;

	bool _dragging = false;
	Vector2f _dragOffset = { 0, 0 };

	UUID _focused{ UUID::zero() };

	std::weak_ptr<Player> _player;
};