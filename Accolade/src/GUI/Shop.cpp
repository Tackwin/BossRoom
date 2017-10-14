#include "GUI/Shop.hpp"

#include "Const.hpp"

#include "Math/Rectangle.hpp"

#include "Managers/AssetsManager.hpp"
#include "Managers/InputsManager.hpp"

Shop::Shop() : Widget() {
	_merchantPanel.setSprite(sf::Sprite(AssetsManager::getTexture("panel_a")));
	_merchantPanel.setOrigin({ 0, 0 });
	_merchantPanel.setPosition({ 50, 50 });
	_merchantPanel.setSize({ WIDTH * 0.8f, HEIGHT * 0.7f });
	_merchantPanel.setVisible(false);
	_merchantPanel.setOnClick({
		std::bind(&Shop::onClickBegan, this),
		std::bind(&Shop::onClickEnded, this),
		std::bind(&Shop::onClickGoing, this)
	});
	_merchantPanel.setParent(this, 0);
	_merchantPanel.getSprite().setColor(sf::Color(100, 100, 100));

	_quitPanel.setSprite(sf::Sprite(AssetsManager::getTexture("quit")));
	_quitPanel.setSize({ 20, 20 });
	_quitPanel.setOrigin({ 0, 0 });
	_quitPanel.setPosition({
		_merchantPanel.getPosition().x + _merchantPanel.getSize().x,
		0
	});
	_quitPanel.setParent(&_merchantPanel, 2);
}

Shop::~Shop() {
	Widget::~Widget();
}

void Shop::addWeapon(const std::shared_ptr<Weapon>& weapon) {
	constexpr float itemPanelSize = 70;

	uint32_t size = _itemPanels.size();

	auto item = std::make_unique<_itemPanel>();
	item->setPosition({ 
		(size % 5) * (itemPanelSize + 5.f) + 10, 
		(size / 5) * (itemPanelSize + 5.f) + 30 
	});
	
	item->back.setSprite(sf::Sprite(AssetsManager::getTexture("panel_a")));
	item->back.getSprite().setColor({80, 80, 80});
	item->back.setSize({ itemPanelSize, itemPanelSize });

	item->sprite.setSprite(weapon->getUiSprite());
	item->sprite.setSize({ itemPanelSize - 4, itemPanelSize - 4 });

	item->setParent(&_merchantPanel, 1);

	_itemPanels.emplace_back();
	_itemPanels.back().swap(item);
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
	auto rect = Rectangle<2, float>(
		{ _merchantPanel.getGlobalPosition().x, _merchantPanel.getGlobalPosition().y },
		{ _merchantPanel.getSize().x,			_merchantPanel.getSize().y * 0.05f }
	);
	if (InputsManager::getMouseScreenPos().inRect(rect.pos, rect.size)) {
		_dragging = true;
		_dragOffset = InputsManager::getMouseScreenPos() - getGlobalPosition();
	}

	rect = Rectangle<2, float>(
		{ _merchantPanel.getGlobalPosition().x, _merchantPanel.getGlobalPosition().y + _merchantPanel.getSize().y - 20 },
		{ 20, 20 }
	);
	if (InputsManager::getMouseScreenPos().inRect(rect.pos, rect.size)) {
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