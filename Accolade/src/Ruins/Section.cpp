#include "Section.hpp"

#include "Game.hpp"

#include "Managers/AssetsManager.hpp"
#include "Managers/InputsManager.hpp"
#include "Managers/EventManager.hpp"
#include "Structure/Plateforme.hpp"

#include "Gameplay/Projectile.hpp"
#include "Gameplay/Zone.hpp"

#include "Utils/json_algorithms.hpp"

#include "Gameplay/Magic/Sources/SourceTarget.hpp"

Section::Section(SectionInfo info) noexcept : _info(info) {
	_cameraView.setCenter(_info.maxRectangle.fitUpRatio(RATIO).center());
	_cameraView.setSize(_info.viewSize.fitUpRatio(RATIO));

	for (auto& plateforme : _info.plateformes) {
		auto ptr = std::make_shared<Plateforme>(plateforme);
		addStructure(std::shared_ptr<Structure>(ptr));
	}

	for (auto& source : _info.sources) {
		auto ptr = std::make_shared<Source>(source);
		addSource(ptr);
	}
	for (auto& source : _info.sourcesBoomerang) {
		auto ptr = std::make_shared<SourceTarget>(source);
		addSource(std::dynamic_pointer_cast<Source>(ptr));
	}

	for (auto& slime : _info.slimes) {
		auto ptr = std::make_shared<Slime>(slime);
		addSlime(ptr);
	}

	for (auto& slime : _info.distanceGuys) {
		auto ptr = std::make_shared<DistanceGuy>(slime);
		addDistanceGuy(ptr);
	}
}

void Section::enter() noexcept {
	_player = std::make_shared<Player>(C::game->getPlayerInfo());
	_player->setPos(_info.startPos);
	_player->enter(this);
	_player->collider->onEnter =
		std::bind(&Section::playerOnEnter, this, std::placeholders::_1);

	if (_player->isEquiped()) {
		_player->swapWeapon(_player->getPlayerInfo().weapon.value());
	}

	for (auto& structure : _structures) {
		_world.addObject(structure);
	}
	for (auto& slime : _slimes) {
		slime->enterSection(this);
		_world.addObject(slime);
	}
	for (auto& source : _sources) {
		source->enter(this);
		_world.addObject(source);
	}
	for (auto& distance: distanceGuys_) {
		distance->enterSection(this);
		_world.addObject(distance);
	}
	_world.addObject(_player);

	subscribeToEvents();
}

void Section::exit() noexcept {
	unsubscribeToEvents();

	for (auto& source : _sources) {
		source->exit();
	}

	_zones.clear();
	_projectiles.clear();
	_slimes.clear();
	distanceGuys_.clear();
	spells_.clear();
	_world.purge();
}

void Section::update(double dt) noexcept {
	auto mouse = IM::getMousePosInView(_cameraView);

	_player->setFacingDir((float)(mouse - _player->getPos()).angleX());
	_player->update(dt);
	for (auto& projectile : _projectiles) {
		projectile->update(dt);
	}
	for (auto& spatial : _structures) {
		spatial->update(dt);
	}
	for (auto& source : _sources) {
		source->update(dt);
	}
	for (auto& slime : _slimes) {
		slime->update(dt);
	}
	for (auto& distance : distanceGuys_) {
		distance->update(dt);
	}
	for (auto& spell : spells_) {
		spell->update(dt);
	}

	removeDeadObject();

	pullPlayerObjects();

	_world.updateInc(dt, 1);

	targetEnnemy_ = getTargetEnnemyFromMouse();
}
void Section::render(sf::RenderTarget& target) const noexcept {
	auto oldView = target.getView();

	Rectangle2f cameraRect;
	cameraRect.size = _info.viewSize;
	cameraRect.setCenter(_player->getPos());
	cameraRect = cameraRect.restrictIn(_info.maxRectangle);

	_cameraView.setCenter(cameraRect.center());
	_cameraView.setSize(cameraRect.size);
	target.setView(_cameraView);

	_player->render(target);

	for (auto& source : _sources) {
		source->render(target);
	}
	for (auto& slime : _slimes) {
		slime->render(target);
	}
	for (auto& distance : distanceGuys_) {
		distance->render(target);
	}
	for (auto& spell : spells_) {
		spell->render(target);
	}

	renderCrossOnTarget(target);

	target.setView(oldView);
}

void Section::renderDebug(sf::RenderTarget& target) const noexcept {
	auto oldView = target.getView();
	target.setView(_cameraView);

	for (auto& spatial : _structures) {
		spatial->renderDebug(target);
	}
	_world.render(target);

	target.setView(oldView);
}

void Section::addSpell(const std::shared_ptr<Spell>& ptr) noexcept {
	spells_.push_back(ptr);
	if (auto objPtr = std::dynamic_pointer_cast<Object>(ptr); objPtr) {
		_world.addObject(objPtr);
	}
}
void Section::addStructure(const std::shared_ptr<Structure>& ptr) noexcept {
	_structures.push_back(ptr);
}
void Section::addSource(const std::shared_ptr<Source>& ptr) noexcept {
	_sources.push_back(ptr);
}
void Section::addDistanceGuy(const std::shared_ptr<DistanceGuy>& ptr) noexcept {
	distanceGuys_.push_back(ptr);
}

void Section::subscribeToEvents() noexcept {
	_keyPressedEvent = EventManager::subscribe("keyPressed",
		[&](EventManager::EventCallbackParameter args) -> void {
		auto key = std::any_cast<sf::Keyboard::Key>(*args.begin());

		_player->keyPressed(key);
	}
	);
	_keyReleasedEvent = EventManager::subscribe("keyReleased",
		[&](EventManager::EventCallbackParameter args) -> void {
		auto key = std::any_cast<sf::Keyboard::Key>(*args.begin());
		_player->keyReleased(key);
	}
	);
}

void Section::unsubscribeToEvents() noexcept {
	EventManager::unSubscribe("keyPressed", _keyPressedEvent);
	EventManager::unSubscribe("keyReleased", _keyReleasedEvent);
}

void Section::removeDeadObject() noexcept {
	for (int i = (int)_projectiles.size() - 1; i >= 0; --i) {
		if (_projectiles[i]->toRemove()) {
			_world.delObject(_projectiles[i]->uuid);
			_projectiles.erase(_projectiles.begin() + i);
		}
	}
	for (int i = (int)_slimes.size() - 1; i >= 0; --i) {
		if (_slimes[i]->toRemove()) {
			_world.delObject(_slimes[i]->uuid);
			_slimes.erase(_slimes.begin() + i);
		}
	}
	for (int i = (int)distanceGuys_.size() - 1; i >= 0; --i) {
		if (distanceGuys_[i]->toRemove()) {
			_world.delObject(distanceGuys_[i]->uuid);
			distanceGuys_.erase(distanceGuys_.begin() + i);
		}
	}
	for (int i = (int)_sources.size() - 1; i >= 0; --i) {
		if (_sources[i]->toRemove()) {
			_world.delObject(_sources[i]->uuid);
			_sources.erase(_sources.begin() + i);
		}
	}
	for (int i = (int)spells_.size() - 1; i >= 0; --i) {
		if (spells_[i]->toRemove()) {
			if (auto objPtr = std::dynamic_pointer_cast<Object>(spells_[i]); objPtr) {
				_world.delObject(spells_[i]->getUuid());
			}
			spells_.erase(spells_.begin() + i);
		}
	}
}

void Section::playerOnEnter(Object* object) noexcept {
	if (object->idMask[Object::STRUCTURE]) {
		_player->floored();
		_player->clearKnockBack();
	}
}
void Section::playerOnExit(Object*) noexcept {

}


SectionInfo SectionInfo::loadJson(const nlohmann::json& json) noexcept {
	SectionInfo info;

	if (json.count("maxRect") != 0) {
		info.maxRectangle.x = json.at("maxRect").get<std::vector<float>>()[0];
		info.maxRectangle.y = json.at("maxRect").get<std::vector<float>>()[1];
		info.maxRectangle.w = json.at("maxRect").get<std::vector<float>>()[2];
		info.maxRectangle.h = json.at("maxRect").get<std::vector<float>>()[3];
	}
	
	if (json.count("plateformes") != 0) {
		info.plateformes = load_json_vector<PlateformeInfo>(json.at("plateformes"));
	}

	if (json.count("sources") != 0) {
		info.sources = load_json_vector<SourceInfo>(json.at("sources"));
	}
	if (json.count(SourceTargetInfo::JSON_ID) != 0) {
		info.sourcesBoomerang = 
			load_json_vector<SourceTargetInfo>(json.at(SourceTargetInfo::JSON_ID)
		);
	}

	if (json.count("startPos") != 0) {
		info.startPos.x = json.at("startPos").get<std::vector<float>>()[0];
		info.startPos.y = json.at("startPos").get<std::vector<float>>()[1];
	}

	if (json.count("viewSize") != 0) {
		info.viewSize = Vector2f::loadJson(json.at("viewSize"));
	}

	if (auto slimes = json.find("slimes"); slimes != json.end() && !slimes->is_null()) {
		for (auto slime : slimes->get<nlohmann::json::array_t>()) {
			info.slimes.push_back(SlimeInfo::loadJson(slime));
		}
	}
	if (auto distance = json.find("distanceGuys");
		distance != json.end() && !distance->is_null()
	) {
		for (auto slime : distance->get<nlohmann::json::array_t>()) {
			info.distanceGuys.push_back(DistanceGuyInfo::loadJson(slime));
		}
	}

	return info;
}

nlohmann::json SectionInfo::saveJson(SectionInfo info) noexcept {
	nlohmann::json json;
	nlohmann::json slimeArray = nlohmann::json::array();
	nlohmann::json distanceArray = nlohmann::json::array();
	nlohmann::json sourceArray = nlohmann::json::array();
	nlohmann::json sourceBoomerangArray = nlohmann::json::array();
	nlohmann::json plateformeArray = nlohmann::json::array();

	for (auto& slime : info.slimes) {
		slimeArray.push_back(SlimeInfo::saveJson(slime));
	}
	for (auto& distance : info.distanceGuys) {
		distanceArray.push_back(DistanceGuyInfo::saveJson(distance));
	}
	for (auto& source : info.sources) {
		sourceArray.push_back(SourceInfo::saveJson(source));
	}
	for (auto& plateforme : info.plateformes) {
		plateformeArray.push_back(PlateformeInfo::saveJson(plateforme));
	}

	for (auto& source : info.sourcesBoomerang) {
		sourceBoomerangArray.push_back(SourceTargetInfo::saveJson(source));
	}

	json["maxRect"]						= Rectangle2f::saveJson(info.maxRectangle);
	json["startPos"]					= Vector2f::saveJson(info.startPos);
	json["viewSize"]					= Vector2f::saveJson(info.viewSize);
	json["plateformes"]					= plateformeArray;
	json[SourceTargetInfo::JSON_ID]		= sourceBoomerangArray;
	json["sources"]						= sourceArray;
	json["distanceGuys"]				= distanceArray;
	json["slimes"]						= slimeArray;

	return json;
}

Vector2f Section::getPlayerPos() const noexcept {
	return _player->getPos();
}

std::shared_ptr<Player> Section::getPlayer() const noexcept {
	return _player;
}

void Section::addSlime(const std::shared_ptr<Slime>& slime) noexcept {
	_slimes.push_back(slime);
}

void Section::pullPlayerObjects() {
	for (auto& p : _player->getProtectilesToShoot()) {
		_world.addObject(p);
		_projectiles.push_back(p);
	}
	_player->clearProtectilesToShoot();

	for (auto& p : _player->getZonesToApply()) {
		_world.addObject(p);
		_zones.push_back(p);
	}
	_player->clearZonesToApply();
}

SectionInfo Section::getInfo() const noexcept {
	return _info;
}

std::shared_ptr<Object> Section::getTargetEnnemyFromMouse() noexcept {
	if (_slimes.empty() && distanceGuys_.empty()) return {};
	if (_player->isBoomerangSpellAvailable()) {
		bool loopStart = true;

		auto dist = [&](const std::shared_ptr<Object>& obj) {
			return obj->pos - IM::getMousePosInView(_cameraView);
		};

		std::pair<Vector2f, std::shared_ptr<Object>> minPair;

		for (auto& slime : _slimes) {
			if (loopStart) {
				minPair = { dist(slime), slime };
				loopStart = false;
				continue;
			}
			if (minPair.first.length2() > dist(slime).length2()) {
				minPair = { dist(slime), slime };
			}
		}
		for (auto& distance : distanceGuys_) {
			if (loopStart) {
				minPair = { dist(distance), distance };
				loopStart = false;
				continue;
			}
			if (minPair.first.length2() > dist(distance).length2()) {
				minPair = { dist(distance), distance };
			}
		}

		return minPair.second;
	}
	return {};
}

void Section::renderCrossOnTarget(sf::RenderTarget& target) const noexcept {
	if (!targetEnnemy_) return;

	sf::CircleShape shape{ 1.f };
	shape.setOrigin(shape.getRadius(), shape.getRadius());
	shape.setOutlineThickness(0.1f);
	shape.setFillColor(Vector4f{ 0., 0., 0., 0. });
	shape.setOutlineColor(Vector4f{ 1.0, 0.0, 1.0, 1.0 });
	shape.setPosition(targetEnnemy_->pos);

	target.draw(shape);
}

std::weak_ptr<Object> Section::getObject(UUID id) const noexcept {
	return _world.getObject(id);
}
