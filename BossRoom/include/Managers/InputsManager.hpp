#pragma once
#include <SFML/Graphics.hpp>

#include "Math/Vector2.hpp"

class InputsManager {
private:
	InputsManager();
	~InputsManager();

private:
	static bool keyPressed[sf::Keyboard::KeyCount];
	static bool keyJustPressed[sf::Keyboard::KeyCount];
	static bool keyJustReleased[sf::Keyboard::KeyCount];
	
	static bool mousePressed[sf::Mouse::ButtonCount];
	static bool mouseJustPressed[sf::Mouse::ButtonCount];
	static bool mouseJustReleased[sf::Mouse::ButtonCount];

	static Vector2 mouseWorldPos;
	static Vector2 mouseScreenPos;

public:
	static void update(sf::RenderWindow &window);

	static bool isKeyPressed(const int &key) {return isKeyPressed(static_cast<sf::Keyboard::Key>(key));};
	static bool isKeyPressed(const sf::Keyboard::Key &key);

	static bool isKeyJustPressed(const int &key) {return isKeyJustPressed(static_cast<sf::Keyboard::Key>(key));};
	static bool isKeyJustPressed(const sf::Keyboard::Key &key);
	
	static bool isKeyJustReleased(const int &key) {return isKeyJustReleased(static_cast<sf::Keyboard::Key>(key));};
	static bool isKeyJustReleased(const sf::Keyboard::Key &key);

	static bool isMousePressed(const int &button) {return isMousePressed(static_cast<sf::Mouse::Button>(button));};
	static bool isMousePressed(const sf::Mouse::Button &button);

	static bool isMouseJustPressed(const int &button) {return isMouseJustPressed(static_cast<sf::Mouse::Button>(button));};
	static bool isMouseJustPressed(const sf::Mouse::Button &button);
	
	static bool isMouseJustReleased(const int &button) {return isMouseJustReleased(static_cast<sf::Mouse::Button>(button));};
	static bool isMouseJustReleased(const sf::Mouse::Button &button);

	static const Vector2& getMouseWorldPos();
	static const Vector2& getMouseScreenPos();
};

