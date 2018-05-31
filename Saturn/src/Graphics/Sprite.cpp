#include "Graphics/Sprite.hpp"
#include "Managers/AssetManager.hpp"

Sprite::Sprite() {

}

void Sprite::set_texture(const std::string& key) {
	if (!AM::I().find_texture(key)) return;

	_info.texture = &AM::I().get_texture(key);
}

void Sprite::render() const {

}