#pragma once
#include "Vector.hpp"

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

	// works only for top down y
	bool isOnTopOf(Rectangle<T> other) const {
		T distEdgeToEdge = std::max(other.x + other.w - x, x + w - other.x);
		T sumOfWidth = w + other.w;

		return y < other.y && distEdgeToEdge < sumOfWidth;
	}
	bool isFullyOnTopOf(Rectangle<T> other, T tolerance = FLT_EPSILON) const noexcept {
		return y + h < other.y + tolerance;
	}

	// works only for top down y
	bool isOnBotOf(Rectangle<T> other) const {
		T distEdgeToEdge = std::max(other.x + other.w - x, x + w - other.x);
		T sumOfWidth = w + other.w;

		return y > other.y && distEdgeToEdge < sumOfWidth;
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

	// >TODO: implement this, for now we treat this as if it were the support
	// of a circle of radius max(w, h) / 2
	Vector2<T> support(T a, T d) const noexcept {
		return Vector2<T>::createUnitVector((double)a) * (d + std::max({ w, h }) / 2);
	}

	Vector2<T> topLeft() const noexcept {
		return pos;
	}
	Vector2<T> topRight() const noexcept {
		return { x + w, y };
	}
	Vector2<T> botLeft() const noexcept {
		return { x, y + h };
	}
	Vector2<T> botRight() const noexcept {
		return pos + size;
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
	void render(
		sf::RenderTarget& target, Vector4f in, Vector4f on, float thick = 0.01f
	) const noexcept {
		sf::RectangleShape shape{ size };
		shape.setPosition(pos);
		shape.setFillColor(in);
		shape.setOutlineColor(on);
		shape.setOutlineThickness(thick * std::min(w, h));
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

#ifdef NLOHMANN_JSON_HPP
template<typename T>
void from_json(const nlohmann::json& json, Rectangle<T>& x) noexcept {
	assert(json.is_array());

	if (json.size() == 2) {
		x.pos = json.at(0);
		x.size = json.at(1);
	}
	else {
		x.x = json.at(0);
		x.y = json.at(1);
		x.w = json.at(2);
		x.h = json.at(3);
	}
}

template<typename T>
void to_json(nlohmann::json& json, const Rectangle<T>& x) noexcept {
	json.push_back(x.x);
	json.push_back(x.y);
	json.push_back(x.w);
	json.push_back(x.h);
}
#endif