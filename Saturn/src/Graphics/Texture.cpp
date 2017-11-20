#include "Graphics/Texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "3rd/stb_image.hpp"
#include <iostream>

Texture::Texture() {
	glGenTextures(1, &_info.id);
}

void Texture::load_file(const std::string& path) {
	i32 comp = 0;

	stbi_set_flip_vertically_on_load(true);
	u08* data = stbi_load(
		path.c_str(), &_info.width, &_info.height, &comp, STBI_rgb_alpha
	);

	if (!data) {
		std::cerr << "Can't load file: " << path << std::endl;
		LOG_PLACE;
		return;
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
