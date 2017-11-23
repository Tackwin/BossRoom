#pragma once

#include "Math/Vector.hpp"
#include "Math/Matrix.hpp"

struct TransformInfo {
	Vector2f origin;
	Vector2f size;
	Vector2f pos;
	float theta;
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

	Matrix4f apply(const Matrix4f& model) const;

private:

	TransformInfo _info;
	Vector2f _scale;
};

