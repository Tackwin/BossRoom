#pragma once

class NonMovable {
public:
	NonMovable() = default;
	NonMovable(NonMovable&&) = delete;
	NonMovable& operator=(NonMovable&&) = delete;
};

class Movable {
public:
	Movable() = default;
	Movable(Movable&&) = default;
	Movable& operator=(Movable&&) = default;
};