#include "BoolPicker.hpp"

#include "Panel.hpp"

#include "Managers/AssetsManager.hpp"

BoolPicker::BoolPicker(const nlohmann::json& json) noexcept : Widget(json) {
	if (auto it = json.find("on_texture"); it != std::end(json)) {
		// TODO
	}
	if (auto it = json.find("off_texture"); it != std::end(json)) {
		// TODO
	}
	if (auto it = json.find("x"); it != std::end(json)) {
		x = *it;
	}

	Callback on_click;
	on_click.ended = [&] { return onClickEnded(); };

	panel = makeChild<Panel>({
		{"sprite", "off_on_off"}
	});
	panel->setOnClick(on_click);
	panel->setSize(getSize());
}

void BoolPicker::render(sf::RenderTarget& target) noexcept {
	Widget::render(target);

	panel->getSprite().setTexture(AM::getTexture(x ? "on_on_off" : "off_on_off"));
}

UUID BoolPicker::listenChange(BoolPicker::ChangeCallback&& f) noexcept {
	UUID i;
	listeners[i] = f;
	return i;
}

void BoolPicker::stopListeningChange(UUID i) noexcept {
	assert(listeners.count(i));
	listeners.erase(i);
}

void BoolPicker::setX(bool x) noexcept {
	if (this->x != x) {
		this->x = x;
		for (auto& f : listeners) {
			f.second(x);
		}
	}
}

bool BoolPicker::getX() const noexcept {
	return x;
}

void BoolPicker::setSize(const Vector2f& size) noexcept {
	Widget::setSize(size);
	panel->setSize(size);
}

bool BoolPicker::onClickEnded() noexcept {
	setX(!getX());
	return true;
}