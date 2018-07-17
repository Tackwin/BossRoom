#include "Removable.hpp"

void Removable::remove() noexcept {
	_toRemove = true;
}
bool Removable::toRemove() const noexcept {
	return _toRemove;
}