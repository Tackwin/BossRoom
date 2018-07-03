#include "Plateforme.hpp"

Plateforme::Plateforme(Vector2f pos, Vector2f size) noexcept {
	auto boxPtr = std::make_unique<Box>();
	boxPtr->setPos(pos);
	boxPtr->setSize(size);

	auto temp = std::unique_ptr<Collider>(boxPtr.release());
	collider.swap(temp);
}

void Plateforme::update(double) noexcept {
}
void Plateforme::render(sf::RenderTarget&) noexcept {
}
void Plateforme::renderDebug(sf::RenderTarget&) noexcept {
}