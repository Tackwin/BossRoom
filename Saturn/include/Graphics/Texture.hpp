#pragma once

#include <string>
#include <glad/glad.h>

#include "Common.hpp"
#include "Math/Vector.hpp"

struct TextureInfo {
	u32 id = 0u;
	i32 width = 0u;
	i32 height = 0u;
};

class Texture {
public:

	Texture();

	bool load_file(const std::string& path);
	void create_rgb_null(const Vector2f& size) const;
	void create_depth_null(const Vector2f& size) const;

	void set_parameteri(i32 parameter, i32 value) const;
	void set_parameterfv(i32 parameter, float* value) const;
	void bind(u32 unit = 0u) const;

	u32 get_texture_id() const;
private:
	TextureInfo _info;
};