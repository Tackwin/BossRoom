#pragma once

#include <string>
#include <glad/glad.h>

#include "Common.hpp"

struct TextureInfo {
	u32 id = 0u;
	i32 width = 0u;
	i32 height = 0u;
};

class Texture {
public:

	Texture();

	void load_file(const std::string& path);

	void set_parameteri(i32 parameter, i32 value) const;
	void set_parameterfv(i32 parameter, float* value) const;
	void bind(u32 unit = 0u) const;
private:
	TextureInfo _info;
};