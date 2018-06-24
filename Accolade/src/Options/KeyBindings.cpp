#include "KeyBindings.hpp"

KeyBindings::KeyBindings() {}
KeyBindings::KeyBindings(nlohmann::json config) : _config(config) {}


int KeyBindings::getKey(Command command) const noexcept {
	return _config[command];
}
