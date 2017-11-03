#include <Physics/Object.hpp>

u64 Object::N = 0u;

Object::Object() :
id(N++) {

}
