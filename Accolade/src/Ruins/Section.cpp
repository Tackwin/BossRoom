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
		[&](auto& x) mutable -> std::enable_if_t<is_stl_container_v<decltype(x)>> {
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

	load_vectors(info.semiPlateformes);
	load_vectors(info.plateformes);
	load_vectors(info.portals);

	load_vectors(info.first_bosses);
	load_vectors(info.distanceGuys);
	load_vectors(info.meleeGuys);
	load_vectors(info.slimes);
	load_vectors(info.flies);

	if (json.count("meta_data") != 0) info.meta_data = json["meta_data"];

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

	save_vectors(info.semiPlateformes);
	save_vectors(info.plateformes);
	save_vectors(info.portals);

	save_vectors(info.sourcesBoomerang);
	save_vectors(info.sourcesDirection);
	save_vectors(info.sourcesVaccum);
	save_vectors(info.sources);

	save_vectors(info.navigationPoints);
	save_vectors(info.navigationLinks);

	save_vectors(info.first_bosses);
	save_vectors(info.distanceGuys);
	save_vectors(info.meleeGuys);
	save_vectors(info.slimes);
	save_vectors(info.flies);

	json["meta_data"] = info.meta_data;

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

	auto add_ennemy = [&](const auto& x) { for (const auto& i : x) addEnnemy(i); };

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
	// >Add ennemy.
	add_ennemy(_info.first_bosses);

	aim_sprite = sf::Sprite{ AM::getTexture("aim_texture") };
	aim_sprite.setOrigin(
		aim_sprite.getTextureRect().width * 0.5f,
		aim_sprite.getTextureRect().height * 0.5f
	);

	inventory = std::make_unique<Inventory>();
}

void Section::enter() noexcept {
	time_since_entered = 0.f;

	_player = std::make_shared<Player>(C::game->getPlayerInfo());
	_player->setPos(_info.startPos);
	_player->enter(this);

	if (_player->isEquiped()) {
		_player->swapWeapon(_player->getPlayerInfo().weapon.value());
	}

	for (auto& structure : _structures) {
		_world.addObject(structure);
	}
	for (auto& source : _sources) {
		source->enter(this);
		_world.addObject(source);
	}

	auto enter_ennemies = [&](const auto& x) {
		for (auto& element : x) {
			element->enterSection(this);
			_world.addObject(element);
		}
	};
	enter_ennemies(first_bosses);
	enter_ennemies(distanceGuys_);
	enter_ennemies(meleeGuys_);
	enter_ennemies(_slimes);
	enter_ennemies(flies);

	_world.addObject(_player);

	subscribeToEvents();
}

void Section::exit() noexcept {
	unsubscribeToEvents();

	_player->leave();

	for (auto& source : _sources) {
		source->exit();
	}

	auto leave_ennemies = [&](const auto& x) {
		for (auto& element : x) {
			element->leaveSection();
		}
	};
	leave_ennemies(meleeGuys_);
	leave_ennemies(flies);
	leave_ennemies(first_bosses);

	_world.purge();
}

void Section::update(double dt) noexcept {
	time_since_entered += dt;

	auto mouse = IM::getMousePosInView(_cameraView);

	_player->setFacingDir((float)(mouse - _player->getPos()).angleX());
	_player->update(dt);

	auto update_vector = [&](auto& x) {
		for (auto& element : x) {
			element->update(dt);
		}
	};

	update_vector(_projectiles);
	update_vector(_structures);
	update_vector(_sources);
	update_vector(_slimes);
	update_vector(distanceGuys_);
	update_vector(meleeGuys_);
	update_vector(spells_);
	update_vector(flies);
	update_vector(first_bosses);
	update_vector(loots);

	removeDeadObject();
	if (!aiming) updateAimAnimation(dt);

	pullZonesFromObjects();
	pullProjectilsFromObjects();

	_world.updateInc(dt, 1);

	targetEnnemy_ = getTargetEnnemyFromMouse();
}
void Section::updateAimAnimation(double dt) noexcept {
	auto delta = getPlayerPos() - aim_sprite.getPosition();
	// hooke's law
	delta = delta.normalize() * delta.length() * elastic_force_aim_sprite;

	aim_sprite.move(delta * dt);
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

	auto render_vector = [&](const auto& x) {
		for (auto& element : x) { element->render(target); }
	};

	render_vector(_projectiles);
	render_vector(_sources);
	render_vector(_slimes);
	render_vector(distanceGuys_);
	render_vector(meleeGuys_);
	render_vector(spells_);
	render_vector(flies);
	render_vector(_zones);
	render_vector(first_bosses);
	render_vector(loots);

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
	// I'm testing the more routine style of Blow, Casey, Delix etc...
	// In truth i guess it's a c style
	// i'm guessing it will be a great help if i ever want to switch to AOS - SOA
	for (const auto& p : _info.portals) ::render(p, target);

	renderCrossOnTarget(target);
	if (aiming) target.draw(aim_sprite);

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
void Section::addEnnemy(const FirstBossInfo& info) noexcept {
	first_bosses.push_back(std::make_shared<FirstBoss>(info));
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
			if (x[i - 1]->toRemove()) {
				if constexpr (std::is_base_of_v<Object, holded_t<holded_t<decltype(x)>>>) {
					_world.delObject(x[i - 1]->uuid);
				}
				x.erase(std::begin(x) + i - 1);
			}
	};
	removeDead(distanceGuys_);
	removeDead(_projectiles);
	removeDead(first_bosses);
	removeDead(meleeGuys_);
	removeDead(_sources);
	removeDead(spells_);
	removeDead(_slimes);
	removeDead(_zones);
	removeDead(flies);
	removeDead(loots);
}

void Section::playerOnExit(Object*) noexcept {

}

Vector2f Section::getPlayerPos() const noexcept {
	return _player->getPos();
}
void Section::setPlayerPos(Vector2f p) const noexcept {
	return _player->setPos(p);
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

	for (auto& first_boss : first_bosses) {
		for (auto& p : first_boss->getProtectilesToShoot()) {
			_world.addObject(p);
			_projectiles.push_back(p);
		}
		first_boss->clearProtectilesToShoot();
	}
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
		iterate(first_bosses);

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

NavigationPointInfo
Section::getNextNavigationPointFrom(UUID id, Vector2f to) const noexcept {
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

const std::vector<PortalInfo>& Section::getAllPortals() const noexcept {
	return _info.portals;
}
const PortalInfo& Section::getPortalInSpot(size_t spot) const noexcept {
	auto it = std::find_if(
		std::begin(_info.portals),
		std::end(_info.portals),
		[spot](auto x) { return x.spot == spot; }
	);
	assert(it != std::end(_info.portals));
	return *it;
}
const PortalInfo& Section::getPortal(UUID id) const noexcept {
	auto it = std::find_if(
		std::begin(_info.portals),
		std::end(_info.portals),
		[id](const PortalInfo& x) { return x.id == id; }
	);
	assert(it != std::end(_info.portals));
	return *it;
}

double Section::getTimeSinceEntered() const noexcept {
	return time_since_entered;
}

void Section::startAimAnimation() noexcept {
	auto player_pos = getPlayerPos();
	aim_sprite.setPosition(
		player_pos +
		Vector2f::createUnitVector(unitaryRng(RD) * 2 * PIf) * (unitaryRng(RD) * 50 + 100)
	);

	aiming = true;
}

void Section::stopAimAnimation() noexcept {
	aiming = false;
}

void Section::spawnLoot(std::shared_ptr<Loot> loot) noexcept {
	_world.addObject(loot);
	loot->setLootable(true);
	loots.push_back(loot);
}

std::optional<RayCollision> Section::ray_cast(
	const Rayf& ray, std::bitset<Object::SIZE> mask
) const noexcept {
	return _world.ray_cast(ray, mask);
}
