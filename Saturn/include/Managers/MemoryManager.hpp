#pragma once
#include <algorithm>
#include <cstdint>
#include <utility>
#include <type_traits>
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
	template<typename T>
	using unique_ptr = std::unique_ptr<T, void(*)(T*)>;


	template<typename T>
	struct Allocator {
		using value_type = std::remove_cv_t<T>;

		Allocator() = default;

		template<class U>
		constexpr Allocator(const Allocator<U>&) noexcept {};
		

		T* allocate(size_t n) {
			return MemoryManager::I().allocate<T>(n);
		}
		void deallocate(T* ptr, size_t n) {
			MemoryManager::I().deallocate(ptr, n);
		}

		template<class U>
		bool operator==(const Allocator<U>&) const noexcept {
			return true;
		}
		template<class U>
		bool operator!=(const Allocator<U>&) const noexcept {
			return false;
		}

	};


	static MemoryManager& I();

	template<typename T, class... Args>
	static std::shared_ptr<T> make_shared(Args&&... args) {
		const auto& ptr =
			MemoryManager::I().allocate<T>(std::forward<Args>(args)...);

		return std::shared_ptr<T>(
			ptr,
			[](T* ptr) {
				MemoryManager::I().deallocate(ptr);
			}
		);
	}

	template<typename T, class... Args>
	static unique_ptr<T> make_unique(Args&&... args) {
		T* ptr = MemoryManager::I().allocate<T>(std::forward<Args>(args)...);

		return unique_ptr<T>(
			ptr,
			[](T* ptr) {
				MemoryManager::I().deallocate(ptr);
			}
		);
	}


	size_t get_buffer_size() const;
	size_t get_free_size() const;
	void initialize_buffer(size_t size);

	template<typename T>
	T* allocate(size_t n) {
		u08* emplacement = nullptr;

		//testing if the memory place is big enough to hold T
		for (size_t i = 0u; i < _free_memory.size(); ++i) {

			mem_place free_place = _free_memory[i];
			mem_place free_place_aligned = _free_memory[i];

			void* free_place_aligned_location =
				(void*)free_place_aligned.location;

			if (!std::align(
				alignof(T),
				n * sizeof(T),
				free_place_aligned_location,
				free_place_aligned.size
			))
				// if we can't properly align the memory place it's no use
				continue;

			free_place_aligned.location =
				reinterpret_cast<u08*>(free_place_aligned_location);

			// if it's big enough to hold T then let's use it !
			if (sizeof(T) <= free_place_aligned.size) {

				// we remove the old one
				_free_memory.erase(std::begin(_free_memory) + i);

				// so if it isn't _exactly_ the size there's a new place 
				// at the end
				if (n * sizeof(T) < free_place_aligned.size) {
					_free_memory.insert(
						std::begin(_free_memory) + i,
						{
							free_place_aligned.location + n * sizeof(T),
							free_place_aligned.size - n * sizeof(T)
						}
					);
				}

				// if the aligned ptr is not the normal one, then there's
				// a new memory place at the begining
				if (free_place.location != free_place_aligned.location) {
					_free_memory.insert(
						std::begin(_free_memory) + i,
						{
							free_place.location,
							(u32)free_place_aligned.location - (u32)free_place.location
						}
					);
				}

				// all the insertion hsould keep the array sorted

				emplacement = free_place_aligned.location;
				break;
			}
		}

		if (!emplacement) {
			throw(std::bad_alloc());
		}

		return reinterpret_cast<T*>(emplacement);
	}

	template<typename T, class... Args>
	T* create(Args&&... args) {
		auto emplacement = MemoryManager::I().allocate<T>(1);

		return new(emplacement) T(std::forward<Args>(args)...);
	}

	template<typename T>
	void deallocate(T* ptr, size_t n = 1) {
		bool flag = true;

		// if the memory place is just following another, we merge them
		for (size_t i = 0u; i < _free_memory.size(); ++i) {

			mem_place free_place = _free_memory[i];

			if (
				(std::size_t)free_place.location > n * sizeof(T) &&
				(free_place.location - n * sizeof(T)) == (u08*)ptr
			) {
				_free_memory.erase(_free_memory.begin() + i);
				_free_memory.insert(
					_free_memory.begin() + i,
					{
						free_place.location - n * sizeof(T),
						free_place.size + n * sizeof(T)
					}
				);
				flag = false;
				break;
			}

		}

		if (flag) { // else we just push a new memory place
			_free_memory.push_back({ (u08*)ptr, n * sizeof(T) });
		}


		std::sort(
			_free_memory.begin(),
			_free_memory.end(),
			[](const mem_place& A, const mem_place& B) -> bool {
				return A.location < B.location;
			}
		);
	}

private:
	MemoryManager();
	~MemoryManager();

	MemoryManager(const MemoryManager&) = delete;
	MemoryManager& operator=(const MemoryManager&) = delete;

	size_t _main_size = 0u;
	u08* _main_buffer = nullptr;

	std::vector<mem_place> _free_memory;
};

// Optional
using MM = MemoryManager;