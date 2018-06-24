#include "Collider.hpp"

#include "./../Math/Rectangle.hpp"

bool DiskRect(Vector2f circle, float r, Rectangle2f rect) {
	Vector2f closest = Vector2f::clamp(circle, rect.pos, rect.pos + rect.size);

	// If the distance is less than the circle's radius, an intersection occurs
	return Vector2f::equal(circle, closest, r);
}

void Disk::render(sf::RenderTarget& target) {
	sf::CircleShape shape(r);
	shape.setFillColor(sf::Color(200, 200, 200, 200));
	shape.setOrigin(r, r);
	shape.setPosition(getGlobalPos());
	target.draw(shape);
}

#pragma warning(push)
#pragma warning(disable:4456)
bool Disk::collideWith(const Collider* collider) const {
	if (auto ptr = static_cast<const Disk*>(collider); 
		collider->type == Type::DISK) 
	{
		return 
			(getGlobalPos() - ptr->getGlobalPos()).length2() <
			(r + ptr->r) * (r + ptr->r);
	}
	else if (auto ptr = static_cast<const Box*>(collider); 
			collider->type == Type::BOX)
	{
		return DiskRect(
			getGlobalPos(), 
			r, 
			Rectangle2f(ptr->getGlobalPos(), ptr->size)
		);
	}
	return false;
}

bool Box::collideWith(const Collider* collider) const {
	if (auto ptr = static_cast<const Disk*>(collider); 
		collider->type == Type::DISK) 
	{
		return DiskRect(
			ptr->getGlobalPos(), 
			ptr->r, 
			Rectangle2f(getGlobalPos(), size)
		);
	}
	else if (auto ptr = static_cast<const Box*>(collider); 
			collider->type == Type::BOX)
	{
		return Rectangle2f(getGlobalPos(), size).intersect(
			Rectangle2f(ptr->getGlobalPos(), ptr->size)
		);
	}
	return false;
}
#pragma warning(pop)

void Box::render(sf::RenderTarget& target) {
	sf::RectangleShape shape(size);
	shape.setPosition(getGlobalPos());
	shape.setFillColor(sf::Color(200, 200, 200, 200));
	target.draw(shape);
}
