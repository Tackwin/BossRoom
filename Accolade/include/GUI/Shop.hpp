#pragma once

#include <Gameplay/Weapon.hpp>
#include <Graphics/GUI/Widget.hpp> 
#include <Graphics/GUI/Label.hpp> 
#include <Graphics/GUI/Panel.hpp> 

class Shop : public Widget {
public:
	Shop();
	~Shop();

	void addWeapon(const std::shared_ptr<Weapon>& weapon);

	void enter();
	void leave();
private:
	struct _itemPanel : public Widget {
		_itemPanel() {
			addChild(&back, 0);
			addChild(&sprite, 1);
			addChild(&label, 2);

			sprite.setPosition({ 2, 2 });
		}

		Panel back;
		Panel sprite;
		Label label;
	};


	void onClickBegan();
	void onClickEnded();
	void onClickGoing();

	Panel _merchantPanel;
	Panel _quitPanel;
	std::vector<std::unique_ptr<_itemPanel>> _itemPanels;

	bool _in = false;

	bool _dragging = false;
	Vector2f _dragOffset = { 0, 0 };
};