#include "Graphics/Sprite.hpp"
#include "Managers/AssetManager.hpp"

Sprite::Sprite() {
	_info.mesh = VAO::create_quad({ 1, 1 });
}

Sprite::Sprite(const Sprite&& that) {
	this->operator=(std::move(that));
}
Sprite& Sprite::operator=(const Sprite&& that) {
	_info.texture = that._info.texture;
	_info.transform = that._info.transform;
	_info.mesh = std::move(that._info.mesh);
	return *this;
}


void Sprite::set_texture(const std::string& key) {
	if (!AM::find_texture(key)) return;

	_info.texture = &AM::get_texture(key);
}

void Sprite::render(const FrameBuffer& target) const {
	target.bind();
	_info.texture->bind();
	_info.mesh.render(6);
}