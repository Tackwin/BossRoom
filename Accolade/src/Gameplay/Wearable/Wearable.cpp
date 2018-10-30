#include "Wearable.hpp"

#include "./../Player/Player.hpp"

Wearable::Wearable() noexcept {}

Wearable::Wearable(WearableInfo info) noexcept : _info(info) {
}

void Wearable::mount(std::weak_ptr<Player> player) noexcept {
	_player = player;
	_info.onMount(*this);
}
void Wearable::unmount() noexcept {
	_info.onUnmount(*this);
}

void Wearable::render(sf::RenderTarget&) noexcept {}

void Wearable::update(double dt) noexcept {
	_info.onUpdate(dt, *this);
}

WearableInfo Wearable::getInfo() const noexcept {
	return _info;
}

const UUID& Wearable::getUUID() const noexcept {
	return _uuid;
}