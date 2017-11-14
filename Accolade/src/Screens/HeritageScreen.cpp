#include "Screens/HeritageScreen.hpp"

#include "Managers/AssetsManager.hpp"

#include "Common.hpp"

HeritageScreen::HeritageScreen(PlayerInfo& info) :
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
	setSize({ WIDTH * 0.33f, HEIGHT * 0.9f });
	setPosition({ 25.f, HEIGHT * 0.05f });
	getSprite().setColor(sf::Color(100, 200, 100));

	auto& name = labels["name"];
	name = MM::make_shared<Label>();
	name->setFont("consola");
	name->setCharSize(20u);
	name->setString(std::string("Name: ") + info.name);
	name->setOrigin({ 0.5f, 0.f });
	name->setPosition({ getSize().x * 0.5f, 20.f });
	name->setParent(this, 1);

	auto& infoPanel = panels["infoPanel"];
	infoPanel = MM::make_shared<Panel>();
	infoPanel->setSprite(sf::Sprite(AM::getTexture("panel_a")));
	infoPanel->setSize({
		getSize().x * .8f,
		(getSize().y - 40.f - name->getSize().y) * 0.5f
	});
	infoPanel->setOrigin({ 0.5f, 0.f });
	infoPanel->setPosition({
		name->getPosition().x,
		name->getPosition().y - 20.f
	});
	infoPanel->getSprite().setColor(sf::Color(100, 200, 100));
	infoPanel->setParent(this, 1);

	auto& speed = labels["speed"];
	speed = MM::make_shared<Label>();
	speed->setFont("consola");
	speed->setCharSize(15u);
	speed->setString(std::string("Speed: ") + std::to_string(info.speed));
	speed->setPosition({ 15.f, 10.f });
	speed->setParent(infoPanel.get(), 1);

	auto& specialSpeed = labels["specialSpeed"];
	specialSpeed = MM::make_shared<Label>();
	specialSpeed->setFont("consola");
	specialSpeed->setCharSize(15u);
	specialSpeed->setString(
		std::string("Speed: ") + std::to_string(info.specialSpeed)
	);
	specialSpeed->setOrigin({ 1.f, 0.f });
	specialSpeed->setPosition({ infoPanel->getSize().x - 15.f, 10.f });
	specialSpeed->setParent(infoPanel.get(), 1);

	auto& jumpHeight = labels["jumpHeight"];
	jumpHeight = MM::make_shared<Label>();
	jumpHeight->setFont("consola");
	jumpHeight->setCharSize(15u);
	jumpHeight->setString(
		std::string("Speed: ") + std::to_string(info.jumpHeight)
	);
	jumpHeight->setPosition({ 15.f, speed->getPosition().y + 10.f });
	jumpHeight->setParent(infoPanel.get(), 1);

	auto& viewPanel = panels["viewPanel"];
	viewPanel = MM::make_shared<Panel>();
	viewPanel->setSprite(sf::Sprite(AM::getTexture("panel_a")));
	viewPanel->setSize({
		getSize().x * .4f,
		(getSize().y - 40.f - name->getSize().y) * .4f
	});
	viewPanel->setOrigin({ 0.5f, 0.f });
	viewPanel->setPosition({
		name->getPosition().x,
		infoPanel->getPosition().y - 20.f
	});
	viewPanel->getSprite().setColor(sf::Color(80, 160, 80));
	viewPanel->setParent(this, 1);
}
