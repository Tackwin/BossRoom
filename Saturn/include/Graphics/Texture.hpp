#pragma once

#include <string>
#include <glad/glad.h>

#include "Concepts/Delete.hpp"
#include "Common.hpp"
#include "Math/Vector.hpp"

struct TextureInfo {
	u32 id = 0u;
	i32 width = 0u;
	i32 height = 0u;
};

class Texture : NoCopy {
public:

	Texture();

	Texture(const Texture&& that);
	Texture& operator=(const Texture&& that);

	bool load_file(const std::string& path);
	void create_rgb_null(const Vector<2, u32>& size) const;
	void create_depth_null(const Vector<2, u32>& size) const;

	void set_parameteri(i32 parameter, i32 value) const;
	void set_parameterfv(i32 parameter, float* value) const;
	void bind(u32 unit = 0u) const;

	u32 get_texture_id() const;
private:
	TextureInfo _info;
};