#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <optional>
#include <memory>

#include "Utils/meta_algorithms.hpp"
#include "Gameplay/Item/Item.hpp"

#include "Math/Vector.hpp"

template<typename T> class Eid;
template<typename T> class OwnId;
class Section;

class EntityStore {
public:
	using integer_t = size_t;

private:
	// We use unique_ptr not really for the ownership but to allow use of abstract classes.
	std::unordered_map<integer_t, std::unique_ptr<Section>> sections;
	std::unordered_map<integer_t, std::unique_ptr<Vector2f>> vector2fs;
	std::unordered_map<integer_t, std::unique_ptr<sf::View>> views;
	std::unordered_map<integer_t, std::unique_ptr<Item>> items;
#define ITERATE\
	X(sections);\
	X(vector2fs);\
	X(items);\
	X(views);


public:

	template<typename T>
	std::enable_if_t<std::is_const_v<T>, T*> get(const Eid<T>& ptr) const noexcept {
#define X(x)\
		if constexpr (std::is_same_v<\
			holded_t<key_map_type<decltype(x)>::Value>,\
			std::remove_const_t<T>\
		>){\
			auto f = x.find((integer_t)ptr);\
			return f == std::end(x) ? nullptr : f->second.get();\
		}

		ITERATE;
#undef X
		// if the compiler complain about no return value then that means that T is not part
		// of the unordered maps.
	}


	template<typename T>
	std::enable_if_t<!std::is_const_v<T>, T*> get(const Eid<T>& ptr) noexcept {
#define X(x)\
		if constexpr (std::is_same_v<holded_t<key_map_type<decltype(x)>::Value>, T>){\
			auto f = x.find((integer_t)ptr);\
			return f == std::end(x) ? nullptr : f->second.get();\
		}

		ITERATE;
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

	template<typename T>
	Eid<T> integrate(std::unique_ptr<T>&& ptr) noexcept {
#define X(x)\
		if constexpr (std::is_same_v<holded_t<key_map_type<decltype(x)>::Value>, T>) {\
			integer_t id = Eid<T>::N;\
			x.insert(std::make_pair<integer_t, std::unique_ptr<T>>(\
				std::forward<integer_t>(id), std::move(ptr)\
			));\
			return { Eid<T>::N++ };\
		}

		ITERATE
#undef X
	}
	template<typename T>
	Eid<T> integrate(T* ptr) noexcept {
#define X(x)\
		if constexpr (std::is_same_v<holded_t<key_map_type<decltype(x)>::Value>, T>) {\
			integer_t id = Eid<T>::N;\
			x.insert(std::make_pair<integer_t, std::unique_ptr<T>>(\
				std::forward<integer_t>(id), std::unique_ptr<T>{ptr}\
			));\
			return { Eid<T>::N++ };\
		}

		ITERATE
#undef X
	}

	template<typename T, typename... Args>
	Eid<T> make(Args&&... args) noexcept {
		return integrate<T>(std::make_unique<T>(std::forward<Args>(args)...));
	}

	template<typename T>
	Eid<T> copy(const T& t) noexcept {
#define X(x)\
		if constexpr (std::is_same_v<holded_t<key_map_type<decltype(x)>::Value>, T>) {\
			integer_t id = Eid<T>::N;\
			x.insert(std::make_pair<integer_t, std::unique_ptr<T>>(\
				std::forward<integer_t>(id), std::make_unique<T>(t)\
			));\
			return { Eid<T>::N++ };\
		}

		ITERATE
#undef X
	}

	template<typename T>
	void destroy(const Eid<T>& ptr) noexcept {
#define X(x) \
		if constexpr (std::is_same_v<holded_t<key_map_type<decltype(x)>::Value>, T>) {\
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