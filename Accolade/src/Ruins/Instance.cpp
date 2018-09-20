#include "Instance.hpp"
#include "Common.hpp"

#include "Math/Segment.hpp"

#define SAVE(x, y) json[#x] = y(info.x);

InstanceInfo InstanceInfo::loadJson(const nlohmann::json&) noexcept {
	InstanceInfo info;
	return info;
}

#undef SAVE
#define LOAD(x, y) if (auto i = json.find(#x); i != json.end()) info.x = y(*i);

nlohmann::json InstanceInfo::saveJson(const InstanceInfo&) noexcept {
	nlohmann::json json;

	return json;
}
#undef LOAD

Instance::Instance(const InstanceInfo& info) noexcept : info(info) {}

void Instance::runAlgo(sf::RenderWindow& window) noexcept {
	v = sf::View{ {WIDTH / 2.f, HEIGHT / 2.f}, {(float)WIDTH, (float)HEIGHT} };

	auto old = window.getView();
	samples(window); if (rerun) return;
	smoothSamples(window); if (rerun) return;
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
				if (event.key.code == sf::Keyboard::Space && n_current < info.nSamples) {
					return;
				}
			}
		}

		window.setView(v);
		window.clear();

		if (n_current < info.nSamples) {
			if (i % 10 == 0) {
				info.roofSamples.push_back(
					{ unitaryRng(RD) * 80 + 20, unitaryRng(RD) * 700 }
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
	size_t n_current = 0;
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
				if (event.key.code == sf::Keyboard::Space && n_current < info.nSamples) {
					return;
				}
			}
		}

		window.setView(v);
		window.clear();

		if (n_frames % 200 == 0 && i + 1 < info.roofSamples.size()) {
			auto& it = info.roofSamples[i].y;
			auto& it1 = info.roofSamples[i + 1].y;

			auto dist = std::abs(it - it1);

			constexpr auto A = 4;
			constexpr auto P = -0.43f;

			auto f = [=](auto x) {
				return (float)std::fmin(A / std::powf(x, P), x);
			};

			auto comeClose = f(dist) / 2;

			if (i > 0) {
				auto& itm1 = info.roofSamples[i - 1].y;

				if (dist > std::abs(it - itm1)) {
					it += (std::signbit(it1 - it) ? -1 : 1) * comeClose;
					it1 += (std::signbit(it - it1) ? -1 : 1) * comeClose;
				}
			}
			else {
				it += (std::signbit(it1 - it) ? -1 : 1) * comeClose;
				it1 += (std::signbit(it - it1) ? -1 : 1) * comeClose;
			}

			i++;
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