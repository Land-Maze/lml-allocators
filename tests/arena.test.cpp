#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <lml/arena_allocator.h>
#include <bit>
#include <cstdint>

namespace
{
	struct alignas(32) TestStruct
	{
		int32_t x;
		int64_t y[16];
	};
}

TEST_CASE("ArenaAllocator Logic", "[allocator][arena]")
{
	constexpr lml::ArenaAllocatorCreateInfo info{.size = 1024};
	lml::ArenaAllocator allocator(info);

	SECTION("Padding and Alignment")
	{
		auto str_addr = reinterpret_cast<uintptr_t>(allocator.alloc<TestStruct>());
		auto int_addr = reinterpret_cast<uintptr_t>(allocator.alloc<int>());

		/* TestStruct is alignas(32)
		 * Size of struct is 132, but with alignment 32 it is 160
		 */
		CHECK(int_addr - str_addr == 160);
	}

	SECTION("Markers and Rollback")
	{
		allocator.alloc<int>();
		allocator.alloc<TestStruct>();

		const auto mark = allocator.get_marker();

		auto* z = allocator.alloc<uint32_t>();
		*z = 0xFFFFFFFF;

		allocator.rollback(mark);

		auto* zz = allocator.alloc<uint16_t>();
		auto* zy = allocator.alloc<uint16_t>();

		*zz = 0xFFFF;
		*zy = 0xFF00;

		if constexpr (std::endian::native == std::endian::little)
		{
			CHECK(*z == 0xFF00FFFF);
		}
		else
		{
			CHECK(*z == 0xFFFF00FF);
		}
	}

	SECTION("Capacity Management")
	{
		allocator.alloc<uint32_t>(1024 / sizeof(uint32_t));
		CHECK(allocator.remaining_size() == 0);
	}
}