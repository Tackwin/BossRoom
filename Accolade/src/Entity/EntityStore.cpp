#include "EntityStore.hpp"
#include "Ruins/Section.hpp"
#include "Eid.hpp"

EntityStore& EntityStore::I() noexcept {
	static EntityStore e;
	return e;
}

EntityStore& ES = EntityStore::I();