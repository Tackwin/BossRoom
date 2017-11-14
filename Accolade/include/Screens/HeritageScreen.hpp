#pragma once
#include <map>
#include <memory>

#include "Screen.hpp"

#include "Graphics/GUI/Panel.hpp"
#include "Graphics/GUI/Label.hpp"

#include "Managers/MemoryManager.hpp"

#include "Gameplay/Player.hpp"

class HeritageScreen : public Screen {
public:
	virtual void onEnter();
	virtual void onExit();

	virtual void update(double);
	virtual void render(sf::RenderTarget& target);
private:

	struct PlayerPanel : public Panel {
		PlayerPanel(PlayerInfo& info);

		PlayerInfo info;
		std::map<std::string, MM::unique_ptr<Label>> labels;
		std::map<std::string, MM::unique_ptr<Panel>> panels;
	};

	Widget _root;
	PlayerPanel _playerPanel;
};