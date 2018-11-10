#include "Panel.hpp"

#include "Managers/AssetsManager.hpp"
#include "Managers/InputsManager.hpp"

#include "OS/SystemConfiguration.hpp"

Panel::Panel(nlohmann::json json) noexcept : Widget(json) {
	auto ogSize = getSize();
	if (auto it = json.find("sprite"); it != json.end())
		setTexture(*it);
	setSize(ogSize);
	fullHeight = getSize().y;
	textureFullHeight = _backSprite.getTextureRect().height;

	if (auto it = json.find("draggable"); it != json.end())
		setDraggable(*it);
	if (auto it = json.find("collapsable"); it != json.end())
		setCollapsable(*it);
	if (auto it = json.find("resizable"); it != json.end())
		setResizable(*it);
	if (auto it = json.find("collapsed"); it != json.end() && it->get<bool>())
		toggleCollapse();
	if (auto it = json.find("collapsed"); it != json.end())
		setClipper(*it);

	Callback onClick;
	onClick.began = [&]{
		_focused = true;
		auto mousePos = IM::getMouseScreenPos();

		if (isInHeader(mousePos)) {
			deltaDrag = mousePos - getGlobalPosition();

			if (doubleClickClock && !doubleClickClock->isOver()) {
				toggleCollapse();
				doubleClickClock.reset();
			}
			else {
				doubleClickClock = Clock{ get_double_click_time().value_or(1000) / 1000.0 };
			}
			return true;
		}

		return false;
	};
	onClick.going = [&] {
		if (deltaDrag) {
			setGlobalPosition(IM::getMouseScreenPos() - *deltaDrag);
			return true;
		}
		return false;
	};
	onClick.ended = [&] {
		_focused = false;
		deltaDrag.reset();
		return true;
	};

	Callback onHover;
	onHover.began = Callback::TRUE;
	onHover.going = std::bind(&Panel::onHoverGoing, this);
	onHover.ended = Callback::TRUE;

	setOnHover(onHover);
	setOnClick(onClick);
}

sf::Sprite& Panel::getSprite() {
	return _backSprite;
}

void Panel::setTexture(std::string texture) {
	_texture = texture;
	_backSprite.setTexture(AM::getTexture(texture));
	computeSize();
}
void Panel::setTexture(sf::Texture& texture) noexcept {
	_texture = "";
	_backSprite.setTexture(texture);
	computeSize();
}
std::string Panel::getTexture() const noexcept {
	return _texture;
}

void Panel::computeSize() {
	_size.x = _backSprite.getGlobalBounds().width;
	_size.y = _backSprite.getGlobalBounds().height;
}

float Panel::getSizeRatio() const noexcept {
	return _size.x / _size.y;
}

void Panel::render(sf::RenderTarget& target) {
	Widget::render(target);
	if (!_visible) return;

	auto boundingBox = getGlobalBoundingBox();

	if (isClipper()) {
	}

	_backSprite.setScale(
		_size.x / _backSprite.getTextureRect().width,
		_size.y / _backSprite.getTextureRect().height
	);
	_backSprite.setOrigin({
		_origin.x * _backSprite.getTextureRect().width,
		_origin.y * _backSprite.getTextureRect().height
	});

	_backSprite.setPosition(getGlobalPosition());
	target.draw(_backSprite);
}

bool Panel::isDraggable() const noexcept {
	return draggable;
}
bool Panel::isCollapsable() const noexcept {
	return collapsable;
}
bool Panel::isResizable() const noexcept {
	return resizable;
}
bool Panel::isCollapsed() const noexcept {
	return collapsed;
}
bool Panel::isClipper() const noexcept {
	return clipper;
}

void Panel::setDraggable(bool v) noexcept {
	draggable = v;
}
void Panel::setCollapsable(bool v) noexcept {
	collapsable = v;
}
void Panel::setResizable(bool v) noexcept {
	resizable = v;
}
void Panel::setClipper(bool v) noexcept {
	clipper = v;

	if (!clipper) {
	}
	else {
	}
}

std::string Panel::getTitle() const noexcept {
	return title;
}
void Panel::setTitle(std::string str) noexcept {
	title = str;
}

bool Panel::isInHeader(Vector2f pos) const noexcept {
	//TODO: figure out a better algorithm.
	//maybe it can be configurable

	auto box = getGlobalBoundingBox();
	if (!isCollapsed()) box.h = fullHeight * 0.1f;

	return box.in(pos);
}

void Panel::toggleCollapse() noexcept {
	if (collapsed) {
		for (auto&[w, s] : visibleStatebeforeCollapse) {
			w->setVisible(s);
		}

		_backSprite.setTextureRect({
			_backSprite.getTextureRect().left,
			_backSprite.getTextureRect().top,
			_backSprite.getTextureRect().width,
			textureFullHeight
		});
		setSize({ getSize().x, fullHeight });
		collapsed = false;
	}
	else {
		for (auto[_, c] : _childs) {
			_;
			visibleStatebeforeCollapse[c] = c->isVisible();
			c->setVisible(false);
		}

		_backSprite.setTextureRect({
			_backSprite.getTextureRect().left,
			_backSprite.getTextureRect().top,
			_backSprite.getTextureRect().width,
			(int)ceil(textureFullHeight * 0.1)
		});
		setSize({ getSize().x, fullHeight * 0.1f });
		collapsed = true;
	}

}

bool Panel::onHoverGoing() noexcept {

	if (IM::getLastScroll() != 0) {
		auto shift = IM::isKeyPressed(sf::Keyboard::LShift);

		auto& toScroll = shift ? scroll.y : scroll.x;
		toScroll += IM::getLastScroll() * (shift ? getSize().y : getSize().x) / 10.f;
	}

	return true;
}






















