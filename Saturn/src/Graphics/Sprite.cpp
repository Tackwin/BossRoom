#include "Graphics/Sprite.hpp"
#include "Managers/AssetManager.hpp"

Sprite::Sprite() {
	VAO::create_quad(_info.mesh, { 1.f, 1.f });
}

void Sprite::set_shader(const std::string& key) noexcept {
	_info.shaderKey = key;
}
void Sprite::set_texture(const std::string& key) noexcept {
	_info.textureKey = key;
}
Transform& Sprite::get_transform() noexcept {
	return _info.transform;
}

void Sprite::render() const {
	auto& am = AM::I();

	_info.transform.apply_to_shader(am.get_shader(_info.shaderKey));

	am.get_shader(_info.shaderKey).use();
	am.get_texture(_info.textureKey).bind();
	_info.mesh.bind();
	_info.mesh.render(6);
}