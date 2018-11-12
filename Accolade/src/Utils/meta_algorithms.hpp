#pragma once
#include <type_traits>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <type_traits>

#include "Metaprogramming/TypeList.hpp"

template<typename T>
struct size_of_s;

template<>
struct size_of_s<TypeList<>> {
	constexpr static size_t value = 0;
};

template<typename T, typename... U>
struct size_of_s<TypeList<T, U...>> {
	constexpr static size_t value = sizeof...(U) + 1;
};

template<typename T>
constexpr auto size_of = size_of_s<T>::value;

template<typename F, typename T, typename... U>
constexpr void for_each(T& t, U&... u) {
	F{}(t);
	return for_each<F>(u...);
}
template<typename F, typename T>
constexpr void for_each(T& t) {
	return F{}(t);
}

template<typename T>
struct holded;

template<typename T>
struct holded<std::vector<T>> {
	using type = T;
};
template<typename T>
struct holded<std::shared_ptr<T>> {
	using type = T;
};
template<typename T>
struct holded<std::unique_ptr<T>> {
	using type = T;
};
template<typename T>
struct holded<std::initializer_list<T>> {
	using type = T;
};

template<typename T>
struct key_map_type;

template<typename U, typename V>
struct key_map_type<std::unordered_map<U, V>> {
	using key = U;
	using Value = V;
};
template<typename U, typename V>
struct key_map_type<std::map<U, V>> {
	using key = U;
	using Value = V;
};

template<typename T>
using holded_t = typename holded<std::decay_t<T>>::type;

template<typename T>
struct is_vector : std::false_type {};
template<typename T>
struct is_vector<std::vector<T>> : std::true_type {};
template<typename T>
constexpr bool is_vector_v = is_vector<T>::value;


template<typename T>
struct is_shared_ptr: std::false_type {};
template<typename T>
struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};
template<typename T>
constexpr bool is_shared_ptr_v = is_shared_ptr<std::decay_t<T>>::value;

template<typename T>
struct is_unique_ptr : std::false_type {};
template<typename T>
struct is_unique_ptr<std::unique_ptr<T>> : std::true_type {};
template<typename T>
constexpr bool is_unique_ptr_v = is_unique_ptr<std::decay_t<T>>::value;

template<typename T>
struct is_weak_ptr : std::false_type {};
template<typename T>
struct is_weak_ptr<std::weak_ptr<T>> : std::true_type {};
template<typename T>
constexpr bool is_weak_ptr_v = is_weak_ptr<std::decay_t<T>>::value;

template<typename T>
constexpr bool is_smart_ptr_v =
	is_weak_ptr_v<T> || is_shared_ptr_v<T> || is_unique_ptr_v<T>;


//specialize a type for all of the STL containers.
namespace is_stl_container_impl {
	template <typename T>       struct is_stl_container :std::false_type {};
	template <typename T, std::size_t N> 
		struct is_stl_container<std::array     <T, N>> :std::true_type {};
	template <typename... Args> 
		struct is_stl_container<std::vector             <Args...>> :std::true_type {};
	template <typename... Args>
		struct is_stl_container<std::deque              <Args...>> :std::true_type {};
	template <typename... Args>
		struct is_stl_container<std::list               <Args...>> :std::true_type {};
	template <typename... Args>
		struct is_stl_container<std::forward_list       <Args...>> :std::true_type {};
	template <typename... Args>
		struct is_stl_container<std::set                <Args...>> :std::true_type {};
	template <typename... Args>
		struct is_stl_container<std::multiset           <Args...>> :std::true_type {};
	template <typename... Args>
		struct is_stl_container<std::map                <Args...>> :std::true_type {};
	template <typename... Args>
		struct is_stl_container<std::multimap           <Args...>> :std::true_type {};
	template <typename... Args>
		struct is_stl_container<std::unordered_set      <Args...>> :std::true_type {};
	template <typename... Args>
		struct is_stl_container<std::unordered_multiset <Args...>> :std::true_type {};
	template <typename... Args>
		struct is_stl_container<std::unordered_map      <Args...>> :std::true_type {};
	template <typename... Args>
		struct is_stl_container<std::unordered_multimap <Args...>> :std::true_type {};
	template <typename... Args>
		struct is_stl_container<std::stack              <Args...>> :std::true_type {};
	template <typename... Args>
		struct is_stl_container<std::queue              <Args...>> :std::true_type {};
	template <typename... Args>
		struct is_stl_container<std::priority_queue     <Args...>> :std::true_type {};
}

//type trait to utilize the implementation type traits as well as decay the type
template <typename T> 
struct is_stl_container {
	static constexpr bool const value = 
		is_stl_container_impl::is_stl_container<std::decay_t<T>>::value;
};

template<typename T>
constexpr bool is_stl_container_v = is_stl_container<T>::value;


// Primary template with a static assertion
// for a meaningful error message
// if it ever gets instantiated.
// We could leave it undefined if we didn't care.
#define GENERATE_MEMBER_DEDECTOR(x, sig)\
\
template<typename, typename T>\
struct has_##x {\
	static_assert(\
		std::integral_constant<T, false>::value,\
		"Second template parameter needs to be of function type.");\
};\
\
\
template<typename C, typename Ret, typename... Args>\
struct has_##x<C, Ret(Args...)> {\
private:\
	template<typename T>\
	static constexpr auto check(T*)\
		-> typename\
		std::is_same<\
		decltype(std::declval<T>().x(std::declval<Args>()...)),\
		Ret    /* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */ \
		>::type;  /* attempt to call it and see if the return type is correct */ \
		\
	template<typename>\
	static constexpr std::false_type check(...);\
\
	typedef decltype(check<C>(0)) type;\
\
public:\
	static constexpr bool value = type::value;\
};\
template<typename T>\
constexpr bool has_##x##_v = has_##x<T, sig>::value;

GENERATE_MEMBER_DEDECTOR(clone, T*());

#undef GENERATE_MEMBER_DEDECTOR
