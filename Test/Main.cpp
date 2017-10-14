#include <type_traits>
#include <iostream>

template<size_t D, typename T>
struct Vector {

	template<size_t Dp = D>
	Vector(T x, T y, typename std::enable_if<Dp == 2, T>::type* a = nullptr) {
		printf("enable_if\n");
	}

};

void main() {
	Vector<1, int> a;
	Vector<2, int> b(0, 0);
	std::cin.get();
}