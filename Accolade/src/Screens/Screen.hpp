#pragma once
#include <any>
#include <memory>
#include <SFML/Graphics.hpp>

class Screen : public std::enable_shared_from_this<Screen> {
public:
	enum Type {
		start_screen,
		level_screen,
		edit_screen,
		count
	};

	bool exit_screen{ false };
	bool go_back_screen{ false };
public:
	virtual Type getType() const noexcept = 0;

	virtual void onEnter(std::any);
	virtual std::any onExit() = 0;

	virtual void update(double dt) = 0;
	virtual void render(sf::RenderTarget& target) = 0;
	bool toExitScreen() const noexcept;
	bool to_back_screen() const noexcept;
};

