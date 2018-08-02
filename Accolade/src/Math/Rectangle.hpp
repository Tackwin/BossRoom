#pragma once
#include "Vector.hpp"
#include "3rd/json.hpp"

template<typename T>
struct Rectangle {
	union {
		struct {
			Vector<2, T> pos;
			Vector<2, T> size;
		};
		struct {
			T x;
			T y;
			T w;
			T h;
		};
	};

	Rectangle() {}

	Rectangle(const Vector<2, T>& pos, const Vector<2, T>& size) :
		pos(pos),
		size(size) 
	{}

	bool intersect(const Rectangle<T>& other) const {
		return !(
				pos.x + size.x < other.pos.x || pos.x > other.pos.x + other.size.x ||
				pos.y + size.y < other.pos.y || pos.y > other.pos.y + other.size.y
			);
	}

	bool isOnTopOf(Rectangle<T> other) const {
		T dBotTop = (y + h) - (other.y);
		T dTopBot = (y) - (other.y + other.h);

		return dBotTop > dTopBot;
	}

	Vector<2, T> center() const {
		return pos + size / 2;
	}

	void setCenter(Vector2<T> vec) noexcept {
		pos = vec - size / 2.0;
	}

	T bot() const {
		return pos.y + size.y;
	}

	Rectangle<T> fitUpRatio(double ratio) const noexcept {
		if (w > h) {
			return { pos,{ w, (T)(w / ratio) } };
		}
		else {
			return { pos,{ (T)(h * ratio), h } };
		}
	}
	Rectangle<T> fitDownRatio(double ratio) const noexcept {
		if (w < h) {
			return { pos,{ w, (T)(w / ratio) } };
		}
		else {
			return { pos,{ (T)(h * ratio), h } };
		}
	}

	Rectangle<T> restrictIn(Rectangle<T> area) const noexcept {
		if (area.area() <= this->area()) return { area.center(), size };

		Rectangle<T> result = *this;

		if (x < area.x) {
			result.x = area.x;
		}
		if (x + w > area.x + area.w) {
			result.x = area.x + area.w - w;
		}

		if (y < area.y) {
			result.y = area.y;
		}
		if (y + h > area.y + area.h) {
			result.y = area.y + area.h - h;
		}

		return result;
	}

	T area() const noexcept {
		return w * h;
	}

	template<typename U>
	bool in(const Vector<2, U>& p) const {
		return p.inRect(pos, size);
	}

#ifdef SFML_GRAPHICS_HPP
#include <SFML/Graphics.hpp>

	void render(sf::RenderTarget& target, Vector4f color) const noexcept {
		sf::RectangleShape shape{ size };
		shape.setPosition(pos);
		shape.setFillColor(color);
		target.draw(shape);
	}

#endif

	static Rectangle<T> loadJson(nlohmann::json json) noexcept {
		Rectangle<T> rec;
		if (json.is_array()) {
			rec.x = json.get<std::vector<T>>()[0];
			rec.y = json.get<std::vector<T>>()[1];
			rec.w = json.get<std::vector<T>>()[2];
			rec.h = json.get<std::vector<T>>()[3];
		}
		return rec;
	}
	static nlohmann::json saveJson(Rectangle<T> rec) noexcept {
		return nlohmann::json::array({rec.x, rec.y, rec.w, rec.h});
	}


	static Rectangle<T> hull(std::vector<Rectangle<T>> recs) noexcept {
		Rectangle<T> hull = recs[0];

		for (auto rec : recs) {
			hull.x = std::min(rec.x, hull.x);
			hull.y = std::min(rec.y, hull.y);
		}
		for (auto rec : recs) {
			hull.w = std::max(rec.x + rec.w, hull.w + hull.x) - hull.x;
			hull.h = std::max(rec.y + rec.h, hull.h + hull.y) - hull.y;
		}
		return hull;
	}

};

template<typename T>
using Rectangle2 = Rectangle<T>;
using Rectangle2f = Rectangle2<float>;