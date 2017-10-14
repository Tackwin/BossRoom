#include "Physics/Collider.hpp"

#include "Math/Rectangle.hpp"

bool DiskRect(Vector2f circle, float r, Rectangle<2, float> rect) {
	Vector2f circleDistance;

	circleDistance.x = abs(circle.x - rect.x);
	circleDistance.y = abs(circle.y - rect.y);

	if (circleDistance.x > (rect.w / 2 + r)) { return false; }
	if (circleDistance.y > (rect.h / 2 + r)) { return false; }

	if (circleDistance.x <= (rect.w / 2)) { return true; }
	if (circleDistance.y <= (rect.h / 2)) { return true; }

	auto cornerDistance_sq = (circleDistance.x - rect.w / 2) * (circleDistance.x - rect.w / 2) + 
							 (circleDistance.y - rect.h / 2) * (circleDistance.y - rect.h / 2);

	return cornerDistance_sq <= r * r;
}

void Disk::render(sf::RenderTarget& target) {
	sf::CircleShape shape(r);
	shape.setFillColor(sf::Color(200, 200, 200, 200));
	shape.setOrigin(r, r);
	shape.setPosition(getGlobalPos());
	target.draw(shape);
}

bool Disk::collideWith(const Collider* collider) const {
#pragma warning(disable:4456)
	if (auto ptr = static_cast<const Disk*>(collider); collider->type == Type::DISK) {
		return (getGlobalPos() - ptr->getGlobalPos()).length2() < (r + ptr->r) * (r + ptr->r);
	}
	else if (auto ptr = static_cast<const Box*>(collider); collider->type == Type::BOX) {
		return ptr->isIn(getGlobalPos());
	}
	return false;
}

bool Box::collideWith(const Collider* collider) const {
	if (auto ptr = static_cast<const Disk*>(collider); collider->type == Type::DISK) {
		Vector2f halfSize = size * 0.5f;
		Vector2f center = ptr->getGlobalPos() - (getGlobalPos() + halfSize);

		Vector2f side = {
			fabs(center.x) - halfSize.x,
			fabs(center.y) - halfSize.y
		};

		if (side.x > ptr->r || ptr->r < side.y)
			return false;
		if (side.x < -ptr->r && -ptr->r > side.y)
			return true;
		if (side.x < 0.f || side.y < 0.f)
			return true;

		return side.length2() < ptr->r * ptr->r;
	}
	else if (auto ptr = static_cast<const Box*>(collider); collider->type == Type::BOX) {
		return Rectangle<2, float>(getGlobalPos(), size).intersect(Rectangle<2, float>(ptr->getGlobalPos(), ptr->size));
	}
	return false;
#pragma warning(default:4456)
}

void Box::render(sf::RenderTarget& target) {
	sf::RectangleShape shape(size);
	shape.setPosition(getGlobalPos());
	shape.setFillColor(sf::Color(200, 200, 200, 200));
	target.draw(shape);
}
