#pragma once
#include <Math/Vector2.hpp>
#include <Math/Rectangle.hpp>

struct Disk;

struct Collider {
	Vector2 pos = Vector2::ZERO;
	void* userPtr = nullptr;

	virtual bool isIn(const Vector2& p) = 0;
	virtual bool collideWith(const Collider* collider) = 0;

	virtual void render(sf::RenderTarget& target) = 0;
};

struct Disk : Collider {
	float r = 0.f;

	virtual bool isIn(const Vector2& p) override {
		return (p - pos).length2() < r * r;
	}
	virtual bool collideWith(const Collider* collider) override {
		if (auto ptr = dynamic_cast<const Disk*>(collider); ptr) {
			return (pos - ptr->pos).length2() < (r + ptr->r) * (r + ptr->r);
		}
		return false;
	}

	virtual void render(sf::RenderTarget& target) override;
};

struct Box : Collider {
	Vector2 size;

	virtual bool isIn(const Vector2& p) override {
		return Rectangle(pos, size).isInside(p);
	};
	virtual bool collideWith(const Collider* collider) override {
		if (auto ptr = dynamic_cast<const Box*>(collider); ptr) {
			return Rectangle(pos, size).intersect(Rectangle(ptr->pos, ptr->size));
		}
		return false;
	};

	virtual void render(sf::RenderTarget& target) override;
};
