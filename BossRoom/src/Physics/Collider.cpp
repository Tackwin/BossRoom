#include <Physics/Collider.hpp>

void Disk::render(sf::RenderTarget& target) {
	sf::CircleShape shape(r);
	shape.setFillColor(sf::Color(200, 200, 200, 200));
	shape.setOrigin(r, r);
	shape.setPosition(pos);
	target.draw(shape);
}

bool Disk::collideWith(const Collider* collider) const {


	if (auto ptr = dynamic_cast<const Disk*>(collider); ptr) {
		return (pos - ptr->pos).length2() < (r + ptr->r) * (r + ptr->r);
	}
	if (auto ptr = dynamic_cast<const Box*>(collider); ptr) {
		return ptr->isIn(pos);
	}
	return false;
}

void Box::render(sf::RenderTarget& target) {
	sf::RectangleShape shape(size);
	shape.setPosition(pos);
	shape.setFillColor(sf::Color(200, 200, 200, 200));
	target.draw(shape);
}
