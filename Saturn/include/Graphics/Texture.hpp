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

	enum Wrap {
		Repeat = GL_REPEAT
	};
	enum Filter {
		Linear = GL_LINEAR
	};

	Texture();

	void set_wrap(Wrap wrap) const noexcept;
	void set_wrap(Wrap s, Wrap t) const noexcept;

	void set_filter(Filter filter) const noexcept;
	void set_filter(Filter min, Filter mag) const noexcept;

	bool load_file(const std::string& path);
	void create_rgb_null(const Vector2u& size) const;
	void create_depth_null(const Vector2u& size) const;

	void set_parameteri(i32 parameter, i32 value) const noexcept;
	void set_parameterfv(i32 parameter, float* value) const noexcept;
	void bind(u32 unit = 0u) const;

	u32 get_texture_id() const;
private:
	TextureInfo _info;
};