#pragma once

#include <map>

#include "Gameplay/Weapon.hpp"

#include "Graphics/GUI/Widget.hpp" 
#include "Graphics/GUI/Button.hpp"
#include "Graphics/GUI/Label.hpp"
#include "Graphics/GUI/Panel.hpp" 

class Shop : public Widget {
public:
	Shop();
	~Shop();

	void addWeapon(const Weapon* weapon);

	void focusItem(uint32_t i);
	void unFocus();

	void enter();
	void leave();
private:
	struct _ItemPanel : public Panel {
		static constexpr float PANEL_SIZE = 60.f;

		_ItemPanel(Shop* shop, const Weapon* weapon, uint32_t id);

		bool onClickBegan();
		bool onClickEnded();
		bool onClickGoing();

		Panel sprite;
		Label label;

		const Weapon* weapon;

	private:
		Shop* _shop;
		uint32_t _id;
	};
	struct _InfoPanel : public Panel {
	public:
		_InfoPanel(Shop* shop);

		void populateBy(const Weapon* weapon);

		bool onClickBegan();
		bool onClickEnded();
		bool onClickGoing();

		std::map<std::string, Panel> panels;
		std::map<std::string, Label> labels;
	private:
		Shop* _shop;

	};

	bool onClickBegan();
	bool onClickEnded();
	bool onClickGoing();

	Panel _merchantPanel;
	Panel _focusPanel;
	_InfoPanel _infoPanel;
	Panel _quitPanel;
	std::map<uint32_t, std::unique_ptr<_ItemPanel>> _itemPanels;

	bool _in = false;

	bool _dragging = false;
	Vector2f _dragOffset = { 0, 0 };

	uint32_t focused = 0u;
};