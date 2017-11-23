#include "Graphics/Transform.hpp"


void Transform::set_origin(const Vector2f& origin) {
	_info.origin = origin;
}
void Transform::set_position(const Vector2f& pos) {
	_info.pos = pos;
}
void Transform::set_scale(const Vector2f& scale) {
	_scale = scale;
}
void Transform::set_size(const Vector2f& size) {
	_info.size = size;
}
void Transform::set_rotation(float theta) {
	_info.theta = theta;
}

TransformInfo Transform::get_info() const {
	return _info;
}
Vector2f Transform::get_scale() const {
	return _scale;
}

Matrix4f Transform::apply(const Matrix4f& model) const {
	auto result = Matrix4f::translation({
		_info.origin.x,
		_info.origin.y,
		0
	});
	result = Matrix4f::scalar({
		_scale.x * _info.size.x,
		_scale.y * _info.size.y,
		1, 
		1
	}) * result;
	result = Matrix4f::rotation({ 0, 0, 1 }, _info.theta) * result;
	result = Matrix4f::translation({
		_info.pos.x,
		_info.pos.y,
		0
	}) * result;
	return model * result;
}
