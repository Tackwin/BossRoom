#pragma once
#include <unordered_set>
#include "Common.hpp"
#include "Utils/UUID.hpp"

template<typename T>
struct Node {
	std::unordered_set<UUID> edges;
	UUID me;
	T data;
};

template<typename T>
struct Graph {
	std::unordered_map<UUID, Node<T>> nodes;
	std::unordered_map<UUID, std::bitset<32>> flags;
};

template<typename T> void link_nodes(Node<T>& A, Node<T>& B) noexcept {
	A.edges.emplace(B.me);
	B.edges.emplace(A.me);
}
template<typename T> bool is_linked(const Node<T>& A, const Node<T>& B) noexcept {
	return A.edges.count(B.me) != 0 || B.edges.count(A.me) != 0;
}
template<typename T>
bool is_remotely_linked(const Graph<T>& graph, const Node<T>& A, const Node<T>& B) noexcept {
	bool connected = false;
	walk_connexe_coponent(graph, A.me, [&](auto x) { if (x == B.me) connected = true; });
	if (connected) return true;
	walk_connexe_coponent(graph, B.me, [&](auto x) { if (x == A.me) connected = true; });
	return connected;
}

template<typename T>
void add_node(Graph<T>& graph, const T& data, std::unordered_set<UUID> neighboor) noexcept
{
	Node<T> n;
	auto id = n.me;
	n.data = data;
	for (auto& x : neighboor) {
		if (graph.nodes.count(x)) link_nodes(n, graph.nodes.at(x));
	}
	graph.nodes[id] = std::move(n);
}

template<typename T>
void walk_connexe_coponent(
	const Graph<T>& graph, UUID origin, std::function<void(UUID)> pred
) noexcept
{
	std::unordered_set<UUID> close;
	std::vector<UUID> open;
	open.push_back(origin);
	while (!open.empty()) {
		auto n = open.back();
		open.pop_back();

		if (close.count(n) != 0) continue;
		pred(n);
		close.emplace(n);

		if (graph.nodes.count(n) == 0) continue;
		auto& node = graph.nodes.at(n);

		for (auto x : node.edges) {
			open.push_back(x);
		}
	}
}

template<typename T>
void gen_lab_merge(
	Graph<T>& graph,
	std::function<float(const T&, const T&)> score = [](auto, auto) { return .0; },
	float connected_factor = 1.f/17.f
) noexcept
{
	std::vector<UUID> open;
	open.reserve(graph.nodes.size());

	std::vector<UUID> admissible_directions;
	open.push_back(graph.nodes.begin()->first);

	while (!open.empty()) {
		auto next = open.back();
		open.pop_back();

		admissible_directions.clear();
		for (const auto&[id, x] : graph.nodes) {
			if (std::count(std::begin(open), std::end(open), id) != 0) continue;
			if (id == next) continue;

			// So we accept only nodes that are not yet connected, to avoid having a maximally
			// connected, but we introduce a bit of a randomness, we don't want a tree so we accept
			// a few cycles.
			bool can_connect =
				!is_remotely_linked(graph, graph.nodes[id], graph.nodes[next]) ||
				unitaryRng(RD) < connected_factor;

			if (can_connect && score(graph.nodes[next].data, graph.nodes[id].data) < 1.f)
				admissible_directions.push_back(id);
		}

		if (admissible_directions.empty()) continue;
		std::sort(
			std::begin(admissible_directions), std::end(admissible_directions),
			[&](const auto& A, const auto& B) {
				auto s = graph.nodes[next].data;
				return score(graph.nodes[A].data, s) < score(graph.nodes[B].data, s);
			}
		);

		link_nodes(graph.nodes[admissible_directions[0]], graph.nodes[next]);


		open.insert(
			std::end(open), std::begin(admissible_directions), std::end(admissible_directions)
		);
	}
}
