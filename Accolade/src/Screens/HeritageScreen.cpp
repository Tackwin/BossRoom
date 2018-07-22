#include "HeritageScreen.hpp"

#include "./../Common.hpp"

#include "./../Managers/AssetsManager.hpp"

HeritageScreen::HeritageScreen(const PlayerInfo& info) :
	_info(info) {

}

void HeritageScreen::onEnter() {
	_root = Widget();
	_playerPanel.populateWith(_info);
	_root.addChild(&_playerPanel);
}
void HeritageScreen::onExit() {
}

void HeritageScreen::update(double) {
}

void HeritageScreen::render(sf::RenderTarget& target) {
	_root.propagateRender(target);
}

HeritageScreen::PlayerPanel::PlayerPanel() {
}

void HeritageScreen::PlayerPanel::populateWith(PlayerInfo& info_) {
	info = info_;

	setSprite(sf::Sprite(AM::getTexture("panel_a")));
	setSize({ (WIDTH - 50.f) * 0.33f, HEIGHT * 0.9f });
	setPosition({ 25.f, HEIGHT * 0.05f });
	getSprite().setColor(sf::Color(100, 200, 100));

	auto& name = labels["name"];
	name = std::make_shared<Label>();
	name->setFont("consola");
	name->setCharSize(20u);
	name->setStdString(std::string("Name: ") + info.name);
	name->setOrigin({ 0.5f, 0.f });
	name->setPosition({ getSize().x * 0.5f, 10.f });
	name->setParent(this, 2);

	auto& infoPanel = panels["infoPanel"];
	infoPanel = std::make_shared<Panel>();
	infoPanel->setSprite(sf::Sprite(AM::getTexture("panel_a")));
	infoPanel->setSize({
		getSize().x * .8f,
		(getSize().y - 40.f - name->getSize().y) * 0.5f
	});
	infoPanel->setOrigin({ 0.5f, 0.f });
	infoPanel->setPosition({
		name->getPosition().x,
		name->getPosition().y + name->getSize().y + 15.f
	});
	infoPanel->getSprite().setColor(sf::Color(80, 160, 80));
	infoPanel->setParent(this, 1);

	auto& speed = labels["speed"];
	speed = std::make_shared<Label>();
	speed->setFont("consola");
	speed->setCharSize(15u);
	speed->setStdString(std::string("Speed: ") + std::to_string((u32)info.speed));
	speed->setPosition({ 
		15.f - infoPanel->getSize().x / 2.f, 
		10.f 
	});
	speed->setParent(infoPanel.get(), 1);

	auto& specialSpeed = labels["specialSpeed"];
	specialSpeed = std::make_shared<Label>();
	specialSpeed->setFont("consola");
	specialSpeed->setCharSize(15u);
	specialSpeed->setStdString(
		std::string("Special speed: ") + std::to_string((u32)info.specialSpeed)
	);
	specialSpeed->setOrigin({ 1.f, 0.f });
	specialSpeed->setPosition({
		infoPanel->getSize().x / 2.f - 15, 
		10.f 
	});
	specialSpeed->setParent(infoPanel.get(), 2);

	auto& jumpHeight = labels["jumpHeight"];
	jumpHeight = std::make_shared<Label>();
	jumpHeight->setFont("consola");
	jumpHeight->setCharSize(15u);
	jumpHeight->setStdString(
		std::string("Jump: ") + std::to_string((u32)info.jumpHeight)
	);
	jumpHeight->setPosition({ 
		15.f - infoPanel->getSize().x / 2.f, 
		speed->getPosition().y + speed->getSize().y + 5
	});
	jumpHeight->setParent(infoPanel.get(), 1);

	auto& viewPanel = panels["viewPanel"];
	viewPanel = std::make_shared<Panel>();
	viewPanel->setSprite(sf::Sprite(AM::getTexture("panel_a")));
	viewPanel->setSize({
		getSize().x * .4f,
		(getSize().y - 40.f - name->getSize().y) * .45f
	});
	viewPanel->setOrigin({ 0.5f, 0.f });
	viewPanel->setPosition({
		name->getPosition().x,
		infoPanel->getPosition().y + infoPanel->getSize().y + 20.f
	});
	viewPanel->getSprite().setColor(sf::Color(80, 160, 80));
	viewPanel->setParent(this, 1);

	auto& playerViewPanel = panels["playerViewPanel"];
	playerViewPanel = std::make_shared<Panel>();
	playerViewPanel->setSprite(sf::Sprite(AM::getTexture(info.sprite)));
	playerViewPanel->setOrigin({ 0.5f, 1.f });
	playerViewPanel->setPosition({
		0,
		viewPanel->getSize().y - 5
	});
	playerViewPanel->setParent(viewPanel.get(), 1);
}
