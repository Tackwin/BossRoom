#pragma once

#include "Widget.hpp"
#include "Panel.hpp"
#include "Label.hpp"

#include "3rd/json.hpp"

#include <vector>

class SpriteSwitcher : public Widget {

public:

	static constexpr auto NAME = "SpriteSwitcher";

public:

	SpriteSwitcher() = default;
	SpriteSwitcher(nlohmann::json json) noexcept;

	void add(std::string texture, std::string name = "") noexcept;
	void del(std::string name) noexcept;

	const Panel* getCurrentPanel() const noexcept;

	void left(size_t n = 1) noexcept;
	void right(size_t n = 1) noexcept;

	virtual void render(sf::RenderTarget& target) noexcept;

private:

	void setCurrentLabel() noexcept;

	Vector4f _backColor;

	std::vector<std::unique_ptr<Panel>> _panels;
	std::unique_ptr<Panel> _backPanel;
	std::unique_ptr<Label> _label;
};