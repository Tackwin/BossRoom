#pragma once
#include <unordered_map>
#include <optional>

#include "Utils/meta_algorithms.hpp"

template<typename T> class Eid;
class Section;

class EntityStore {
public:
	using integer_t = size_t;

	template<typename T>
	std::enable_if_t<std::is_const_v<T>, T*> get(const Eid<T>& ptr) const noexcept {
#define X(x)\
		if constexpr (\
			std::is_same_v<key_map_type<decltype(x)>::Value, std::remove_const_t<T>>\
		){\
			auto f = x.find((integer_t)ptr);\
			return f == std::end(x) ? nullptr : &f->second;\
		}

		X(sections);
#undef X
	}
	template<typename T>
	std::enable_if_t<!std::is_const_v<T>, T*> get(const Eid<T>& ptr) noexcept {
#define X(x)\
		if constexpr (std::is_same_v<key_map_type<decltype(x)>::Value, T>){\
			auto f = x.find((integer_t)ptr);\
			return f == std::end(x) ? nullptr : &f->second;\
		}

		X(sections);
#undef X
	}

	template<typename T, typename... Args>
	Eid<T> make(Args&&... args) noexcept {
#define X(x)\
		if constexpr (std::is_same_v<key_map_type<decltype(x)>::Value, T>) {\
			integer_t id = Eid<T>::N;\
			T t(std::forward<Args>(args)...);\
			x.insert(std::make_pair<integer_t, T>(\
				std::forward<integer_t>(id), std::forward<T>(t)\
			));\
			return { Eid<T>::N++ };\
		}
		X(sections);
#undef X
	}

	template<typename T>
	void destroy(const Eid<T>& ptr) noexcept {
#define X(x) \
		if constexpr (std::is_same_v<key_map_type<decltype(x)>::Value , T>) {\
			x.erase((integer_t)ptr);\
		}
		X(sections);
#undef X
	}

private:

	template<typename T> std::enable_if_t<std::is_const_v<T>, T*>
	get_impl(const Eid<T>&) const noexcept { static_assert(false); };

	template<typename T> std::enable_if_t<!std::is_const_v<T>, T*>
	get_impl(const Eid<T>&) noexcept { static_assert(false); };

#define X(x) \
	template<> x* get_impl(const Eid<x>& ptr) noexcept; \
	template<> const x* get_impl(const Eid<const x>& ptr) const noexcept;

	X(Section);
#undef X

	std::unordered_map<integer_t, Section> sections;
};
