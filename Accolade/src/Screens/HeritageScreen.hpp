#pragma once
#include <map>
#include <memory>

#include "Screen.hpp"

#include "./../Graphics/GUI/Panel.hpp"
#include "./../Graphics/GUI/Label.hpp"

#include "./../Managers/MemoryManager.hpp"

#include "./../Gameplay/Player/Player.hpp"
#include "./../Gameplay/Player/PlayerInfo.hpp"

class HeritageScreen : public Screen {
public:
	HeritageScreen(const PlayerInfo& info);

	virtual void onEnter();
	virtual void onExit();

	virtual void update(double);
	virtual void render(sf::RenderTarget& target);
private:

	struct PlayerPanel : public Panel {
		PlayerPanel();

		void populateWith(PlayerInfo& info_);

		PlayerInfo info;
		std::map<std::string, std::shared_ptr<Label>> labels;
		std::map<std::string, std::shared_ptr<Panel>> panels;
	};

	Widget _root;
	PlayerPanel _playerPanel;
	PlayerInfo _info;
};