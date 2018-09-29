#include "Instance.hpp"
#include "Common.hpp"

#include "Math/Segment.hpp"

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
				float margin_x = unitaryRng(RD) * (info.roofSamples[i].x / 10) + 5;
				float height = unitaryRng(RD) * (50 - 20) + 20;
				float pos_y = unitaryRng(RD) * (info.roofSamples[i].y * 0.8) + 50;

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
				r.render(window, { 0, 0, 0, 0 }, { 1, 1, 1, 1 }, 0.05);
			}
		}

		window.display();
		n_frames++;
	}
}

void Instance::spaceTowers(sf::RenderWindow& window) noexcept {
	size_t n_frames = 0;
	size_t i = 0;
	float sum_x = 0;

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
				r.render(window, { 0, 0, 0, 0 }, { 1, 1, 1, 1 }, 0.05);
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
				float pos_y = unitaryRng(RD) * (info.roofSamples[i].y * 0.8) + 50;

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
				r.render(window, { 0, 0, 0, 0 }, { 1, 1, 1, 1 }, 0.05);
			}
		}

		window.display();
		n_frames++;
	}
}

