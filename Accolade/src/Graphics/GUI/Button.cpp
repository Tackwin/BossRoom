#include "Button.hpp"

Button::Button(const nlohmann::json& json) :
	Widget(json),
	_normal{ new Panel{json.count("normal") ? json.at("normal") : nlohmann::json{}} },
	_hold{ new Panel{json.count("hold") ? json.at("hold") : nlohmann::json{}} },
	_label{ new Label{json.count("label") ? json.at("label") : nlohmann::json{}} }
{
	//_onClick.began = std::bind(&Button::onClickBegan, this);
	//_onClick.ended = std::bind(&Button::onClickEnded, this);

	if (const auto& it = json.find("texture"); it != std::end(json)) {
		setTexture(*it);
	}
	if (const auto& it = json.find("holdTexture"); it != std::end(json)) {
		setHoldSprite(*it);
	}
	if (const auto& it = json.find("color"); it != std::end(json)) {
		getSprite().setColor(Vector4d::loadJson(*it));
	}

	// the panels here are static so we overwrite their callbacks.
	_normal->setOnClick({});
	_hold->setOnClick({});

	_hold->setVisible(false);

	_normal->setParent(this, 0);
	_hold->setParent(this, 1);
	_label->setParent(this, 2);

	_normal->setOrigin(getOrigin());
	_hold->setOrigin(getOrigin());
	_label->setOrigin(getOrigin());
}


sf::Sprite& Button::getSprite() {
	return _normal->getSprite();
}

sf::Sprite& Button::getHoldSprite() {
	return _hold->getSprite();
}

void Button::setTexture(std::string texture) {
	_normal->setTexture(texture);
	computeSize();
}
void Button::setHoldSprite(std::string texture) {
	_hold->setTexture(texture);
	computeSize();
}

void Button::computeSize() {
	_normal->computeSize();
	_hold->computeSize();
	_label->computeSize();

	_size.x = std::max({ 
		_normal->getSize().x,
		_hold->getSize().x,
		_label->getSize().x
	});
	_size.y = std::max({ 
		_normal->getSize().y,
		_hold->getSize().y,
		_label->getSize().y 
	});
}

void Button::render(sf::RenderTarget&) {}

void Button::setStdString(const std::string& label) {
	_label->setStdString(label);
}
std::string Button::getString() const {
	return _label->getString();
}

bool Button::onClickBegan() {
	_normal->setVisible(false);
	_hold->setVisible(true);
	return true;
}
bool Button::onClickEnded() {
	_normal->setVisible(true);
	_hold->setVisible(false);
	return true;
}
bool Button::onClickGoing() {
	return false;
}

void Button::setSize(const Vector2f& size) {
	Widget::setSize(size);
	_normal->setSize(size);
	_label->setSize(size);
	_hold->setSize(size);
}
void Button::setOrigin(const Vector2f& origin) {
	Widget::setOrigin(origin);
	_normal->setOrigin(origin);
	_label->setOrigin(origin);
	_hold->setOrigin(origin);
}

Label& Button::getLabel() {
	return *_label;
}
