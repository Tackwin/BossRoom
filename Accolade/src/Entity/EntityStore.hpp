#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <optional>

#include "Utils/meta_algorithms.hpp"

#include "Math/Vector.hpp"

template<typename T> class Eid;
template<typename T> class OwnId;
class Section;

class EntityStore {
public:
	using integer_t = size_t;

private:
	std::unordered_map<integer_t, Section> sections;
	std::unordered_map<integer_t, Vector2f> vector2fs;
	std::unordered_map<integer_t, sf::View> views;
#define ITERATE\
	X(sections);\
	X(vector2fs);\
	X(views);


public:

	template<typename T>
	std::enable_if_t<std::is_const_v<T>, T*> get(const Eid<T>& ptr) const noexcept {
#define X(x)\
		if constexpr (\
			std::is_same_v<key_map_type<decltype(x)>::Value, std::remove_const_t<T>>\
		){\
			auto f = x.find((integer_t)ptr);\
			return f == std::end(x) ? nullptr : &f->second;\
		}

		ITERATE
#undef X
	}


	template<typename T>
	std::enable_if_t<!std::is_const_v<T>, T*> get(const Eid<T>& ptr) noexcept {
#define X(x)\
		if constexpr (std::is_same_v<key_map_type<decltype(x)>::Value, T>){\
			auto f = x.find((integer_t)ptr);\
			return f == std::end(x) ? nullptr : &f->second;\
		}

		ITERATE
#undef X
	}
	template<typename T>
	T* get(const OwnId<T>& ptr) const noexcept {
		auto x = (const Eid<T>)ptr;
		return get(x);
	}
	template<typename T>
	T* get(const OwnId<T>& ptr) noexcept {
		auto x = (const Eid<T>)ptr;
		return get(x);
	}

	template<typename T, typename... Args>
	Eid<T> make(Args&&... args) noexcept {
		return copy(T{ std::forward<Args>(args)... });
	}

	template<typename T>
	Eid<T> copy(T&& t) noexcept {
#define X(x)\
		if constexpr (std::is_same_v<key_map_type<decltype(x)>::Value, T>) {\
			integer_t id = Eid<T>::N;\
			x.insert(std::make_pair<integer_t, T>(\
				std::forward<integer_t>(id), std::forward<T>(t)\
			));\
			return { Eid<T>::N++ };\
		}

		ITERATE
#undef X
	}

	template<typename T>
	void destroy(const Eid<T>& ptr) noexcept {
#define X(x) \
		if constexpr (std::is_same_v<key_map_type<decltype(x)>::Value , T>) {\
			x.erase((integer_t)ptr);\
		}

		ITERATE
#undef X
	}
	template<typename T>
	void destroy(const OwnId<T>& ptr) noexcept {
		auto& x = (const Eid<T>)ptr;
		destroy(x);
	}

#undef ITERATE
};

using ES = EntityStore;