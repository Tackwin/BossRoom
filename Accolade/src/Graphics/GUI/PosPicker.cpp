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

	Callback on_click;
	on_click.began = std::bind(&PosPicker::onClickBegan, this);

	Callback on_focus;
	on_focus.began = std::bind(&PosPicker::onFocusBegan, this);
	on_focus.going = std::bind(&PosPicker::onFocusGoing, this);
	on_focus.ended = std::bind(&PosPicker::onFocusEnded, this);


	label = makeChild<Label>(json.at("label"), 0);
	label->setStdString(static_cast<std::string>(x));

	panel = makeChild<Panel>({ {"sprite", "point"} }, 1);
	panel->setPosition({ label->getSize().x + 2, 0 });
	panel->getSprite().setColor(Vector4d{ 1, 1, 1, 0.5 });
	panel->setOnFocus(on_focus);
	panel->setOnClick(on_click);
	panel->setSize({ 20, 20 });
}

void PosPicker::render(sf::RenderTarget& target) noexcept {
	Widget::render(target);

	label->setStdString(static_cast<std::string>(x));

	auto mouse_pos = IM::getMouseScreenPos();
	auto x_mark = x;
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
			x_mark = render_window->mapCoordsToPixel(x, *v);
		}
	}

	sf::CircleShape mark{ 5 };
	mark.setOrigin(mark.getRadius(), mark.getRadius());
	mark.setPosition(x_mark);
	auto temp_view = target.getView();
	target.setView(target.getDefaultView());
	target.draw(mark);
	target.setView(temp_view);
}

bool PosPicker::onClickBegan() noexcept {
	panel->setFocus(!panel->isFocus());
	return true;
}

bool PosPicker::onFocusBegan() noexcept {
	assert(panel);

	panel->getSprite().setColor(Vector4d{ 1, 1, 1, 1 });

	return true;
}

bool PosPicker::onFocusGoing() noexcept {
	assert(panel);
	
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
			setX(render_window->mapPixelToCoords(mouse_pos, *v));
			return true;
		}
	}

	return true;
}

bool PosPicker::onFocusEnded() noexcept {
	assert(panel);

	panel->getSprite().setColor(Vector4d{ 1, 1, 1, 0.5 });

	return true;
}

void PosPicker::setViews(const std::vector<Eid<sf::View>>& views) noexcept {
	this->views = views;
}

UUID PosPicker::listenChange(PosPicker::ChangeCallback&& f) noexcept {
	UUID i;
	listeners.emplace(i, std::forward<ChangeCallback>(f));
	return i;
}
void PosPicker::stopListeningChange(UUID id) noexcept {
	assert(listeners.count(id));
	listeners.erase(id);
}
void PosPicker::setX(const Vector2f& x) noexcept {
	if (this->x == x) return;
	this->x = x;
	for (auto& f : listeners) f.second(x);
}