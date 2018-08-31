#pragma once

struct null_t {};

template<typename... List>
struct TypeList {};

template<typename list>
struct ListContent;

template<typename T, typename... U>
struct ListContent<TypeList<T, U...>> {
	using head = T;
	using tail = TypeList<U...>;
};

template<>
struct ListContent<TypeList<>> {
	using head = null_t;
	using tail = null_t;
};

template<typename T>
using head_of = typename ListContent<T>::head;
template<typename T>
using tail_of = typename ListContent<T>::tail;
