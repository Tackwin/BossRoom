#pragma once

constexpr long double operator"" _min(long double v) noexcept {
	return v * 60.0;
}
constexpr long double operator"" _s(long double v) noexcept {
	return v;
}
constexpr long double operator"" _ms(long double v) noexcept {
	return v / 1000.0;
}
constexpr long double operator"" _us(long double v) noexcept {
	return v / (1000.0 * 1000.0);
}

constexpr long double operator"" _m(long double v) noexcept {
	return v;
}
constexpr long double operator"" _cm(long double v) noexcept {
	return v / 100.0;
}
constexpr long double operator"" _mm(long double v) noexcept {
	return v / 1000.0;
}