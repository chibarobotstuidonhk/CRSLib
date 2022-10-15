#pragma once

#include "hal_can.hpp"

#include <CRSLib/include/utility.hpp>

#include <CRSLib/include/std_int.hpp>
#include <CRSLib/Can/CommonAmongMpu/include/utility.hpp>

#include "utility.hpp"

namespace CRSLib::Can::STM32f1
{
	struct RxHeader final
	{
		u32 id;
		u32 time_stamp;
		u32 filter_match_index;
		u8 dlc;
		bool rtr;

		constexpr u32 get_id() const noexcept
		{
			return id;
		}
	};

	struct RxFrame final
	{
		RxHeader header{};
		DataField data{};
	};

	class Letterbox final
	{
		CAN_HandleTypeDef * hcan;
		FifoIndex fifo_index;

	public:
		Letterbox(CAN_HandleTypeDef *const hcan, const FifoIndex fifo_index) noexcept:
			hcan{hcan},
			fifo_index{fifo_index}
		{}

		void receive(RxFrame& frame) noexcept
		{
			CAN_RxHeaderTypeDef rx_header{};

			HAL_CAN_GetRxMessage(hcan, to_underlying(fifo_index), &rx_header, frame.data.data());
			frame.header =
			{
				(rx_header.ExtId << (u32)11) | rx_header.StdId,
				rx_header.Timestamp,
				rx_header.FilterMatchIndex,
				static_cast<u8>(rx_header.DLC),
				rx_header.RTR == CAN_RTR_REMOTE
			};
		}

		bool empty() const noexcept
		{
			return HAL_CAN_GetRxFifoFillLevel(hcan, to_underlying(fifo_index)) == 0;
		}
	};
}
