#pragma once
#include <memory>
#include <functional>

#include <SFML/Graphics.hpp>

#include "Widget.hpp"

class Label : public Widget {
public:
	static constexpr auto NAME = "Label";

	Label();
	Label(nlohmann::json json) noexcept;

	void setFont(const std::string& fontKey);
	void setStdString(const std::string& message);
	void setSfString(const sf::String& message);
	void setTextColor(const sf::Color& color);
	void setCharSize(const u32& size);

	const sf::Text& getText() const;
	sf::Text& getText();

	std::string getString() const;

	void computeSize();

	virtual void render(sf::RenderTarget& target);

private:
	std::weak_ptr<const sf::Font> _font;
	sf::Text _text;
};
