#pragma once

// under heavely hard work
#include <array>
#include <string>
#include <numeric>

template<uint32_t D, typename c_t>
struct Vec {
	union {
		std::array<c_t, D> components;
		struct { c_t x, y, z, w; };
		struct { c_t r, g, b, a; };
	};

	Vec(c_t l = 0) {
		for (uint32_t i = 0u; i < D; ++i) {
			components[i] = l;
		}
	}

	operator std::string const () {
		std::string str = "{";
		for (uint32_t i = 0U; i < D; ++i) {
			str += " ";
			str += std::to_string(components[i]);

			if (i != D - 1) {
				str += ",";
			}
		}
		str += "}";

		return str;
	}

	c_t& operator[](uint32_t i) {
		assert(i <= D);
		return components[i];
	}
	
	template<typename c_u>
	Vec<D, c_t>& operator+(const Vec<D, c_u>& other) {
		Vec<D, c_t> result;

		for (uint32_t i = 0u; i < D; ++i) {
			result[i] = components[i] + static_cast<c_t>(other.components[i]);
		}
		
		return result;
	}
};
/*
template<typename c_t>
struct Vec<1U, c_t> {
	static constexpr uint32_t D = 1U;

	union {
		std::array<c_t, D> components;
		c_t x;
	};

	Vec(c_t l = 0) {
		for (uint32_t i = 0u; i < D; ++i) {
			components[i] = l;
		}
	}

	c_t operator[](uint32_t i) {
		assert(i <= D);
		return components[i];
	}
};
template<typename c_t>
struct Vec<2U, c_t> {
	static constexpr uint32_t D = 2U;

	union {
		std::array<c_t, D> components;
		c_t x, y;
	};

	Vec(c_t l = 0) {
		for (uint32_t i = 0u; i < D; ++i) {
			components[i] = l;
		}
	}

	c_t operator[](uint32_t i) {
		assert(i <= D);
		return components[i];
	}
};
template<typename c_t>
struct Vec<3U, c_t> {
	static constexpr uint32_t D = 3U;

	union {
		std::array<c_t, D> components;
		c_t x, y, z;
	};

	Vec(c_t l = 0) {
		for (uint32_t i = 0u; i < D; ++i) {
			components[i] = l;
		}
	}

	c_t operator[](uint32_t i) {
		assert(i <= D);
		return components[i];
	}
};
template<typename c_t>
struct Vec<4U, c_t> {
	static constexpr uint32_t D = 4U;

	union {
		std::array<c_t, D> components;
		struct { c_t x, y, z, w; };
		struct { c_t r, g, b, a; };
	};

	Vec(c_t l = 0) {
		for (uint32_t i = 0u; i < D; ++i) {
			components[i] = l;
		}
	}

	c_t operator[](uint32_t i) {
		assert(i <= D);
		return components[i];
	}
};*/