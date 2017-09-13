#pragma once
#include <Math/Vector2.hpp>

struct Disk;

struct Collider {
	Vector2 pos = Vector2::ZERO;
	void* userPtr = nullptr;

	virtual bool isIn(const Vector2& p) = 0;
	virtual bool collideWith(const Disk& disk);
};

struct Disk : Collider {
	float r = 0.f;

	virtual bool isIn(const Vector2& p) override {
		return (p - pos).length2() < r * r;
	}
	virtual bool collideWith(const Disk& disk) override {
		return (pos - disk.pos).length2() < (r + disk.r) * (r + disk.r);
	}

	void render(sf::RenderTarget& target);
};