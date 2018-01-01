#pragma once

#include "Graphics/Shader.hpp"

#include "Math/Vector.hpp"
#include "Math/Matrix.hpp"

struct TransformInfo {
	Vector2f origin = { 0, 0 };
	Vector2f size = { 1, 1 };
	Vector2f pos = { 0, 0 };
	float theta = 0.f;
};

class Transform {
public:

	void set_origin(const Vector2f& origin);
	void set_position(const Vector2f& pos);
	void set_scale(const Vector2f& scale);
	void set_size(const Vector2f& size);
	void set_rotation(float theta);

	TransformInfo get_info() const;
	Vector2f get_scale() const;

	Matrix4f apply(const Matrix4f& model = Matrix4f::identity()) const;
	void apply_to_shader(const Shader& shader) const;

private:

	TransformInfo _info;
	Vector2f _scale = { 1, 1 };
};

