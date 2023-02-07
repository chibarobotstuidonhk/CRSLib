#pragma once

#include <concepts>

namespace CRSLib
{
	template<class T>
	concept ErrorReporterC = std::move_constructible<T> &&
	requires(T reporter, const char *const c_str)
	{
		{reporter(c_str)} noexcept;
	};
}