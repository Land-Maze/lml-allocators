#include <cstdio>
#include <iostream>

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	#define LML_BIG_ENDIAN
#else
	#define LML_LITTLE_ENDIAN
#endif

#define LML_TESTING
#include <arena_allocator.h>

namespace
{
	template <typename T>
	void print_ptr(T& obj_ptr, const char* name)
	{
		printf("%s_ptr=%p\n", name, obj_ptr);
	}

	struct TestStruct
	{
		/*
		 * Size: 132
		 * Aligned Size: 160
		 */
		alignas(32)

		int32_t x;
		int64_t y[16];
	};

	bool test_padding()
	{
		bool test_failed = false;

		std::printf("\n===STARTING TEST PADDING===\n");

		constexpr lml::ArenaAllocatorCreateInfo allocator_create_info{
			.size = 1024
		};
		lml::ArenaAllocator allocator(allocator_create_info);

		auto str_ptr = reinterpret_cast<intptr_t>(allocator.alloc<TestStruct>());
		auto int_ptr = reinterpret_cast<intptr_t>(allocator.alloc<int>());

		// print_ptr(str_ptr, "struct");
		// print_ptr(int_ptr, "int");

		if (int_ptr - str_ptr != 160) test_failed |= 1;

		return test_failed;
	}

	bool test_marking()
	{
		bool test_failed = false;

		std::printf("\n===STARTING TEST MARKING===\n");

		constexpr lml::ArenaAllocatorCreateInfo allocator_create_info{
			.size = 1024
		};
		lml::ArenaAllocator allocator(allocator_create_info);

		auto* x = allocator.alloc<int>();
		auto* y = allocator.alloc<TestStruct>();

		const auto mark = allocator.get_marker();

		auto* z = allocator.alloc<uint32_t>();
		*z = UINT32_MAX;

		allocator.rollback(mark);

		auto* zz = allocator.alloc<uint16_t>();
		auto* zy = allocator.alloc<uint16_t>();

		*zy = UINT16_MAX ^ UINT8_MAX;

		if (*zz != UINT16_MAX) test_failed |= 1;

#ifdef LML_LITTLE_ENDIAN
		if (*z != 0b11111111000000001111111111111111) test_failed |= 1;
#elif LML_BIG_ENDIAN
		if (*z != 0b11111111111111110000000011111111) test_failed |= 1;
#endif

		return test_failed;
	}

	bool test_size()
	{
		bool test_failed = false;

		std::printf("\n===STARTING TEST SIZE===\n");

		constexpr lml::ArenaAllocatorCreateInfo allocator_create_info{
			.size = 1024
		};
		lml::ArenaAllocator allocator(allocator_create_info);

		allocator.alloc<uint32_t>(1024/sizeof(uint32_t));

		if (allocator.remaining_size() != 0) test_failed |= 1;

		return test_failed;
	}

	void test_frying_ram()
	{
		constexpr lml::ArenaAllocatorCreateInfo allocator_create_info{
			.size = 1ull << 34
		};
		lml::ArenaAllocator allocator(allocator_create_info);
		void* x;
		std::cin >> x;
	}
}

int main()
{
	const auto is_padding_wrong = test_padding();
	std::printf("is_padding_wrong=%d", is_padding_wrong);

	const auto is_marking_wrong = test_marking();
	std::printf("is_marking_wrong=%d", is_marking_wrong);

	const auto is_size_wrong = test_size();
	std::printf("is_size_wrong=%d", is_size_wrong);

	test_frying_ram();
}
