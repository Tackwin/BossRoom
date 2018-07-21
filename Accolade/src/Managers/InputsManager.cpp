#include "InputsManager.hpp"

#include "EventManager.hpp"

#include "./../Common.hpp"

bool InputsManager::keyPressed[sf::Keyboard::KeyCount];
bool InputsManager::keyJustPressed[sf::Keyboard::KeyCount];
bool InputsManager::keyJustReleased[sf::Keyboard::KeyCount];

bool InputsManager::mousePressed[sf::Mouse::ButtonCount];
bool InputsManager::mouseJustPressed[sf::Mouse::ButtonCount];
bool InputsManager::mouseJustReleased[sf::Mouse::ButtonCount];

Vector2u InputsManager::windowsSize;
Vector2f InputsManager::mouseWorldPos;
Vector2f InputsManager::mouseScreenPos;
Vector2f InputsManager::relativeMouseScreenPos;

std::vector<sf::Keyboard::Key> InputsManager::currentSequence;

sf::Keyboard::Key InputsManager::lastKey;

InputsManager::InputsManager() {
}
InputsManager::~InputsManager() {
}

bool InputsManager::isLastSequence(std::initializer_list<sf::Keyboard::Key> keys) noexcept {
	unsigned i{ 0u };
	for (auto k : keys) {
		if (i >= currentSequence.size()) return false;
		if (k != currentSequence[i++])
			return false;
	}
	return keys.size() == currentSequence.size();
}
bool InputsManager::isLastSequenceJustFinished(
	std::initializer_list<sf::Keyboard::Key> keys
) noexcept {
	unsigned i = { 0u };
	sf::Keyboard::Key last;
	for (auto k : keys) {
		last = k;
		if (i >= currentSequence.size()) return false;
		if (k != currentSequence[i++])
			return false;
	}
	return isKeyJustPressed(last) && keys.size() == currentSequence.size();
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

Vector2f InputsManager::getMousePosInView(const sf::View& view) {
	auto screenPos = getMouseScreenPos();

	const auto& viewScope = view.getViewport();
	auto viewPort = sf::IntRect(
		(int)std::ceil(windowsSize.x * viewScope.left),
		(int)std::ceil(windowsSize.y * viewScope.top),
		(int)std::ceil(windowsSize.x * viewScope.width),
		(int)std::ceil(windowsSize.y * viewScope.height)
	);

	Vector2f normalized;

	normalized.x = -1.f + 2.f * (screenPos.x - viewPort.left) / viewPort.width;
	normalized.y = +1.f - 2.f * (screenPos.y - viewPort.top) / viewPort.height;

	return view.getInverseTransform().transformPoint(normalized);
}
Vector2f InputsManager::getMouseScreenPos() {
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

			EventManager::fire("keyPressed", { event.key.code });
			lastKey = event.key.code;

			if (
				InputsManager::isKeyJustPressed(sf::Keyboard::LControl) ||
				InputsManager::isKeyJustPressed(sf::Keyboard::RControl)
			) {
				currentSequence.clear();
				currentSequence.push_back(lastKey);
			}
			else if (currentSequence.size() > 2) {
				currentSequence.clear();
			}
			else if (!currentSequence.empty()) {
				currentSequence.push_back(lastKey);
			}
		}
		if(event.type == sf::Event::KeyReleased) {
			if (event.key.code == sf::Keyboard::Unknown)
				continue;

			keyJustReleased[event.key.code] = true;
			keyPressed[event.key.code] = false;
			
			if (event.key.code == sf::Keyboard::LControl ||
				event.key.code == sf::Keyboard::RControl
			) {
				currentSequence.clear();
			}

			EventManager::fire("keyReleased", { event.key.code });
		}
		if(event.type == sf::Event::MouseButtonPressed) {
			mouseJustPressed[event.mouseButton.button] = true;
			mousePressed[event.mouseButton.button] = true;
			
			EventManager::fire("mousePressed", { event.mouseButton.button });
		}
		if(event.type == sf::Event::MouseButtonReleased) {
			mouseJustReleased[event.mouseButton.button] = true;
			mousePressed[event.mouseButton.button] = false;
			
			EventManager::fire("mouseReleased", { event.mouseButton.button });
		}
	}
	for (i32 i = 0u; i < sf::Keyboard::KeyCount; ++i) {
		if (keyPressed[i])
			EventManager::fire("keyPress", { i });
	}
	for (i32 i = 0u; i < sf::Mouse::ButtonCount; ++i) {
		if (mousePressed[i])
			EventManager::fire("mousePress", { i });
	}

	mouseScreenPos = sf::Mouse::getPosition(window);
	
	mouseWorldPos = window.mapPixelToCoords({
		(i32)mouseScreenPos.x,
		(i32)mouseScreenPos.y
	});

	windowsSize = window.getSize();
}
