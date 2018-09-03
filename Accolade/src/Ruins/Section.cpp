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

#include "Utils/meta_algorithms.hpp"


SectionInfo SectionInfo::loadJson(const nlohmann::json& json) noexcept {
	SectionInfo info;
	auto load_vectors =
		[&](auto& x) -> std::enable_if_t<is_stl_container_v<decltype(x)>> {
			using T = holded_t<decltype(x)>;

			if (json.count(T::JSON_ID)) {
				x = load_json_vector<T>(json.at(T::JSON_ID));
			}
		};

	if (json.count("maxRect") != 0) {
		info.maxRectangle = Rectangle2f::loadJson(json.at("maxRect"));
	}

	if (json.count("startPos") != 0) {
		info.startPos = Vector2f::loadJson(json.at("startPos"));
	}

	if (json.count("viewSize") != 0) {
		info.viewSize = Vector2f::loadJson(json.at("viewSize"));
	}

	load_vectors(info.navigationPoints);
	load_vectors(info.navigationLinks);

	load_vectors(info.sourcesBoomerang);
	load_vectors(info.sourcesDirection);
	load_vectors(info.sourcesVaccum);
	load_vectors(info.sources);

	load_vectors(info.plateformes);

	load_vectors(info.distanceGuys);
	load_vectors(info.meleeGuys);
	load_vectors(info.slimes);
	load_vectors(info.flies);
	return info;
}

nlohmann::json SectionInfo::saveJson(SectionInfo info) noexcept {
	nlohmann::json json;
	auto save_vectors =
		[&](auto& x) -> std::enable_if_t<is_stl_container_v<decltype(x)>> {
			using T = holded_t<decltype(x)>;

			auto json_array = nlohmann::json::array();

			for (auto& e : x){
				json_array.push_back(T::saveJson(e));
			}
			json[T::JSON_ID] = json_array;
		};

	json["maxRect"] = Rectangle2f::saveJson(info.maxRectangle);
	json["startPos"] = Vector2f::saveJson(info.startPos);
	json["viewSize"] = Vector2f::saveJson(info.viewSize);

	save_vectors(info.plateformes);

	save_vectors(info.sourcesBoomerang);
	save_vectors(info.sourcesDirection);
	save_vectors(info.sourcesVaccum);
	save_vectors(info.sources);

	save_vectors(info.navigationPoints);
	save_vectors(info.navigationLinks);

	save_vectors(info.distanceGuys);
	save_vectors(info.meleeGuys);
	save_vectors(info.slimes);
	save_vectors(info.flies);

	return json;
}

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
	for (auto& source : _info.sourcesVaccum) {
		auto ptr = std::make_shared<SourceVaccum>(source);
		addSource(std::dynamic_pointer_cast<Source>(ptr));
	}
	for (auto& source : _info.sourcesDirection) {
		auto ptr = std::make_shared<SourceDirection>(source);
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
	for (auto& slime : _info.meleeGuys) {
		auto ptr = std::make_shared<MeleeGuy>(slime);
		addMeleeGuy(ptr);
	}
	for (auto& e : _info.flies) {
		auto ptr = std::make_shared<Fly>(e);
		addFly(ptr);
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
	for (auto& melee : meleeGuys_) {
		melee->enterSection(this);
		_world.addObject(melee);
	}
	for (auto& fly : flies) {
		fly->enterSection(this);
		_world.addObject(fly);
	}
	_world.addObject(_player);

	subscribeToEvents();
}

void Section::exit() noexcept {
	unsubscribeToEvents();

	for (auto& source : _sources) {
		source->exit();
	}
	for (auto& melee : meleeGuys_) {
		melee->leaveSection();
	}
	for (auto& fly : flies) {
		fly->leaveSection();
	}
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
	for (auto& melee : meleeGuys_) {
		melee->update(dt);
	}
	for (auto& spell : spells_) {
		spell->update(dt);
	}
	for (auto& fly : flies) {
		fly->update(dt);
	}

	removeDeadObject();

	pullZonesFromObjects();
	pullProjectilsFromObjects();

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

	for (auto& proj : _projectiles) {
		proj->render(target);
	}
	for (auto& source : _sources) {
		source->render(target);
	}
	for (auto& slime : _slimes) {
		slime->render(target);
	}
	for (auto& distance : distanceGuys_) {
		distance->render(target);
	}
	for (auto& melee : meleeGuys_) {
		melee->render(target);
	}
	for (auto& fly : flies) {
		fly->render(target);
	}
	for (auto& spell : spells_) {
		spell->render(target);
	}
	for (auto& zone : _zones) {
		zone->render(target);
	}
	for (auto& nav : _info.navigationPoints) {
		nav.pos.plot(target, nav.range, { 1.0, 1.0, 0.0, 0.5 }, {}, 0.f);
	}
	for (auto& link : _info.navigationLinks) {
		auto a = std::find_if(
			std::begin(_info.navigationPoints),
			std::end(_info.navigationPoints),
			[X = link.A](auto x) {return x.id == X; }
		);
		auto b = std::find_if(
			std::begin(_info.navigationPoints),
			std::end(_info.navigationPoints),
			[X = link.B](auto x) {return x.id == X; }
		);

		assert(a != b);
		assert(a != std::end(_info.navigationPoints));
		assert(b != std::end(_info.navigationPoints));

		Vector2f::renderLine(target, a->pos, b->pos, { 1.0, 0.0, 1.0, 0.5 });
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
void Section::addMeleeGuy(const std::shared_ptr<MeleeGuy>& ptr) noexcept {
	ptr->attachTo(getClosestNavigationPoint(ptr->pos));
	meleeGuys_.push_back(ptr);
}
void Section::addSlime(const std::shared_ptr<Slime>& slime) noexcept {
	slime->attachTo(getClosestNavigationPoint(slime->pos));
	_slimes.push_back(slime);
}
void Section::addFly(const std::shared_ptr<Fly>& ptr) noexcept {
	flies.push_back(ptr);
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
	auto removeDead = [&](auto& x) -> 
		std::enable_if_t<
			is_stl_container_v<decltype(x)> &&
			is_shared_ptr_v<holded_t<decltype(x)>> &&
			std::is_base_of_v<Removable, holded_t<holded_t<decltype(x)>>>
		>
	{
		for (size_t i = x.size(); i > 0; --i) 
			if (x[i - 1]->toRemove()) x.erase(std::begin(x) + i - 1);
	};

	removeDead(_projectiles);
	removeDead(_slimes);
	removeDead(_zones);
	removeDead(distanceGuys_);
	removeDead(meleeGuys_);
	removeDead(_sources);
	removeDead(spells_);
	removeDead(flies);
}

void Section::playerOnEnter(Object* object) noexcept {
	if (object->idMask[Object::STRUCTURE]) {
		auto box = ((Box*)object->collider.get())->getGlobalBoundingBox();

		if (_player->getBoundingBox().isOnTopOf(box)) _player->floored();
		if (_player->getBoundingBox().isOnBotOf(box)) _player->ceiled();
	}
}
void Section::playerOnExit(Object*) noexcept {

}

Vector2f Section::getPlayerPos() const noexcept {
	return _player->getPos();
}

std::shared_ptr<Player> Section::getPlayer() const noexcept {
	return _player;
}

void Section::pullZonesFromObjects() noexcept {
	for (auto& p : _player->getZonesToApply()) {
		_world.addObject(p);
		_zones.push_back(p);
	}
	_player->clearZonesToApply();

	for (auto& m : meleeGuys_) {
		for (auto& z : m->getZonesToApply()) {
			_world.addObject(z);
			_zones.push_back(z);
		}
		m->clearZones();
	}
}

void Section::pullProjectilsFromObjects() noexcept {
	for (auto& p : _player->getProtectilesToShoot()) {
		_world.addObject(p);
		_projectiles.push_back(p);
	}
	_player->clearProtectilesToShoot();
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

		auto iterate = [&](auto vec) -> std::enable_if_t<
			is_vector_v<decltype(vec)> &&
			is_shared_ptr_v<holded_t<decltype(vec)>> &&
			std::is_base_of_v<Object, holded_t<holded_t<decltype(vec)>>>
		> {
			for (auto& e : vec) {
				if (loopStart) {
					minPair = { dist(e), e };
					loopStart = false;
					continue;
				}
				if (minPair.first.length2() > dist(e).length2()) {
					minPair = { dist(e), e };
				}
			}
		};

		iterate(_slimes);
		iterate(distanceGuys_);
		iterate(meleeGuys_);
		iterate(flies);

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

sf::View Section::getCameraView() const noexcept {
	return _cameraView;
}

NavigationPointInfo Section::getNavigationPoint(UUID id) const noexcept {
	auto it = std::find_if(
		std::begin(_info.navigationPoints),
		std::end(_info.navigationPoints),
		[id](auto x) { return x.id == id; }
	);

	assert(it != std::end(_info.navigationPoints));

	return *it;
}

NavigationPointInfo Section::getNavigationPoint(UUID link, UUID current) const noexcept {
	auto it = std::find_if(
		std::begin(_info.navigationLinks),
		std::end(_info.navigationLinks),
		[link](auto x) { return x.id == link; }
	);

	assert(it != std::end(_info.navigationLinks));

	return getNavigationPoint(it->A == current ? it->B : it->A);
}

NavigationPointInfo Section::getNextNavigationPointFrom(
	UUID id, Vector2f to
) const noexcept {
	auto navPoint = getNavigationPoint(id);

	NavigationPointInfo closest;
	for (auto link : navPoint.links) {
		auto next = getNavigationPoint(link, navPoint.id);

		if (!closest.id) {
			closest = next;
			continue;
		}

		auto d1 = next.pos - to;
		auto d2 = closest.pos - to;

		if (d1.length2() < d2.length2()) {
			closest = next;
		}
	}

	return closest;
}

NavigationPointInfo Section::getClosestNavigationPoint(Vector2f p) const noexcept {
	assert(!_info.navigationPoints.empty());

	auto closest = _info.navigationPoints[0];
	for (auto n : _info.navigationPoints) {
		if ((closest.pos - p).length2() > (n.pos - p).length2()) closest = n;
	}
	return closest;
}

void Section::setFileName(std::filesystem::path fileName) noexcept {
	fileName_ = fileName;
}
std::filesystem::path Section::getFileName() const noexcept {
	return fileName_;
}

void Section::setFilepath(std::filesystem::path filepath) noexcept {
	filepath_ = filepath;
}
std::filesystem::path Section::getFilepath() const noexcept {
	return filepath_;
}
