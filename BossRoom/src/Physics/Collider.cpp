#include <Physics/Collider.hpp>

bool Collider::collideWith(const Disk&) { return false; }

void Disk::render(sf::RenderTarget& target) {
	sf::CircleShape shape(r);
	shape.setFillColor(sf::Color(200, 200, 200, 200));
	shape.setOrigin(r, r);
	shape.setPosition(pos);
	target.draw(shape);
}
