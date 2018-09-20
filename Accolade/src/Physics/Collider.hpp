#pragma once

#include <functional>
#include <SFML/Graphics.hpp>

#include "./../Common.hpp"

#include "./../Math/Vector.hpp"
#include "./../Math/Rectangle.hpp"

struct Object;
struct Collider {
	enum struct Type : u32 {
		DISK,
		BOX
	} type;

	using Callback = std::function<void(Object*)>;
	using Filter = std::function<bool(Object&)>;

	Callback onEnter = [](Object*) {};
	Callback onExit = [](Object*) {};

	Filter filterCollide = [](Object&) { return true; };

	Vector2f dtPos = { 0, 0 };

	bool sensor = false;

	void* userPtr = nullptr;

	virtual bool isIn(const Vector2f& p) const = 0;
	virtual bool collideWith(const Collider* collider) const = 0;

	virtual void render(sf::RenderTarget& target) = 0;

	Vector2f getGlobalPos() const { return __pos + dtPos; }
	void setPos(const Vector2f& pos) { __pos = pos; }

protected:
	Vector2f __pos;

private:
	bool _colliding = false;
};

struct Disk : Collider {
	float r = 0.f;

	Disk() :
		Collider()
	{
		type = Type::DISK;
	}

	virtual bool isIn(const Vector2f& p) const override {
		return (p - getGlobalPos()).length2() < r * r;
	}
	virtual bool collideWith(const Collider* collider) const override;

	virtual void render(sf::RenderTarget& target) override;
};
struct Box : Collider {
	Vector2f size;

	Box() :
		Collider()
	{
		type = Type::BOX;
	}

	Rectangle2f getGlobalBoundingBox() const noexcept;
	virtual bool isIn(const Vector2f& p) const override {
		return p.inRect(getGlobalPos(), size);
	};
	virtual bool collideWith(const Collider* collider) const override;

	virtual void render(sf::RenderTarget& target) override;

	virtual Vector2f getSize() const noexcept;
	virtual void setSize(Vector2f size) noexcept;
private:
};