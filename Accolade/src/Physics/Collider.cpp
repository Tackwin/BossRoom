#include <Physics/Collider.hpp>

bool DiskRect(Vector2 circle, float r, Rectangle rect) {
	Vector2 circleDistance;

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
	if (auto ptr = dynamic_cast<const Disk*>(collider); ptr) {
		return (getGlobalPos() - ptr->getGlobalPos()).length2() < (r + ptr->r) * (r + ptr->r);
	}
	else if (auto ptr = dynamic_cast<const Box*>(collider); ptr) {
		return ptr->isIn(getGlobalPos());
	}
	return false;
}

bool Box::collideWith(const Collider* collider) const {
	if (auto ptr = dynamic_cast<const Disk*>(collider); ptr) {
		Vector2 halfSize = size * 0.5f;
		Vector2 center = ptr->getGlobalPos() - (getGlobalPos() + halfSize);

		Vector2 side = {
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
	else if (auto ptr = dynamic_cast<const Box*>(collider); ptr) {
		return Rectangle(getGlobalPos(), size).intersect(Rectangle(ptr->getGlobalPos(), ptr->size));
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
