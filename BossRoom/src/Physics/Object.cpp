#include <Physics/Object.hpp>

uint64_t Object::N = 0u;

Object::Object() :
id(N++) {

}
