#include <Physics/Collider.hpp>

void Disk::render(sf::RenderTarget& target) {
	sf::CircleShape shape(r);
	shape.setFillColor(sf::Color(200, 200, 200, 200));
	shape.setOrigin(r, r);
	shape.setPosition(pos);
	target.draw(shape);
}

void Box::render(sf::RenderTarget& target) {
	sf::RectangleShape shape(size);
	shape.setPosition(pos);
	shape.setFillColor(sf::Color(200, 200, 200, 200));
	target.draw(shape);
}
