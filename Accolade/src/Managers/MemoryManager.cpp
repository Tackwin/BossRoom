#include "Managers/MemoryManager.hpp"


MemoryManager::MemoryManager() {
}
MemoryManager::~MemoryManager() {
	if (!_main_buffer)
		delete[] _main_buffer;
}

MemoryManager& MemoryManager::I() {
	static MemoryManager memory_manager;
	return memory_manager;
}

void MemoryManager::initialize_buffer(size_t size) {
	_main_buffer = new u08[size];
	_free_memory.push_back({ _main_buffer, size });
}

