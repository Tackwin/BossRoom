#include "Instance.hpp"

#include <iostream>
#include "Common.hpp"

#include "Math/Segment.hpp"
#include "Math/algorithms.hpp"
#include "Containers/Graph.hpp"

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

std::unordered_map<std::set<size_t>, std::vector<std::string>>
load_maps_by_directions() noexcept {
	const auto& json = AM::getJson("definitions");
	const auto& rooms_by_opennings = json.at("rooms_by_opennings");

	std::unordered_map<std::set<size_t>, std::vector<std::string>> results;
	
	for (const auto& element : rooms_by_opennings.get<nlohmann::json::array_t>()) {
		auto room = element.at("room").get<std::string>();
		auto directions = element.at("directions").get<std::set<size_t>>();

		results[directions].push_back(room);
	}

	return results;
}


Instance::Instance(const InstanceInfo& info) noexcept : info(info) {}

OrderedPair<size_t> pick_random_dir_to_dir() noexcept {
	return { (size_t)(unitaryRng(RD) * 4), (size_t)(unitaryRng(RD) * 4) };
}

std::pair<size_t, size_t>
next_pos_from_to(std::pair<size_t, size_t> pos, size_t dir) noexcept
{
	switch (dir)
	{
	case 0: return { pos.first - 1, pos.second };
	case 1: return { pos.first, pos.second + 1 };
	case 2: return { pos.first + 1, pos.second };
	case 3: return { pos.first, pos.second - 1 };
	default: return assert(true), pos;
	}
}
size_t portal_spot_to_look_for_from_dir(size_t dir) noexcept {
	return (dir + 2) % 4;
}

InstanceInfo InstanceInfo::generateMaze(size_t width, size_t height) noexcept {
	assert(width != 0);
	assert(height != 0);

	// Right now we just generate a snake maze since we don't have more thant 2 way
	// cross maps.
	// The end point should tp to a the boss_1 room.
	// And the boss_1 room should link back to the beginning.

	InstanceInfo info;

	std::unordered_map<OrderedPair<size_t>, std::string> rooms_name;
	rooms_name[OrderedPair<size_t>{0, 1}] = "left_up";
	rooms_name[OrderedPair<size_t>{0, 2}] = "left_right";
	rooms_name[OrderedPair<size_t>{0, 3}] = "left_down";
	rooms_name[OrderedPair<size_t>{1, 2}] = "up_right";
	rooms_name[OrderedPair<size_t>{1, 3}] = "up_down";
	rooms_name[OrderedPair<size_t>{2, 3}] = "right_down";

	std::unordered_map<std::pair<size_t, size_t>, OrderedPair<size_t>> pos_to_dir;

	for (size_t y = 0; y < height; ++y) {
		for (size_t x = 0; x < width; x++) {
			if (x == 0 || x + 1 == width) continue; // we'll do the turn later

			pos_to_dir[{x, y}] = { 0, 2 };
		}
	}

	for (size_t y = 0; y < height; y+=2) {
		pos_to_dir[{0, y}] = { 3, 2 };
		pos_to_dir[{width - 1, y}] = { 0, 1 };
	}
	for (size_t y = 1; y < height; y+=2) {
		pos_to_dir[{0, y}] = { 2, 1 };
		pos_to_dir[{width - 1, y}] = { 3, 0 };
	}

	std::unordered_map<std::pair<size_t, size_t>, size_t> pos_to_index;

	// now we construct the Sections.
	for (auto& [pos, dir] : pos_to_dir) {
		pos_to_index[pos] = info.sections.size();

		info.sections.push_back(SectionInfo::loadJson(AM::getJson(rooms_name[dir])));
		auto& sec = info.sections.back();
		std::transform( //we don't care for the id in the json, we reinstantiate them.
			std::begin(sec.portals),
			std::end(sec.portals),
			std::begin(sec.portals),
			[](const PortalInfo& i) {
				auto copy = i;
				copy.id = UUID{};
				return copy;
			}
		);
		auto list_of_dir = { dir.a, dir.b }; // we link all the portals together.
		for (auto d : list_of_dir) {

			// little hack, we don't want to connect actively the ends points.
			if (pos == std::pair<size_t, size_t>{0, 0} && d != 2u) continue;
			if (
				pos == std::pair<size_t, size_t>{height % 2 ? width - 1 : 0, height + 1} &&
				d != (height % 2 ? 0u : 2u)
			) continue;
			

			if (
				auto next_pos = next_pos_from_to(pos, d);
				pos_to_index.count(next_pos) > 0
			) {
				auto portal_spot_to_look_for = portal_spot_to_look_for_from_dir(d);

				PortalInfo* their_portal{ nullptr };
				PortalInfo* my_portal{ nullptr };

				for (auto& p : info.sections[pos_to_index[next_pos]].portals) {
					if (p.spot == portal_spot_to_look_for) { their_portal = &p; break; }
				}
				for (auto& p : sec.portals) {
					if (p.spot == d) { my_portal = &p; break; }
				}

				assert(their_portal);
				assert(my_portal);

				my_portal->tp_to = their_portal->id;
				their_portal->tp_to = my_portal->id;
			}
		}
	}

	auto& first_section = info.sections[pos_to_index[{0, 0}]];
	auto& last_section = info.sections[pos_to_index[{
		(height % 2) == 0 ? 0 : width - 1, height - 1
	}]];

	auto last_portal_it = std::find_if(
		std::begin(last_section.portals),
		std::end(last_section.portals),
		[](const PortalInfo& it) {return it.spot == 1; }
	);
	assert(last_portal_it != std::end(last_section.portals));

	auto first_portal_it = std::find_if(
		std::begin(first_section.portals),
		std::end(first_section.portals),
		[](const PortalInfo& it) {return it.spot == 3; }
	);
	assert(first_portal_it != std::end(first_section.portals));

	// We know it only has one portal.
	auto boss_section = SectionInfo::loadJson(AM::getJson("boss_1"));
	auto& boss_portal = boss_section.portals[0];
	boss_portal.id = {};

	last_portal_it->tp_to = boss_portal.id;
	boss_portal.tp_to = first_portal_it->id;

	info.sections.push_back(boss_section);

	return info;
}

InstanceInfo InstanceInfo::generate_graph(
	size_t n, const std::filesystem::path& pool_of_rooms
) noexcept {
	assert(std::filesystem::is_directory(pool_of_rooms));

	enum class Dir {
		Left,
		Top,
		Right,
		Bot,
		Count
	};

	auto complementary_dir = [](Dir d) {
		if (d == Dir::Left) return Dir::Right;
		if (d == Dir::Right) return Dir::Left;
		if (d == Dir::Top) return Dir::Bot;
		if (d == Dir::Bot) return Dir::Top;
		assert(true);
		return Dir::Left;
	};

	struct DirHash { std::size_t operator()(Dir t) const { return static_cast<std::size_t>(t); } };

	std::vector<std::unordered_set<Dir, DirHash>> dirs_to_index;
	std::vector<SectionInfo> room_pool;

	for (auto f : std::filesystem::recursive_directory_iterator(pool_of_rooms)) {
		auto d_struct = load_from_json_file(f);
		if (!d_struct) {
			std::cerr << "Couldn't load file: " << f << std::endl;
			continue;
		}

		room_pool.push_back((SectionInfo)*d_struct);

		//>TODO harmonize direction away from size_t (soooo stupid)
		auto all_dir_in_uint = get_all_accessible_dir(room_pool.back());
		std::unordered_set<Dir, DirHash> all_dir;
		for (auto x : all_dir_in_uint) all_dir.insert((Dir)x);

		dirs_to_index.push_back(all_dir);
	}

	// obtained with the help of wolfram alpha and desmos.com :D
	// there is probably a formula for the estimated radius of n circles packed in an unit square
	// but whatever.
	float estimated_radius;
	{
		double a = 0.023833;
		double b = 5.62943;
		double c = -0.996572;
		double f = 0.000370036;

		estimated_radius = (float)(std::log(b * (std::pow((double)n, a) + c)) / std::log(f));
	}
	auto section_sites = poisson_disc_sampling(estimated_radius, { 1, 1 });

	struct Node_Data {
		Vector2f instance_pos;
		std::optional<size_t> section_index;
	};
	Graph<Node_Data> graph;
	for (auto p : section_sites) {
		Node_Data node;
		node.instance_pos = p;
		add_node(graph, node, {});
	}
	graph_min_spanning_tree<Node_Data>(
		graph,
		[](const Node<Node_Data>& A, const Node<Node_Data>& B) {
			return std::abs(
				(A.data.instance_pos - B.data.instance_pos).length2() /
				std::powf(
					std::cosf(2 * (float)A.data.instance_pos.angleTo(B.data.instance_pos)), 4.f
				)
			);
		}
	);

	// We add a few edges at random since we don't want to make a tree, a few cycle is encoraged.
	size_t n_added_edges = (size_t)(unitaryRng(RD) * n / 2.f);

	auto nodes_to_add_edges = reservoir_sampling_predicated<UUID, Node<Node_Data>>(
		graph,
		[](const Node<Node_Data>& x) {
			return x.edges.size() == 1;
		},
		n_added_edges,
		RD
	);

	for (auto& node : nodes_to_add_edges) {
		UUID min = graph.begin()->first;
		auto angle =
			(float)node->data.instance_pos.angleTo(graph.begin()->second.data.instance_pos);
		float min_score =
			(node->data.instance_pos - graph.begin()->second.data.instance_pos).length2() /
			std::powf(std::cosf(2 * angle), 4.f);

		for (auto&[id, candidate] : graph) {
			if (node == &candidate) continue;
			if (is_linked(*node, candidate)) continue;

			auto angle = (float)node->data.instance_pos.angleTo(candidate.data.instance_pos);
			
			// If the corresponding room already have someone on that side we don't insert ourself.
			// >TODO remove this when i'll handle multiple way out.
			Dir discrete_angle;
			if (-C::PIf / 4 < angle && angle < C::PIf / 4) discrete_angle = Dir::Right;
			if (C::PIf / 4 < angle && angle < 3 * C::PIf / 4) discrete_angle = Dir::Bot;
			if (3 * C::PIf / 4 < angle || angle < -3 * C::PIf / 4) discrete_angle = Dir::Left;
			if (-3 * C::PIf / 4 < angle && angle < -C::PIf / 4) discrete_angle = Dir::Top;

			bool already_have{ false };
			for (auto& x : candidate.edges) {
				float x_angle =
					(float)graph.at(x).data.instance_pos.angleTo(candidate.data.instance_pos);

				// >TODO make a Dir Enum in a proper header file for evryone to use.
				Dir discrete_x_angle;
				if (-C::PIf / 4 < x_angle && x_angle < C::PIf / 4) discrete_x_angle = Dir::Right;
				if (C::PIf / 4 < x_angle && x_angle < 3 * C::PIf / 4) discrete_x_angle = Dir::Bot;
				if (3 * C::PIf / 4 < x_angle || x_angle < -3 * C::PIf / 4)
					discrete_x_angle = Dir::Left;
				if (-3 * C::PIf / 4 < x_angle && x_angle < -C::PIf / 4)
					discrete_x_angle = Dir::Top;

				if (discrete_angle == discrete_x_angle) {
					already_have = true;
					break;
				}
			}

			if (already_have) continue;


			auto candidate_score =
				(node->data.instance_pos - candidate.data.instance_pos).length2() /
				std::powf(std::cosf(2 * angle), 4.f);



			if (candidate_score < min_score) {
				min = id;
				min_score = candidate_score;
			}
		}

		link_nodes(*node, graph.at(min));
	}

	InstanceInfo instance_info;

	for (auto&[id, node] : graph) {
		std::vector<Dir> section_outgoing_dir;
		std::vector<UUID> section_outgoing_neighboor;

		for (auto& edge : node.edges) {
			auto pos_dt = (graph.at(edge).data.instance_pos - node.data.instance_pos);
			auto angle = pos_dt.angleX();
			Dir discrete_angle;
			if (-C::PIf / 4 < angle && angle < C::PIf / 4) discrete_angle = Dir::Right;
			if (C::PIf / 4 < angle && angle < 3 * C::PIf / 4) discrete_angle = Dir::Bot;
			if (3 * C::PIf / 4 < angle || angle < -3 * C::PIf / 4) discrete_angle = Dir::Left;
			if (-3 * C::PIf / 4 < angle && angle < -C::PIf / 4) discrete_angle = Dir::Top;

			if (auto it = std::find(
				std::begin(section_outgoing_dir), std::end(section_outgoing_dir), discrete_angle
			); it != std::end(section_outgoing_dir)) {
				// we need to insert an intermediary section.
				// it's a Y switch.

				// for now a path get "forgotten" meaning it's not connected to main component

				//>TODO
			}

			section_outgoing_dir.push_back(discrete_angle);
			section_outgoing_neighboor.push_back(edge);
		}

		std::unordered_set<Dir, DirHash> set_of_dir(
			std::begin(section_outgoing_dir), std::end(section_outgoing_dir)
		);

		std::vector<size_t> available_indexes;
		for (size_t i = 0; i < dirs_to_index.size(); ++i) {
			if (dirs_to_index[i] == set_of_dir) {
				available_indexes.push_back(i);
			}
		}

		assert(!available_indexes.empty());

		size_t idx = available_indexes[(size_t)(unitaryRng(RD) * available_indexes.size())];
		SectionInfo section_info = room_pool[idx];
		section_info.instance_pos = node.data.instance_pos;

		std::transform( //we don't care for the default id we reset them.
			std::begin(section_info.portals),
			std::end(section_info.portals),
			std::begin(section_info.portals),
			[](const PortalInfo& i) {
				auto copy = i;
				copy.id = UUID{};
				return copy;
			}
		);

		for (size_t i = 0; i < section_outgoing_dir.size(); ++i) {
			auto& neighboor = graph.at(section_outgoing_neighboor[i]);
			if (!neighboor.data.section_index) continue;
			auto& neighboor_section = instance_info.sections[*neighboor.data.section_index];

			// That mean we can connect the two.

			auto my_portal = std::find_if(
				std::begin(section_info.portals),
				std::end(section_info.portals),
				[&](const PortalInfo& portal_info) {
					return (Dir)portal_info.spot == section_outgoing_dir[i];
				}
			);
			assert(my_portal != std::end(section_info.portals));

			auto their_portal = std::find_if(
				std::begin(neighboor_section.portals),
				std::end(neighboor_section.portals),
				[&](const PortalInfo& portal_info) {
					return (Dir)portal_info.spot == complementary_dir(section_outgoing_dir[i]);
				}
			);
			assert(their_portal != std::end(neighboor_section.portals));

			my_portal->tp_to = their_portal->id;
			their_portal->tp_to = my_portal->id;
		}

		node.data.section_index = instance_info.sections.size();
		instance_info.sections.push_back(section_info);
	}

	return instance_info;
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


	if (section->getTimeSinceEntered() > 0.0) {
		std::optional<PortalInfo> opt;
		
		for (auto& p : section->getAllPortals()) {
			if (segment_rec(p.frontier, section->getPlayer()->getBoundingBox())) {
				opt = p;
				break;
			}
		}

		if (auto p = *opt; opt) {
			section->exit();
			auto next = getNextSectionIfInstantiated(p);
			
			PortalInfo portal_to_enter_from;
			if (next) {
				current_section = *next;

				section = entity_store.get(current_section);
				portal_to_enter_from = section->getPortal(p.tp_to);
			}
			else {
				size_t current_section_info_idx;
				auto nextToInstantiate =
					find<SectionInfo>(
						info.sections,
						[&, id = p.tp_to, idx = 0](const SectionInfo& i) mutable {
							auto it = std::find_if(
								std::begin(i.portals),
								std::end(i.portals),
								[id](PortalInfo i) {
									return i.id == id;
								}
							);
							if (it != std::end(i.portals)) {
								portal_to_enter_from = *it;
								current_section_info_idx = idx;
								return true;
							}
							++idx;
							return false;
						}
					);

				assert(nextToInstantiate);
				sections.push_back(entity_store.make<Section>(*nextToInstantiate));
				current_section = sections.back();
				entity_store.get(current_section)->original_info_idx = current_section_info_idx;
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

void Instance::render_map(sf::RenderTarget& target) noexcept {
	for (size_t i = 0; i < info.sections.size(); ++i) {
		const auto& s = info.sections[i];
		for (const auto& p : s.portals) {
			if (p.tp_to == UUID::zero()) continue;
			auto edge = find<SectionInfo>(info.sections, [&](const SectionInfo& x) {
				return find<PortalInfo>(x.portals, [&](const PortalInfo& b) {
					return b.id == p.tp_to;
				}) != nullptr;
			});
			assert(edge);

			Vector2f::renderLine(target, s.instance_pos, edge->instance_pos, { 1, 1, 1, 1 });
		}
	}

	for (size_t i = 0; i < info.sections.size(); ++i) {
		const auto& s = info.sections[i];

		sf::CircleShape mark{ 0.01f };
		mark.setOrigin(0.01f, 0.01f);
		mark.setPosition(s.instance_pos);

		if (i == entity_store.get(current_section)->original_info_idx) {
			mark.setFillColor({ 255, 0, 0, 255 });
		}

		target.draw(mark);
	}
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
Instance::getNextSectionIfInstantiated(const PortalInfo& portal) const noexcept {
	auto tp_to = portal.tp_to;

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

size_t Instance::getCurrentSectionIndex() const noexcept {
	return 
		std::find(std::begin(sections), std::end(sections), current_section) - 
		std::begin(sections);
}