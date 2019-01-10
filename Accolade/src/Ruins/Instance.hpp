#pragma once
#include <SFML/Graphics.hpp>
#include <filesystem>
#include <optional>
#include <vector>

#include "Entity/EntityStore.hpp"
#include "Entity/Eid.hpp"

#include "3rd/json.hpp"

#include "Math/Vector.hpp"
#include "Math/Rectangle.hpp"

#include "Section.hpp"

struct InstanceInfo {
	std::vector<SectionInfo> sections;
	size_t start_section_idx;

	static InstanceInfo loadJson(const nlohmann::json& json) noexcept;
	static nlohmann::json saveJson(const InstanceInfo& info) noexcept;

	static InstanceInfo generateMaze(size_t width, size_t height) noexcept;
	static InstanceInfo generate_graph(
		size_t n, const std::filesystem::path& pool_of_rooms
	) noexcept;
};

class Instance {
public:
	Instance(const InstanceInfo& info) noexcept;

	void generateGrid(size_t n) noexcept;

	void enter() noexcept;
	void startAt(size_t p) noexcept;

	void update(double dt) noexcept;
	void render(sf::RenderTarget& target) noexcept;
	void renderDebug(sf::RenderTarget& target) noexcept;

	void render_map(sf::RenderTarget& target) noexcept;

	void hardSetCurrentSection(const SectionInfo& sec) noexcept;

	const Section& getCurrentSection() const noexcept;
	Section& getCurrentSection() noexcept;
	size_t getCurrentSectionIndex() const noexcept;

	std::optional<Eid<Section>>
	getNextSectionIfInstantiated(const PortalInfo& portal) const noexcept;

private:
	InstanceInfo info;
	std::vector<Eid<Section>> sections;

	Eid<Section> current_section;

	EntityStore entity_store;

	size_t complementary_dir(size_t dir) const noexcept;
};