#include "Graphics/Sprite.hpp"
#include "Managers/AssetManager.hpp"

Sprite::Sprite() {

}

void Sprite::set_texture(const std::string& key) {
	if (!AM::find_texture(key)) return;

	_info.texture = &AM::get_texture(key);
}

void Sprite::render() const {

}