#pragma once
#include <unordered_map>
#include <optional>

template<typename T> class Eid;
class Section;

class EntityStore {
public:
	using integer_t = size_t;

	template<typename T>
	T* get(const Eid<T>& ptr) noexcept {
		return get_impl(ptr);
	}

	template<typename T, typename... Args>
	Eid<T> make(Args&&... args) noexcept {
		return make_impl(std::forward(args)...);
	}

private:

	template<typename T>
	T* get_impl(const Eid<T>&) noexcept { static_assert(false); };
#define X(x) template<> x* get_impl(const Eid<x>& ptr) noexcept
	X(Section);
#undef X

	template<typename T, typename... Args>
	Eid<T> make_impl(Args&&...) noexcept { static_assert(false); };
#define X(x) template<typename... Args> Eid<x> make_impl(Args&&...) noexcept
	X(Section);
#undef X

	std::unordered_map<integer_t, Section> sections;
};