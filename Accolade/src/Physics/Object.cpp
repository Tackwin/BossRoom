#include "Object.hpp"

u64 Object::N = 0u;

Object::Object() : uuid() {
	N++;
}
