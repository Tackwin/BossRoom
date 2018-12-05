#include "algorithms.hpp"

bool is_in_ellipse(Vector2f A, Vector2f B, float small_axis, Vector2f p) noexcept {
	return	(p - A).length() + (p - B).length() <=
			(B - A).length() + small_axis;
}
double evaluate_expression(const std::string& str) noexcept {
	// TODO

	return std::stod(str);
}

std::optional<Vector2f> segment_rec(const Segment2f& seg, const Rectangle2f& rec) noexcept {
	if (auto x = segment_segment(seg, { rec.pos, {rec.x, rec.y + rec.h} }))
		return x;
	if (auto x = segment_segment(seg, { {rec.x, rec.y + rec.h}, rec.pos + rec.size }))
		return x;
	if (auto x = segment_segment(seg, { rec.pos + rec.size, {rec.x + rec.w, rec.y} }))
		return x;
	if (auto x = segment_segment(seg, { {rec.x + rec.w, rec.y}, rec.pos }))
		return x;
	return {};
}

std::optional<Vector2f> segment_segment(const Segment2f& A, const Segment2f& B) noexcept {
	auto s1 = A.B - A.A;
	auto s2 = B.B - B.A;

	float det = (-s2.x * s1.y + s1.x * s2.y);
	if (det == 0) return {};

	float s, t;
	s = (-s1.y * (A.A.x - B.A.x) + s1.x * (A.A.y - B.A.y)) / det;
	t = (s2.x * (A.A.y - B.A.y) - s2.y * (A.A.x - B.A.x)) / det;

	if (std::max({ s, t, 1.f }) != 1 || std::min({ s, t, 0.f }) != 0) return {};

	return A.A + t * s1;
}

std::optional<Vector2f> ray_circle(const Rayf& ray, const Circlef& c) noexcept {
	auto e = Vector2f::createUnitVector(ray.angle);
	auto h = c.c - ray.pos;
	auto lf = e.dot(h);
	auto s = (c.r * c.r) - h.dot(h) + lf * lf;

	if (s < 0.0)	return std::nullopt;
	else			return e * (lf - std::sqrt(s)) + ray.pos;
}

std::optional<Vector2f> ray_rectangle(const Rayf& ray, const Rectangle2f& rec) noexcept {
	std::optional<Vector2f> x = std::nullopt;
	
	if (ray.pos.x < rec.x)
		x = ray_segment(ray, { rec.pos, {rec.x, rec.y + rec.h} });
	else if (ray.pos.x > rec.x + rec.size.x)
		x = ray_segment(ray, { rec.pos + rec.size, {rec.x + rec.w, rec.y} });

	if (ray.pos.y < rec.y)
		x = ray_segment(ray, { {rec.x + rec.w, rec.y}, rec.pos });
	else if (ray.pos.y > rec.y + rec.size.y)
		x = ray_segment(ray, { {rec.x, rec.y + rec.h}, rec.pos + rec.size });
	
	return x;
}

std::optional<Vector2f> ray_segment(const Rayf& A, const Segment2f& B) noexcept {
	auto s1 = Vector2f::createUnitVector(A.angle);
	auto s2 = B.B - B.A;

	float det = (-s2.x * s1.y + s1.x * s2.y);
	if (det == 0) return {};

	float s, t;
	s = (-s1.y * (A.pos.x - B.A.x) + s1.x * (A.pos.y - B.A.y)) / det;
	t = (s2.x * (A.pos.y - B.A.y) - s2.y * (A.pos.x - B.A.x)) / det;

	if (std::max({ s, 1.f }) != 1 || std::min({ s, t, 0.f }) != 0) return {};

	return A.pos + t * s1;
}
