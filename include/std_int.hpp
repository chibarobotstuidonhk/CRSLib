#pragma once

#include <cstddef>
#include <cstdint>

namespace CRSLib
{
	namespace IntegerTypes
	{
#define Stew_define_standard_integer_type(bit_size)\
		using i##bit_size = std::int##bit_size##_t;\
		using u##bit_size = std::uint##bit_size##_t

		Stew_define_standard_integer_type(8);
		Stew_define_standard_integer_type(16);
		Stew_define_standard_integer_type(32);
		Stew_define_standard_integer_type(64);

#undef Stew_define_standard_integer_type

		using i_ptr = std::intptr_t;
		using ptrdiff_t = std::ptrdiff_t;
		using size_t = std::size_t;
		using nullptr_t = std::nullptr_t;
		using max_align_t = std::max_align_t;
#if __cplusplus >= 201703L
		using byte = std::byte;
#endif
	}

	using namespace IntegerTypes;
}
