#include "PosPicker.hpp"

#include "Common.hpp"
#include "Managers/InputsManager.hpp"

PosPicker::PosPicker(const nlohmann::json& json) noexcept : Widget(json) {
	if (auto it = json.find("views"); it != std::end(json)) {
		views = it->get<decltype(views)>();
	}
	if (auto it = json.find("x"); it != std::end(json)) {
		x = *it;
	}

	Callback on_focus;
	on_focus.began = decltype(on_focus)::TRUE;
	on_focus.going = std::bind(&PosPicker::onFocusGoing, this);
	on_focus.ended = decltype(on_focus)::TRUE;

	label = makeChild<Label>(json.at("label"));
}

void PosPicker::render(sf::RenderTarget& target) noexcept {
	sf::CircleShape mark{ 5 };
	mark.setOrigin(mark.getRadius(), mark.getRadius());
	mark.setPosition(x);
	target.draw(mark);
}

bool PosPicker::onFocusGoing() noexcept {
	// Robustesse Quid des cas ou les views s'appliquent à une autre render_target ?
	auto mouse_pos = IM::getMouseScreenPos();
	for (auto& ptr_v : views) {
		auto v = es_instance->get(ptr_v);
		assert(v);

		Rectangle2f viewport{
			{v->getViewport().left, v->getViewport().top},
			{v->getViewport().width, v->getViewport().height}
		};
		viewport.x *= render_window->getSize().x;
		viewport.w *= render_window->getSize().x;
		viewport.y *= render_window->getSize().y;
		viewport.h *= render_window->getSize().y;

		if (viewport.in(mouse_pos)) {
			x = render_window->mapPixelToCoords(mouse_pos, *v);
			return true;
		}
	}

	label->setStdString(static_cast<std::string>(x));
	return true;
}

void PosPicker::setViews(const std::vector<Eid<sf::View>>& views) noexcept {
	this->views = views;
}