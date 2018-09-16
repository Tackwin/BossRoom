#include "Switcher.hpp"

#include "Managers/AssetsManager.hpp"

SpriteSwitcher::SpriteSwitcher(nlohmann::json json) noexcept : Widget(json) {

	if (const auto& it = json.find("panels"); it != json.end()) {
		const auto& panels = it->get<std::vector<nlohmann::json::object_t>>();

		for (const auto& p : panels) {
			auto ptr = new Panel{ p };
			ptr->setOrigin(getOrigin());
			ptr->setVisible(false);
			ptr->setSize(getSize().fitDownRatio(ptr->getSizeRatio()));
			ptr->setPosition({ (getSize().x - ptr->getSize().x) / 2.f, 0.f });
			addChild(ptr, 1);
			_panels.push_back(std::move(ptr));
		}

		if (!_panels.empty()) _panels.front()->setVisible(true);
	}

	if (auto it = json.find("backColor"); it != json.end()) {
		_backColor = Vector4f::loadJson(*it);
	}

	if (auto it = json.find("label"); it != json.end()) {
		_label = new Label{ *it };
		addChild(_label, 2);
		setCurrentLabel();
	}
	
	_backPanel = new Panel;
	_backPanel->setOrigin(getOrigin());
	_backPanel->setTexture("panel_a");
	_backPanel->setSize(getSize());
	_backPanel->getSprite().setColor(_backColor);

	addChild(_backPanel, 0);
}

void SpriteSwitcher::add(std::string texture, std::string name) noexcept {
	if (name == "") name = texture;

	auto ptr = new Panel{};
	ptr->setTexture(texture);
	ptr->setOrigin(getOrigin());
	ptr->setName(name);
	ptr->setSize(getSize().fitDownRatio(ptr->getSizeRatio()));
	addChild(ptr);
	_panels.push_back(std::move(ptr));
}

void SpriteSwitcher::del(std::string name) noexcept {
	auto it = std::find_if(
		std::begin(_panels),
		std::end(_panels),
		[name](const auto& ptr) -> bool {
			return ptr->getName() == name;
		}
	);

	if (it != std::end(_panels)) {
		_panels.erase(it);
	}
}

const Panel* SpriteSwitcher::getCurrentPanel() const noexcept {
	return _panels.front();
}

void SpriteSwitcher::left(size_t n) noexcept {
	if (_panels.empty()) return;

	_panels.front()->setVisible(false);
	std::rotate(std::begin(_panels), std::begin(_panels) + n, std::end(_panels));
	_panels.front()->setVisible(true);

	setCurrentLabel();
}
void SpriteSwitcher::right(size_t n) noexcept {
	if (_panels.empty()) return;
	
	_panels.front()->setVisible(false);
	std::rotate(std::rbegin(_panels), std::rbegin(_panels) + n, std::rend(_panels));
	_panels.front()->setVisible(true);

	setCurrentLabel();
}

void SpriteSwitcher::render(sf::RenderTarget& target) noexcept {
	sf::CircleShape marker{ 2.f };
	marker.setOrigin(marker.getRadius(), marker.getRadius());
	marker.setPosition(getGlobalPosition());
	target.draw(marker);
}

void SpriteSwitcher::setCurrentLabel() noexcept {
	if (_label) {
		_label->setStdString(getCurrentPanel()->getName());
	}
}