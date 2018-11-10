#pragma once
#include "Widget.hpp"

#include <optional>
#include <memory>
#include <unordered_map>

#include <SFML/Graphics.hpp>

#include "Time/Clock.hpp"

class Panel : public Widget {
public:

	static constexpr auto NAME = "Panel";

public:

	Panel() = default;
	Panel(nlohmann::json json) noexcept;

	void setTexture(std::string texture);
	void setTexture(sf::Texture& texture) noexcept;
	std::string getTexture() const noexcept;
	sf::Sprite& getSprite();

	float getSizeRatio() const noexcept;

	void computeSize();

	void render(sf::RenderTarget& target);

	bool isDraggable() const noexcept;
	bool isCollapsable() const noexcept;
	bool isResizable() const noexcept;
	bool isCollapsed() const noexcept;
	bool isClipper() const noexcept;

	void setDraggable(bool v) noexcept;
	void setCollapsable(bool v) noexcept;
	void setResizable(bool v) noexcept;
	void setClipper(bool v) noexcept;

	std::string getTitle() const noexcept;

	void setTitle(std::string str) noexcept;

	bool isInHeader(Vector2f p) const noexcept;

protected:

	void toggleCollapse() noexcept;

	bool onHoverGoing() noexcept;

	std::string _texture;
	std::string title;

	std::optional<Vector2f> deltaDrag;

	bool clipper{ false };
	bool collapsed{ false };
	bool draggable{ false };
	bool resizable{ false };
	bool collapsable{ false };

	int textureFullHeight{ 0 };
	float fullHeight{ NAN };

	std::optional<Clock> doubleClickClock{ std::nullopt };

	std::unordered_map<Widget*, bool> visibleStatebeforeCollapse;
	sf::Sprite _backSprite;

	Vector2f scroll{ 0, 0 };
};

