#include "Screen.hpp"

bool Screen::toExitScreen() const noexcept {
	return exit_screen;
}

bool Screen::to_back_screen() const noexcept {
	return go_back_screen;
}

void Screen::onEnter(std::any) {
	exit_screen = false;
	go_back_screen = false;
}
