#pragma once
#include <algorithm>
#include <cstdint>
#include <vector>
#include <memory>
#include <cmath>

// trying the singleton thing
// and snake_case sssssssss
class MemoryManager {
	using u08 = std::uint8_t;

	struct mem_place {
		u08* location;
		std::size_t size;
	};
public:

	static MemoryManager& I();

	void initialize_buffer(size_t size);

	template<typename T, class... Args>
	T* allocate(Args... args) {
		printf("allocate\n");

		u08* emplacement = nullptr;
		for (size_t i = 0u; i < _free_memory.size(); ++i) { //testing if the memory place is big enough to hold T

			mem_place free_place = _free_memory[i];
			mem_place free_place_aligned = _free_memory[i];

			void* free_place_aligned_location = (void*)free_place_aligned.location;

			if (!std::align(alignof(T), sizeof(T), free_place_aligned_location, free_place_aligned.size)) 
				continue; // if we can't properly align the memory place it's no use

			free_place_aligned.location = (u08*)free_place_aligned_location;

			if (sizeof(T) <= free_place.size) { // if it's big enough to hold T then let's use it !

				if (free_place.location != free_place_aligned.location) { //if the aligned ptr is not the normal one, then there's a new memory place at the begining
					_free_memory.push_back({ free_place.location, free_place.size - free_place_aligned.size });
					_free_memory[i].location = (u08*)free_place_aligned_location; // and we update the old one to start at the aligned place
				}

				if (sizeof(T) < free_place.size) // so if it isn't _exactly_ the size there's a new place at the end
					_free_memory.insert(_free_memory.begin() + i + 1, {free_place.location + sizeof(T), free_place.size - sizeof(T)});

				_free_memory.erase(_free_memory.begin() + i); // we delete the memory place that we use

				std::sort(_free_memory.begin(), _free_memory.end(), [](const mem_place& A, const mem_place& B) -> bool {
					return A.location < B.location; 
				});
				
				emplacement = free_place.location;
				break;
			}

		}

		if (!emplacement) {
			throw std::runtime_error("Not enough place");
		}
		
		return new(emplacement) T(args...);
	}

	template<typename T>
	void deallocate(T* ptr, size_t size = sizeof(T)) {
		printf("deallocate\n");
	
		bool flag = true;

		for (size_t i = 0u; i < _free_memory.size(); ++i) { // if the memory place is just following another, we merge them

			mem_place free_place = _free_memory[i];
			
			if ((std::size_t)free_place.location > sizeof(T) && (free_place.location - sizeof(T)) == (u08*)ptr) {
				ptr->~T();
				_free_memory.erase(_free_memory.begin() + i);
				_free_memory.insert(_free_memory.begin() + i, {free_place.location - sizeof(T), free_place.size + sizeof(T)});
				flag = false;
				break;
			}

		}

		if (flag) { // else we just push a new memory place
			_free_memory.push_back({ (u08*)ptr, size });
		}

		std::sort(_free_memory.begin(), _free_memory.end(), [](const mem_place& A, const mem_place& B) -> bool {
			return A.location < B.location;
		});
	}

private:
	MemoryManager();
	~MemoryManager();

	MemoryManager(const MemoryManager&) = delete;
	MemoryManager& operator=(const MemoryManager&) = delete;

	u08* _main_buffer = nullptr;

	std::vector<mem_place> _free_memory;
};

template <typename T>
struct shrd_ptr : std::shared_ptr<T> {

	template<class... Args>
	shrd_ptr(Args... args) :
		std::shared_ptr<T>(
			MemoryManager::I().allocate<T>(args...),
			std::bind(&MemoryManager::deallocate<T>, &MemoryManager::I(), std::placeholders::_1, sizeof(T))
		)
	{}

};