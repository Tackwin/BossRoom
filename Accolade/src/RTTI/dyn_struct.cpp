/*#include "dyn_struct.hpp"

#include <cassert>

dyn_struct::dyn_struct(const dyn_struct& copy) noexcept : kind(copy.kind) {

	std::unique_ptr<array_t> arr{ nullptr };
	for (const auto&[k, v] : copy.values) {
		switch (v.first)
		{
#define X(t) case Kind::t : \
			values[k] = std::pair{\
				Kind::t,\
				std::make_unique<t##_t>(*static_cast<t##_t*>(v.second.get()))\
			}; break;

			X(integer);
			X(real);
			X(string);
			X(boolean);
		case Kind::structure:
			values[k] = std::pair{
				Kind::structure,
				std::make_unique<structure_t>(
					*static_cast<structure_t*>(v.second.get())->get()
				)
			};
			break;
		case Kind::array:
			arr = std::make_unique<array_t>();
			for (const auto& x : *static_cast<array_t*>(v.second.get())) {
				arr->push_back(std::make_unique<dyn_struct>(*x));
			}

			values[k] =
				std::pair{ Kind::array, std::unique_ptr<void>((void*)arr.release()) };
			break;
#undef X
		default:
			assert("SHOULD NOT HAPPEN PANIC :(");
			break;
		}
	}

}

dyn_struct& dyn_struct::operator=(const dyn_struct& copy) noexcept {
	*this = dyn_struct(copy);
}*/