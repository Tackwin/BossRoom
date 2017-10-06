#include <Global/Const.hpp>
#include <Managers/AssetsManager.hpp>
#include <Managers/InputsManager.hpp>
#include <Math/Rectangle.hpp>
#include <GUI/Shop.hpp>

Shop::Shop() : Widget() {
	_merchantPanel.setSprite(sf::Sprite(AssetsManager::getTexture("panel_a")));
	_merchantPanel.setOrigin(Vector2::ZERO);
	_merchantPanel.setPosition({ 50, 50 });
	_merchantPanel.setSize({ WIDTH * 0.8f, HEIGHT * 0.7f });
	_merchantPanel.setVisible(false);
	_merchantPanel.setOnClick({
		std::bind(&Shop::onClickBegan, this),
		std::bind(&Shop::onClickEnded, this),
		std::bind(&Shop::onClickGoing, this)
	});
	_merchantPanel.setParent(this);
}

Shop::~Shop() {
	Widget::~Widget();
}

void Shop::addWeapon(const std::shared_ptr<Weapon>& weapon) {
	uint32_t size = _itemPanels.size();

	_itemPanels.push_back(Panel());
	_itemPanels.back().setSprite(weapon->getUiSprite());
	_itemPanels.back().setSize({ 30, 30 });
	_itemPanels.back().setPosition({ size * 35.f + 10, (size % 5) * 35.f + 10 });
	_itemPanels.back().setParent(&_merchantPanel);
}

void Shop::enter() {
	_in = true;
	_merchantPanel.setVisible(true);
}
void Shop::leave() {
	_in = false;
	_merchantPanel.setVisible(false);
}

void Shop::onClickBegan() {
	auto rect = Rectangle(_merchantPanel.getGlobalPosition().x, _merchantPanel.getGlobalPosition().y, _merchantPanel.getSize().x, _merchantPanel.getSize().y * 0.05f);
	if (rect.isInside(InputsManager::getMouseScreenPos())) {
		_dragging = true;
		_dragOffset = InputsManager::getMouseScreenPos() - getGlobalPosition();
	}

	rect = Rectangle(_merchantPanel.getGlobalPosition().x, _merchantPanel.getGlobalPosition().y + _merchantPanel.getSize().y - 20, 20, 20);
	if (rect.isInside(InputsManager::getMouseScreenPos())) {
		leave();
	}
}
void Shop::onClickEnded(){
	_dragging = false;
}
void Shop::onClickGoing() {
	if (_dragging) {
		setPosition(InputsManager::getMouseScreenPos() - _dragOffset);
	}
}