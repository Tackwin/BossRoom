#pragma once
#include <functional>
#include <Math/Vector2.hpp>
#include <Math/Rectangle.hpp>

struct Collider {
	using Callback = std::function<void(void)>;

	Callback onExit = []() {};
	Callback onEnter = []() {};

	Vector2 dtPos = Vector2::ZERO;

	void* userPtr = nullptr;

	virtual bool isIn(const Vector2& p) const = 0;
	virtual bool collideWith(const Collider* collider) const = 0;

	virtual void render(sf::RenderTarget& target) = 0;

	Vector2 getGlobalPos() const { return __pos + dtPos; }
	void setPos(const Vector2& pos) { __pos = pos; }

protected:
	Vector2 __pos;
};

struct Disk : Collider {
	float r = 0.f;

	virtual bool isIn(const Vector2& p) const override {
		return (p - getGlobalPos()).length2() < r * r;
	}
	virtual bool collideWith(const Collider* collider) const override;

	virtual void render(sf::RenderTarget& target) override;
};

struct Box : Collider {
	Vector2 size;

	virtual bool isIn(const Vector2& p) const override {
		return Rectangle(getGlobalPos(), size).isInside(p);
	};
	virtual bool collideWith(const Collider* collider) const override {
		if (auto ptr = dynamic_cast<const Box*>(collider); ptr) {
			return Rectangle(getGlobalPos(), size).intersect(Rectangle(ptr->getGlobalPos(), ptr->size));
		}
		return false;
	};

	virtual void render(sf::RenderTarget& target) override;
};
