#pragma once
#include <map>
#include <memory>

#include "Screen.hpp"

#include "./../Graphics/GUI/Panel.hpp"
#include "./../Graphics/GUI/Label.hpp"

#include "./../Managers/MemoryManager.hpp"

#include "./../Gameplay/Player/Player.hpp"

class HeritageScreen : public Screen {
public:
	HeritageScreen(const PlayerInfo& info);


	virtual Type getType() const noexcept override { return edit_screen; }

	virtual void onEnter(std::any) override;
	virtual std::any onExit() override;

	virtual void update(double) override;
	virtual void render(sf::RenderTarget& target) override;
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