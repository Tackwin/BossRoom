#include "Instance.hpp"
#include "Common.hpp"

#include "Math/Segment.hpp"
#include "Math/algorithms.hpp"

#include "Managers/AssetsManager.hpp"

#include "Utils/container_algorithm.hpp"

#include "Order/OrderedPair.hpp"

#define SAVE(x, y) json[#x] = y(info.x);
nlohmann::json InstanceInfo::saveJson(const InstanceInfo& info) noexcept {
	nlohmann::json json;
	SAVE(nSamples, );
	//SAVE(roofSamples, );
	//SAVE(roughTowers, );
	return json;

}

#undef SAVE
#define LOAD(x, y) if (auto i = json.find(#x); i != json.end()) info.x = y(*i);

InstanceInfo InstanceInfo::loadJson(const nlohmann::json& json) noexcept {
	InstanceInfo info;
	LOAD(nSamples, );
	//LOAD(roofSamples, );
	//LOAD(roughTowers, );
	return info;
}
#undef LOAD

Instance::Instance(const InstanceInfo& info) noexcept : info(info) {}

void Instance::generateMaze(size_t width, size_t height) noexcept {
	std::unordered_map<OrderedPair<size_t>, std::string> rooms;
	rooms[OrderedPair<size_t>{0, 1}] = "left_to_up";
	rooms[OrderedPair<size_t>{0, 2}] = "left_to_right";
	rooms[OrderedPair<size_t>{0, 3}] = "left_to_down";
	rooms[OrderedPair<size_t>{1, 2}] = "up_to_right";
	rooms[OrderedPair<size_t>{1, 3}] = "up_to_down";
	rooms[OrderedPair<size_t>{2, 3}] = "right_to_down";
}

void Instance::generateGrid(size_t n) noexcept {
	SectionInfo model = SectionInfo::loadJson(AM::getJson("1x1"));

	for (size_t y = 0; y < n; ++y) {
		for (size_t x = 0; x < n; ++x) {
			SectionInfo sec = model;
			for (auto& p : sec.portals) p.id = {};

			if (x > 0) {
				SectionInfo& left = info.sections[x - 1 + y * n];

				PortalInfo& portal_A = *std::find_if(
					std::begin(sec.portals),
					std::end(sec.portals),
					[](auto x) {
						return x.spot == 0;
					}
				);
				PortalInfo& portal_B = *std::find_if(
					std::begin(left.portals),
					std::end(left.portals),
					[](auto x) {
						return x.spot == 2;
					}
				);

				portal_A.tp_to = portal_B.id;
				portal_B.tp_to = portal_A.id;
			}
			if (y > 0) {
				SectionInfo& down = info.sections[x + (y - 1) * n];

				PortalInfo& portal_A = *std::find_if(
					std::begin(sec.portals),
					std::end(sec.portals),
					[](auto x) {
						return x.spot == 3;
					}
				);
				PortalInfo& portal_B = *std::find_if(
					std::begin(down.portals),
					std::end(down.portals),
					[](auto x) {
						return x.spot == 1;
					}
				);

				portal_A.tp_to = portal_B.id;
				portal_B.tp_to = portal_A.id;
			}

			info.sections.push_back(sec);
		}
	}
}

void Instance::startAt(size_t p) noexcept {
	current_section = entity_store.make<Section>(info.sections[p]);
	sections.push_back(current_section);
}

void Instance::hardSetCurrentSection(const SectionInfo& sec) noexcept {
	auto it = std::find(std::begin(sections), std::end(sections), current_section);
	entity_store.destroy(current_section);
	current_section = entity_store.make<Section>(sec);
	*it = current_section;
}

void Instance::update(double dt) noexcept {
	auto section = entity_store.get(current_section);
	assert(section);
	auto playerPos = section->getPlayerPos();

	section->update(dt);

	std::optional<PortalInfo> opt;

	if (section->getTimeSinceEntered() > 5.0) {
		for (auto& p : section->getAllPortals()) {
			if (min_dist2(p.frontier, playerPos) < 1) {
				opt = p;
				break;
			}
		}

		if (auto p = *opt; opt) {
			section->exit();
			auto next = getNextSectionIfInstantiated(p.spot);
			
			PortalInfo portal_to_enter_from;
			if (next) {
				current_section = *next;

				section = entity_store.get(current_section);
				portal_to_enter_from = section->getPortal(p.tp_to);
			}
			else {
				auto nextToInstantiate =
					find<SectionInfo>(
						info.sections,
						[&portal_to_enter_from, id = p.tp_to](const SectionInfo& i) {
							auto it = std::find_if(
								std::begin(i.portals),
								std::end(i.portals),
								[id](PortalInfo i) {
									return i.id == id;
								}
							);
							if (it != std::end(i.portals)) {
								portal_to_enter_from = *it;
								return true;
							}
							return false;
						}
					);

				assert(nextToInstantiate);
				sections.push_back(entity_store.make<Section>(*nextToInstantiate));
				current_section = sections.back();
			}
			
			section = entity_store.get(current_section);
			section->enter();
			section->setPlayerPos(portal_to_enter_from.drop_pos);
		}
	}
}

void Instance::render(sf::RenderTarget& target) noexcept {
	auto section = entity_store.get(current_section);
	assert(section);

	section->render(target);
}
void Instance::renderDebug(sf::RenderTarget& target) noexcept {
	auto section = entity_store.get(current_section);
	assert(section);

	section->renderDebug(target);
}

Section& Instance::getCurrentSection() noexcept {
	return *entity_store.get(current_section);
}
const Section& Instance::getCurrentSection() const noexcept {
	return *entity_store.get(current_section.to_const());
}

void Instance::runAlgo(sf::RenderWindow& window) noexcept {
	v = sf::View{ {WIDTH / 2.f, HEIGHT / 2.f}, {(float)WIDTH, (float)HEIGHT} };

	auto old = window.getView();
	samples(window); if (rerun) return;
	smoothSamples(window); if (rerun) return;
	selectSamples(window); if (rerun) return;
	generateRoughTowers(window); if (rerun) return;
	// spaceTowers(window); if (rerun) return;
	// generateTowersBridge(window); if (rerun) return;
	window.setView(old);
}

void Instance::samples(sf::RenderWindow& window) noexcept {
	size_t n_current = 0;
	size_t i = 0;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::D)
					v.move(10, 0);
				if (event.key.code == sf::Keyboard::Q)
					v.move(-10, 0);
				if (event.key.code == sf::Keyboard::Z)
					v.move(0, 10);
				if (event.key.code == sf::Keyboard::S)
					v.move(0, -10);
				if (event.key.code == sf::Keyboard::Return) {
					rerun = true;
					return;
				}
				if (event.key.code == sf::Keyboard::Space && n_current >= info.nSamples) {
					return;
				}
			}
		}

		window.setView(v);
		window.clear();

		if (n_current < info.nSamples * 5) {
			if (i % 20 == 0) {
				info.roofSamples.push_back(
					{ unitaryRng(RD) * 70 + 50, unitaryRng(RD) * 700 }
				);
				n_current++;
			}
		}

		float x_pos = 0;
		for (const auto& s : info.roofSamples) {
			Segment2f{ {x_pos, s.y}, {x_pos + s.x, s.y } }.render(window, { 0, 1, 0, 1 });
			x_pos += s.x;
		}

		window.display();
		i++;
	}
}

void Instance::smoothSamples(sf::RenderWindow& window) noexcept {
	size_t i = 0;
	size_t n_frames = 0;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::D)
					v.move(10, 0);
				if (event.key.code == sf::Keyboard::Q)
					v.move(-10, 0);
				if (event.key.code == sf::Keyboard::Z)
					v.move(0, 10);
				if (event.key.code == sf::Keyboard::S)
					v.move(0, -10);
				if (event.key.code == sf::Keyboard::Return) {
					rerun = true;
					return;
				}
				if (
					event.key.code == sf::Keyboard::Space &&
					i + 1 >= info.roofSamples.size()
				) {
					return;
				}
			}
		}

		window.setView(v);
		window.clear();

		if (n_frames % 20 == 0 && i + 1 < info.roofSamples.size()) {
			auto& it = info.roofSamples[i].y;
			auto& it1 = info.roofSamples[i + 1].y;

			auto dist = std::abs(it - it1);

			constexpr auto A = 4;
			constexpr auto P = -0.43f;

			auto f = [=](auto x) {
				return (float)std::fmin(A / std::powf(x, P), x);
			};

			auto comeClose = f(dist) / 2;

			it += (std::signbit(it1 - it) ? -1 : 1) * comeClose;
			it1 += (std::signbit(it - it1) ? -1 : 1) * comeClose;

			i += 2;
		}

		float x_pos = 0;
		for (const auto& s : info.roofSamples) {
			Segment2f{ {x_pos, s.y}, {x_pos + s.x, s.y } }.render(window, { 0, 1, 0, 1 });
			x_pos += s.x;
		}

		window.display();
		n_frames++;
	}
}

void Instance::selectSamples(sf::RenderWindow& window) noexcept {
	size_t n_frames = 0;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::D)
					v.move(10, 0);
				if (event.key.code == sf::Keyboard::Q)
					v.move(-10, 0);
				if (event.key.code == sf::Keyboard::Z)
					v.move(0, 10);
				if (event.key.code == sf::Keyboard::S)
					v.move(0, -10);
				if (event.key.code == sf::Keyboard::Return) {
					rerun = true;
					return;
				}
				if (
					event.key.code == sf::Keyboard::Space &&
					info.roofSamples.size() >= info.nSamples
				) {
					return;
				}
			}
		}

		window.setView(v);
		window.clear();

		if (n_frames % 20 == 0 && info.nSamples < info.roofSamples.size()) {
			int choosen = int(unitaryRng(RD) * info.roofSamples.size());
			info.roofSamples.erase(std::begin(info.roofSamples) + choosen);
		}

		float x_pos = 0;
		for (const auto& s : info.roofSamples) {
			Segment2f{ {x_pos, s.y}, {x_pos + s.x, s.y } }.render(window, { 0, 1, 0, 1 });
			x_pos += s.x;
		}

		window.display();
		n_frames++;
	}
}

void Instance::generateRoughTowers(sf::RenderWindow& window) noexcept {
	size_t n_frames = 0;
	size_t i = 0;
	float sum_x = 0;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::D)
					v.move(10, 0);
				if (event.key.code == sf::Keyboard::Q)
					v.move(-10, 0);
				if (event.key.code == sf::Keyboard::Z)
					v.move(0, 10);
				if (event.key.code == sf::Keyboard::S)
					v.move(0, -10);
				if (event.key.code == sf::Keyboard::Return) {
					rerun = true;
					return;
				}
				if (
					event.key.code == sf::Keyboard::Space &&
					info.roughTowers.size() >= info.nSamples
					) {
					return;
				}
			}
		}

		window.setView(v);
		window.clear();

		if (n_frames % 20 == 0 && info.nSamples > info.roughTowers.size()) {
			int n_features = (int)(unitaryRng(RD) * 3 + 1);

			std::vector<Rectangle2f> rectangles{};
			Rectangle2f core{
				{sum_x, 0},
				{info.roofSamples[i].x, info.roofSamples[i].y}
			};
			rectangles.push_back(core);

			for (int j = 0; j < n_features; ++j) {
				float margin_x = unitaryRng(RD) * (info.roofSamples[i].x / 10.f) + 5;
				float height = unitaryRng(RD) * (50 - 20) + 20;
				float pos_y = unitaryRng(RD) * (info.roofSamples[i].y * 0.8f) + 50;

				if (pos_y >= info.roofSamples[i].y) continue;

				rectangles.push_back({
					{sum_x - margin_x, pos_y - height / 2},
					{info.roofSamples[i].x + margin_x * 2, height}
					});
			}
			sum_x += info.roofSamples[i].x;
			i++;
			info.roughTowers.push_back(rectangles);
		}

		for (auto& t : info.roughTowers) {
			for (auto& r : t) {
				r.render(window, { 0, 0, 0, 0 }, { 1, 1, 1, 1 }, 0.05f);
			}
		}

		window.display();
		n_frames++;
	}
}

void Instance::spaceTowers(sf::RenderWindow& window) noexcept {
	size_t n_frames = 0;
	size_t i = 0;

	size_t median_height = 0;
	std::vector<size_t> indexes(info.roughTowers.size());
	std::generate(std::begin(indexes), std::end(indexes), [i = 0] () mutable {
		return i++;
	});
	std::sort(std::begin(indexes), std::end(indexes), [&](size_t a, size_t b) {
		return info.roughTowers[a][0].h < info.roughTowers[b][0].h;
	});

	median_height = indexes.size() / 2;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::D)
					v.move(10, 0);
				if (event.key.code == sf::Keyboard::Q)
					v.move(-10, 0);
				if (event.key.code == sf::Keyboard::Z)
					v.move(0, 10);
				if (event.key.code == sf::Keyboard::S)
					v.move(0, -10);
				if (event.key.code == sf::Keyboard::Return) {
					rerun = true;
					return;
				}
				if (
					event.key.code == sf::Keyboard::Space &&
					i >= info.nSamples
				) {
					return;
				}
			}
		}

		window.setView(v);
		window.clear();

		if (n_frames % 20 == 0 && indexes.size() > 2 * i) {
			info.roughTowers.emplace(
				std::begin(info.roughTowers) + indexes[i * 2 + 1],
				info.roughTowers[indexes[i + median_height]]
			);
			info.roughTowers.erase(std::begin(info.roughTowers) + indexes[i + median_height] + 1);
			i++;
		}

		for (auto& t : info.roughTowers) {
			for (auto& r : t) {
				r.render(window, { 0, 0, 0, 0 }, { 1, 1, 1, 1 }, 0.05f);
			}
		}

		window.display();
		n_frames++;
	}
}

void Instance::generateTowersBridge(sf::RenderWindow& window) noexcept {
	size_t n_frames = 0;
	size_t i = 0;
	float sum_x = 0;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::D)
					v.move(10, 0);
				if (event.key.code == sf::Keyboard::Q)
					v.move(-10, 0);
				if (event.key.code == sf::Keyboard::Z)
					v.move(0, 10);
				if (event.key.code == sf::Keyboard::S)
					v.move(0, -10);
				if (event.key.code == sf::Keyboard::Return) {
					rerun = true;
					return;
				}
				if (
					event.key.code == sf::Keyboard::Space &&
					info.roughTowers.size() >= info.nSamples
				) {
					return;
				}
			}
		}

		window.setView(v);
		window.clear();

		if (n_frames % 20 == 0 && info.nSamples > info.roughTowers.size()) {
			int n_features = (int)(unitaryRng(RD) * 3 + 1);

			std::vector<Rectangle2f> rectangles{};
			Rectangle2f core{
				{sum_x, 0},
				{info.roofSamples[i].x, info.roofSamples[i].y}
			};
			rectangles.push_back(core);

			for (int j = 0; j < n_features; ++j) {
				float margin_x = unitaryRng(RD) * (info.roofSamples[i].x / 10) + 5;
				float height = unitaryRng(RD) * (50 - 20) + 20;
				float pos_y = unitaryRng(RD) * (info.roofSamples[i].y * 0.8f) + 50;

				if (pos_y >= info.roofSamples[i].y) continue;

				rectangles.push_back({
					{sum_x - margin_x, pos_y - height / 2},
					{info.roofSamples[i].x + margin_x * 2, height}
					});
			}
			sum_x += info.roofSamples[i].x;
			i++;
			info.roughTowers.push_back(rectangles);
		}

		for (auto& t : info.roughTowers) {
			for (auto& r : t) {
				r.render(window, { 0, 0, 0, 0 }, { 1, 1, 1, 1 }, 0.05f);
			}
		}

		window.display();
		n_frames++;
	}
}

std::optional<Eid<Section>>
Instance::getNextSectionIfInstantiated(size_t dir) const noexcept {
	auto& s = getCurrentSection();
	auto tp_to_it = std::find_if(
		std::begin(s.getAllPortals()),
		std::end(s.getAllPortals()),
		[dir](PortalInfo i) {
			return i.spot == dir;
		}
	);
	assert(tp_to_it != std::end(s.getAllPortals()));
	auto tp_to = tp_to_it->tp_to;

	auto it = std::find_if(
		std::begin(sections),
		std::end(sections),
		[&](Eid<Section> ptr) {
			auto& portals = entity_store.get(ptr.to_const())->getAllPortals();
			return std::count_if(
				std::begin(portals),
				std::end(portals),
				[tp_to](PortalInfo i) {
					return i.id == tp_to;
				}
			) > 0;
		}
	);

	if (it != std::end(sections))	return *it;
	else							return {};
}

size_t Instance::complementary_dir(size_t dir) const noexcept {
	return	dir == 0 ? 2 :
			dir == 1 ? 3 :
			dir == 2 ? 0 :
			dir == 3 ? 1 : assert("???"), 0;
}
