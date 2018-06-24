#include "Graphics/Texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "3rd/stb_image.hpp"
#include <iostream>

Texture::Texture() {
	glGenTextures(1, &_info.id);
}

Texture::Texture(const Texture&& that) : _info(that._info) {}
Texture& Texture::operator=(const Texture&& that) {
	_info = that._info;
	return *this;
}

bool Texture::load_file(const std::string& path) {
	i32 comp = 0;

	stbi_set_flip_vertically_on_load(true);
	u08* data = stbi_load(
		path.c_str(), &_info.width, &_info.height, &comp, STBI_rgb_alpha
	);

	if (!data) {
		std::cerr << "Can't load file: " << path << std::endl;
		LOG_PLACE;
		return false;
	}

	bind();
	glTexImage2D(
		GL_TEXTURE_2D, 
		0, 
		GL_RGBA, 
		_info.width, 
		_info.height, 
		0, 
		GL_RGBA, 
		GL_UNSIGNED_BYTE, 
		data
	);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
	return true;
}
void Texture::create_rgb_null(const Vector<2, u32>& size) const {
	bind();
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGB,
		size.x,
		size.y,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		NULL
	);
	glGenerateMipmap(GL_TEXTURE_2D);
}
void Texture::create_depth_null(const Vector<2, u32>& size) const {
	bind();
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_DEPTH24_STENCIL8,
		size.x,
		size.y,
		0,
		GL_DEPTH_STENCIL,
		GL_UNSIGNED_BYTE,
		NULL
	);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::set_parameteri(i32 parameter, i32 value) const {
	bind();
	glTexParameteri(GL_TEXTURE_2D, parameter, value);
}
void Texture::set_parameterfv(i32 parameter, float* value) const {
	bind();
	glTexParameterfv(GL_TEXTURE_2D, parameter, value);
}

void Texture::bind(u32 unit) const {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, _info.id);
}

u32 Texture::get_texture_id() const {
	return _info.id;
}
