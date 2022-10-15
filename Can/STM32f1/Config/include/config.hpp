#pragma once

#include <CRSLib/include/std_int.hpp>
#include <CRSLib/include/utility.hpp>

#include <CRSLib/Can/STM32f1/include/hal_can.hpp>
#include <CRSLib/Can/STM32f1/include/utility.hpp>

namespace CRSLib::Can::Config
{
	// フィルタバンクの個数
	inline constexpr u32 filter_bank_total_size = 14;

	// DualCANを使うかどうか
	inline constexpr bool use_dual_can = false;
}
