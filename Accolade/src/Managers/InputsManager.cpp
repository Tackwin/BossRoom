#include "Managers/InputsManager.hpp"

bool InputsManager::keyPressed[sf::Keyboard::KeyCount];
bool InputsManager::keyJustPressed[sf::Keyboard::KeyCount];
bool InputsManager::keyJustReleased[sf::Keyboard::KeyCount];

bool InputsManager::mousePressed[sf::Mouse::ButtonCount];
bool InputsManager::mouseJustPressed[sf::Mouse::ButtonCount];
bool InputsManager::mouseJustReleased[sf::Mouse::ButtonCount];

Vector2f InputsManager::mouseWorldPos;
Vector2f InputsManager::mouseScreenPos;

InputsManager::InputsManager() {
}
InputsManager::~InputsManager() {
}

bool InputsManager::isKeyPressed(const sf::Keyboard::Key &key) {
	return keyPressed[key];
}
bool InputsManager::isKeyJustPressed(const sf::Keyboard::Key &key) {
	return keyJustPressed[key];
}
bool InputsManager::isKeyJustReleased(const sf::Keyboard::Key &key) {
	return keyJustReleased[key];
}

bool InputsManager::isMousePressed(const sf::Mouse::Button &button) {
	return mousePressed[button];
}
bool InputsManager::isMouseJustPressed(const sf::Mouse::Button &button) {
	return mouseJustPressed[button];
}
bool InputsManager::isMouseJustReleased(const sf::Mouse::Button &button) {
	return mouseJustReleased[button];
}

const Vector2f& InputsManager::getMouseWorldPos() {
	return mouseWorldPos;
}
const Vector2f& InputsManager::getMouseScreenPos() {
	return mouseScreenPos;
}

void InputsManager::update(sf::RenderWindow &window) {
	for(auto &b : keyJustPressed) {
		b = false;
	}
	for(auto &b : keyJustReleased) {
		b = false;
	}
	for(auto &b : mouseJustPressed) {
		b = false;
	}
	for(auto &b : mouseJustReleased) {
		b = false;
	}

	sf::Event event;
	while(window.pollEvent(event)) {
		if(event.type == sf::Event::Closed)
			window.close();
		if(event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::Unknown)
				continue;

			keyJustPressed[event.key.code] = true;
			keyPressed[event.key.code] = true;
		}
		if(event.type == sf::Event::KeyReleased) {
			if (event.key.code == sf::Keyboard::Unknown)
				continue;

			keyJustReleased[event.key.code] = true;
			keyPressed[event.key.code] = false;
		}
		if(event.type == sf::Event::MouseButtonPressed) {
			mouseJustPressed[event.mouseButton.button] = true;
			mousePressed[event.mouseButton.button] = true;
		}
		if(event.type == sf::Event::MouseButtonReleased) {
			mouseJustReleased[event.mouseButton.button] = true;
			mousePressed[event.mouseButton.button] = false;
		}
	}
	mouseScreenPos.x = (float)sf::Mouse::getPosition(window).x;
	mouseScreenPos.y = (float)sf::Mouse::getPosition(window).y;
	
	mouseWorldPos.x = window.mapPixelToCoords({ (int)mouseScreenPos.x, (int)mouseScreenPos.y }).x;
	mouseWorldPos.y = window.mapPixelToCoords({ (int)mouseScreenPos.x, (int)mouseScreenPos.y }).y;
}