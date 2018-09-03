#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

#include "./../Math/Vector.hpp"

class InputsManager {
private:
	InputsManager();
	~InputsManager();

private:
	static bool wasKeyJustPressed;
	static bool wasKeyJustReleased;
	static int nKeyPressed;

	static float lastScroll;

	static sf::Uint32 textEntered;

	static bool keyPressed[sf::Keyboard::KeyCount];
	static bool keyJustPressed[sf::Keyboard::KeyCount];
	static bool keyJustReleased[sf::Keyboard::KeyCount];
	
	static bool mousePressed[sf::Mouse::ButtonCount];
	static bool mouseJustPressed[sf::Mouse::ButtonCount];
	static bool mouseJustReleased[sf::Mouse::ButtonCount];

	static Vector2f mouseWorldPos;
	static Vector2f mouseScreenPos;
	static Vector2f relativeMouseScreenPos;

	static Vector2u windowsSize;

	static constexpr auto MAX_SEQUENCE = 4;
	static std::vector<sf::Keyboard::Key> currentSequence;

	static sf::Keyboard::Key lastKey;

public:
	static std::string nameOfKey(sf::Keyboard::Key) noexcept;

	static void update(sf::RenderWindow &window);

	static sf::Keyboard::Key getLastKeyPressed() noexcept;

	static bool isTextJustEntered() noexcept;
	static sf::Uint32 getTextEntered() noexcept;

	static bool isLastSequence(std::initializer_list<sf::Keyboard::Key> keys) noexcept;
	static bool isLastSequenceJustFinished(
		std::initializer_list<sf::Keyboard::Key> keys
	) noexcept;

	static int countKeyPressed() noexcept;

	static bool isKeyPressed() noexcept;
	static bool isKeyPressed(const int &key) {
		return isKeyPressed(static_cast<sf::Keyboard::Key>(key));
	};
	static bool isKeyJustPressed() noexcept;
	static bool isKeyPressed(const sf::Keyboard::Key &key);

	static bool isKeyJustPressed(const int &key) {
		return isKeyJustPressed(static_cast<sf::Keyboard::Key>(key));
	};
	static bool isKeyJustPressed(const sf::Keyboard::Key &key);
	
	static bool isKeyJustReleased() noexcept;
	static bool isKeyJustReleased(const int &key) {
		return isKeyJustReleased(static_cast<sf::Keyboard::Key>(key));
	};
	static bool isKeyJustReleased(const sf::Keyboard::Key &key);

	static bool isMousePressed(const int &button) {
		return isMousePressed(static_cast<sf::Mouse::Button>(button));
	};
	static bool isMousePressed(const sf::Mouse::Button &button);

	static bool isMouseJustPressed(const int &button) {
		return isMouseJustPressed(static_cast<sf::Mouse::Button>(button));
	};
	static bool isMouseJustPressed(const sf::Mouse::Button &button);
	
	static bool isMouseJustReleased(const int &button) {
		return isMouseJustReleased(static_cast<sf::Mouse::Button>(button));
	};
	static bool isMouseJustReleased(const sf::Mouse::Button &button);

	static float getLastScroll() noexcept;

	static Vector2f getMousePosInView(const sf::View& view);
	static Vector2f getMouseScreenPos();
};

using IM = InputsManager;