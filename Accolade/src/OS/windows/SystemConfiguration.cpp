#include "OS/SystemConfiguration.hpp"

#include <Windows.h>

size_t get_double_click_time() noexcept {
	return (size_t)GetDoubleClickTime();
}
