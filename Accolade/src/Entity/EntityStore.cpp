#include "EntityStore.hpp"
#include "Ruins/Section.hpp"
#include "Eid.hpp"

#pragma region GET
#ifdef AGGRESSIVE_RELEASE
#define X(n, x) \
template<>\
x* EntityStore::get_impl(const Eid<x>& ptr) noexcept {\
	return &n[(integer_t)ptr];\
}
#else
#define X(n, x) \
template<>\
x* EntityStore::get_impl(const Eid<x>& ptr) noexcept {\
	auto f = n.find((integer_t)ptr);\
	return f == std::end(n) ? nullptr : &f->second;\
}
#endif

X(sections, Section);

#undef X
#pragma endregion

#pragma region MAKE
#define X(n, x)\
template<typename... Args>\
Eid<x> EntityStore::make_impl(Args&&... args) noexcept {\
	n.emplace({ Eid<x>::N, x(std::forward(args)...) });\
	return { Eid<x>::N++ };\
}

X(sections, Section);

#undef X
#pragma endregion