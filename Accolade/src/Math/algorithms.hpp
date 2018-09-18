#pragma once
#include "Vector.hpp"
#include <string>

extern bool is_in_ellipse(Vector2f A, Vector2f B, float r, Vector2f p) noexcept;
extern double evaluate_expression(const std::string& str) noexcept;