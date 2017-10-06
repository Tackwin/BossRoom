#pragma once

#include <Gameplay/Weapon.hpp>
#include <Graphics/GUI/Widget.hpp> 
#include <Graphics/GUI/Panel.hpp> 

class Shop : public Widget {
public:
	Shop();
	~Shop();

	void addWeapon(const std::shared_ptr<Weapon>& weapon);

	void enter();
	void leave();
private:
	void onClickBegan();
	void onClickEnded();
	void onClickGoing();

	Panel _merchantPanel;
	std::vector<Panel> _itemPanels;

	bool _in = false;

	bool _dragging = false;
	Vector2 _dragOffset = Vector2::ZERO;
};