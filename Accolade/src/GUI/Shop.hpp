#pragma once

#include <map>

#include "./../Gameplay/Wearable/Wearable.hpp"
#include "./../Gameplay/Player/Player.hpp"

#include "./../Graphics/GUI/Widget.hpp" 
#include "./../Graphics/GUI/Button.hpp"
#include "./../Graphics/GUI/Label.hpp"
#include "./../Graphics/GUI/Panel.hpp" 

class Shop : public Widget {
public:
	Shop();
	~Shop();

	void setPlayer(std::weak_ptr<Player> player);

	void addWeapon(const std::string& weapon);

	void focusItem(u32 i);
	void unFocus();

	void enter();
	void leave();

	bool isIn() const;
private:
	struct _ItemPanel : public Panel {
		static constexpr float PANEL_SIZE = 60.f;

		_ItemPanel() {};
		_ItemPanel(Shop* shop, const std::string& weapon, u32 id);

		bool onClickBegan();
		bool onClickEnded();
		bool onClickGoing();

		Panel sprite;
		Label label;

		std::string weapon;

	private:
		Shop* _shop;
		u32 _id;
	};
	struct _InfoPanel : public Panel {
	public:
		_InfoPanel() {};
		_InfoPanel(Shop* shop);

		void populateBy(const std::string& weapon);

		bool onClickBegan();
		bool onClickEnded();
		bool onClickGoing();

		std::map<std::string, Panel> panels;
		std::map<std::string, Label> labels;
		std::map<std::string, Button> buttons;
	private:
		
		Shop* _shop;

		std::string _weapon;
	};

	bool onClickBegan();
	bool onClickEnded();
	bool onClickGoing();

	Panel _merchantPanel;
	Panel _focusPanel;
	_InfoPanel _infoPanel;
	Panel _quitPanel;
	std::map<u32, std::unique_ptr<_ItemPanel>> _itemPanels;

	bool _in = false;

	bool _dragging = false;
	Vector2f _dragOffset = { 0, 0 };

	u32 _focused = 0u;

	std::weak_ptr<Player> _player;
};