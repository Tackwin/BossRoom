#include "GUI/Shop.hpp"

#include <string>

#include "Game.hpp"
#include "Const.hpp"

#include "Math/Rectangle.hpp"

#include "Managers/AssetsManager.hpp"
#include "Managers/InputsManager.hpp"

#include "Gameplay/Player.hpp"

Shop::Shop() : 
	Widget(),
	_infoPanel(this)
{
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
	_quitPanel.setOrigin({ 1, 0 });
	_quitPanel.setPosition({
		_merchantPanel.getSize().x,
		0
	});
	_quitPanel.setParent(&_merchantPanel, 2);

	_infoPanel.setParent(&_merchantPanel, 2);

	_focusPanel.setSprite(sf::Sprite(AssetsManager::getTexture("panel_a")));
	_focusPanel.setSize({ _ItemPanel::PANEL_SIZE + 2, _ItemPanel::PANEL_SIZE + 2 });
	_focusPanel.setOriginAbs({ 3, 3});
	_focusPanel.setVisible(true);
	_focusPanel.getSprite().setColor(sf::Color(255, 255, 255, 100));
}

Shop::~Shop() {
	Widget::~Widget();
}

void Shop::addWeapon(Weapon* weapon) {
	constexpr u32 itemPerRow = 7;

	u32 size = _itemPanels.size();
	u32 nId = 0u;
	bool flag = false;
	do {
		flag = false;
		for (const auto& it : _itemPanels) {
			if (nId == it.first) {
				nId++;
				flag = true;
				break;
			}
		}
	} while (flag);

	auto item = std::make_unique<_ItemPanel>(this, weapon, nId);
	item->setPosition({ 
		(size % itemPerRow) * (_ItemPanel::PANEL_SIZE + 5.f) + 10,
		(size / itemPerRow) * (_ItemPanel::PANEL_SIZE + 5.f) + 30
	});
	item->setParent(&_merchantPanel, 1);

	_itemPanels[nId].swap(item);
}

void Shop::enter() {
	_in = true;
	_merchantPanel.setVisible(true);
}
void Shop::leave() {
	_in = false;
	_merchantPanel.setVisible(false);
}

bool Shop::onClickBegan() {
	printf("Shop\n");
	auto rect = Rectangle2f(
		{ _quitPanel.getGlobalPosition().x - 20, _quitPanel.getGlobalPosition().y },
		{ 20, 20 }
	);
	if (InputsManager::getMouseScreenPos().inRect(rect.pos, rect.size)) {
		leave();
		return true;
	}

	rect = Rectangle2f(
		{ _merchantPanel.getGlobalPosition().x, _merchantPanel.getGlobalPosition().y },
		{ _merchantPanel.getSize().x,			_merchantPanel.getSize().y * 0.05f }
	);
	if (InputsManager::getMouseScreenPos().inRect(rect.pos, rect.size)) {
		_dragging = true;
		_dragOffset = InputsManager::getMouseScreenPos() - getGlobalPosition();
		return true;
	}

	unFocus();
	return true;
}

bool Shop::onClickEnded(){
	if (_dragging) {
		_dragging = false;
		return true;
	}
	return false;
}

bool Shop::onClickGoing() {
	if (_dragging) {
		setPosition(InputsManager::getMouseScreenPos() - _dragOffset);
		return true;
	}
	return false;
}

void Shop::focusItem(u32 i) {
	focused = i;
	_focusPanel.setParent(&_itemPanels[i]->sprite, 1);
	
	_infoPanel.populateBy(_itemPanels[i]->weapon);
	_infoPanel.setVisible(true);
}
void Shop::unFocus() {
	focused = 0u;
	_focusPanel.emancipate();

	_infoPanel.setVisible(false);
}

bool Shop::isIn() const {
	return _in;
}


Shop::_ItemPanel::_ItemPanel(Shop * shop, Weapon* weapon, u32 id) :
	_shop(shop),
	weapon(weapon),
	_id(id)
{

	setSprite(sf::Sprite(AssetsManager::getTexture("panel_a")));
	getSprite().setColor({ 80, 80, 80 });
	setSize({ PANEL_SIZE, PANEL_SIZE });

	sprite.setSprite(weapon->getUiSprite());
	sprite.setSize({ PANEL_SIZE - 4, PANEL_SIZE - 4 });
	sprite.setPosition({ 2, 2 });
	sprite.setOnClick({
		std::bind(&Shop::_ItemPanel::onClickBegan, this),
		std::bind(&Shop::_ItemPanel::onClickEnded, this),
		std::bind(&Shop::_ItemPanel::onClickGoing, this)
	});

	addChild(&sprite, 1);
	addChild(&label, 2);
}

bool Shop::_ItemPanel::onClickBegan() {
	printf("ItemPanel\n");
	_shop->focusItem(_id);
	return true;
}
bool Shop::_ItemPanel::onClickEnded() {
	return false;
}
bool Shop::_ItemPanel::onClickGoing() {
	return false;
}

Shop::_InfoPanel::_InfoPanel(Shop* shop) :
	_shop(shop)
{
	setSprite(sf::Sprite(AssetsManager::getTexture("panel_a")));
	setOrigin({ 0, 0 });
	setPosition({ WIDTH * 0.4f - 25, 25.f });
	setSize({ WIDTH * 0.4f - 25, HEIGHT * 0.7f - 25 });
	getSprite().setColor(sf::Color(120, 110, 110));
	setVisible(false);

	auto& name = labels["name"];
	name.setString("Name: ");
	name.setFont("consola");
	name.setCharSize(25);
	name.setPosition({ 50, 25 });
	name.setParent(this, 1);

	auto& effect = labels["effect"];
	effect.setString("Effect: nique ta mère");
	effect.setFont("consola");
	effect.setCharSize(22);
	effect.setPosition({ 70, 50 });
	effect.setParent(this, 1);

	auto& cost = labels["cost"];
	cost.setString("cost: ");
	cost.setFont("consola");
	cost.setCharSize(22);
	cost.setPosition({ 70, 75 });
	cost.setParent(this, 1);

	auto& icon = panels["icon"];
	icon.setParent(this, 1);

	auto& buy = buttons["buy"];
	buy.getLabel().setFont("consola");
	buy.setSprite(sf::Sprite(AssetsManager::getTexture("panel_a")));
	buy.getSprite().setColor(sf::Color(150, 160, 150));
	buy.setSize({ 200, 65 });
	buy.setString("Buy");
	buy.setOrigin({ .5f, .5f });
	buy.setPosition({ getSize().x / 2, getSize().y * 0.75f });
	buy.setParent(this, 1);
	buy.setOnClick({
		std::bind(&Shop::_InfoPanel::onClickBegan, this),
		std::bind(&Shop::_InfoPanel::onClickEnded, this),
		std::bind(&Shop::_InfoPanel::onClickGoing, this)
	});
}
void Shop::_InfoPanel::populateBy(Weapon* weapon) {
	using namespace std::literals;

	auto& icon = panels["icon"];
	icon.setSprite(weapon->getUiSprite());
	icon.setSize({ 150, 150 });
	icon.setPosition({ 50, 120 });

	auto& name = labels["name"];
	name.setString("Name: "s + weapon->getName());

	auto& cost = labels["cost"];
	cost.setString("Cost: "s + std::to_string(weapon->getCost()));

	_weapon = weapon;
}

bool Shop::_InfoPanel::onClickBegan() {
	return true;
}
bool Shop::_InfoPanel::onClickEnded() {
	game->getPlayer()->swapWeapon(_weapon);
	return true;
}
bool Shop::_InfoPanel::onClickGoing() {
	return true;
}