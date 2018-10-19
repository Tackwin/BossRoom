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
