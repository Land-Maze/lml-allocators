#pragma once
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cassert>
#include <cstdio>

namespace lml
{
	struct ArenaAllocatorCreateInfo
	{
		uint64_t size;
	};

	using ArenaAllocatorMark = size_t;

	class ArenaAllocator
	{
	public:
		template <typename T>
		T* alloc(const size_t count = 1)
		{
			return static_cast<T*>(alloc(sizeof(T) * count, alignof(T)));
		}

		void* alloc(const size_t size, const int alignment)
		{
			const uintptr_t current_ptr = reinterpret_cast<uintptr_t>(buffer) + offset;

			const uintptr_t aligned_ptr = (current_ptr + (alignment - 1)) & ~(alignment - 1);

			const size_t next_offset = (aligned_ptr - reinterpret_cast<uintptr_t>(buffer)) + size;
			assert(next_offset <= this->size);
			this->offset = next_offset;
			return reinterpret_cast<void*>(aligned_ptr);
		}

		void reset()
		{
			this->offset = 0;
		}

		[[nodiscard]] size_t remaining_size() const
		{
			return this->size - this->offset;
		}

		ArenaAllocatorMark get_marker()
		{
			return offset;
		}

		void rollback(const ArenaAllocatorMark mark)
		{
			this->offset = mark;
		}

		explicit ArenaAllocator(const ArenaAllocatorCreateInfo& create_info) : offset(0), size(create_info.size)
		{
			this->buffer = static_cast<std::byte*>(::operator new(size));
		}

		~ArenaAllocator()
		{
			::operator delete(buffer);
		}

		ArenaAllocator(ArenaAllocator&) = delete;

	private:
		std::byte* buffer;
		size_t offset;
		size_t size;
	};
}
