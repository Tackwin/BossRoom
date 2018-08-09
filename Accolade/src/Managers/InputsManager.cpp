#include "InputsManager.hpp"

#include "EventManager.hpp"

#include "./../Common.hpp"

sf::Uint32 InputsManager::textEntered;

bool InputsManager::wasKeyJustPressed = false;
bool InputsManager::wasKeyJustReleased = false;
int InputsManager::nKeyPressed = 0;

float InputsManager::lastScroll = 0.f;

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

sf::Keyboard::Key InputsManager::getLastKeyPressed() noexcept {
	return lastKey;
}

bool InputsManager::isTextJustEntered() noexcept {
	return textEntered != 0;
}
sf::Uint32 InputsManager::getTextEntered() noexcept {
	return textEntered;
}

bool InputsManager::isLastSequence(std::initializer_list<sf::Keyboard::Key> keys) noexcept {
	unsigned i{ 0u };
	if (keys.size() != currentSequence.size()) return false;
	for (auto k : keys) {
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
	if (keys.size() != currentSequence.size()) return false;
	for (auto k : keys) {
		last = k;
		if (k != currentSequence[i++])
			return false;
	}
	return isKeyJustPressed(last) && keys.size() == currentSequence.size();
}

bool InputsManager::isKeyJustPressed() noexcept {
	return wasKeyJustPressed;
}
bool InputsManager::isKeyJustReleased() noexcept {
	return wasKeyJustReleased;
}

int InputsManager::countKeyPressed() noexcept {
	return nKeyPressed;
}

bool InputsManager::isKeyPressed() noexcept {
	return nKeyPressed != 0;
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
float InputsManager::getLastScroll() noexcept {
	return lastScroll;
}

void InputsManager::update(sf::RenderWindow &window) {
	wasKeyJustPressed = false;
	wasKeyJustReleased = false;
	nKeyPressed = std::max(nKeyPressed, 0);
	
	textEntered = 0;
	lastScroll = 0;

	memset(keyJustPressed   , 0, sizeof(keyJustPressed));
	memset(keyJustReleased  , 0, sizeof(keyJustReleased));
	memset(mouseJustPressed , 0, sizeof(mouseJustPressed));
	memset(mouseJustReleased, 0, sizeof(mouseJustReleased));

	sf::Event event;
	while(window.pollEvent(event)) {
		if(event.type == sf::Event::Closed)
			window.close();
		if(event.type == sf::Event::KeyPressed) {
			nKeyPressed++;
			wasKeyJustPressed = true;
			if (event.key.code == sf::Keyboard::Unknown)
				continue;

			keyJustPressed[event.key.code] = true;
			keyPressed[event.key.code] = true;

			EventManager::fire("keyPressed", { event.key.code });
			lastKey = event.key.code;

			if (
				InputsManager::isKeyJustPressed(sf::Keyboard::LControl) ||
				InputsManager::isKeyJustPressed(sf::Keyboard::RControl) ||
				InputsManager::isKeyJustPressed(sf::Keyboard::LShift) ||
				InputsManager::isKeyJustPressed(sf::Keyboard::RShift)
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
			nKeyPressed--;
			wasKeyJustReleased = false;
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
		if (event.type == sf::Event::TextEntered) {
			textEntered = event.text.unicode;
		}
		if (event.type == sf::Event::MouseWheelScrolled) {
			lastScroll = event.mouseWheelScroll.delta;
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
