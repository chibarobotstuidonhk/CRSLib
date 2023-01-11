#include <CRSLib/Can/RM0365/include/can_manager.hpp>
#include <CRSLib/Can/RM0365/include/filter_manager.hpp>
#include <CRSLib/Can/RM0365/include/letterbox.hpp>
#include <CRSLib/Can/RM0365/include/pillarbox.hpp>

using namespace CRSLib::IntegerTypes;
using namespace CRSLib::Can;
using namespace CRSLib::Can::RM0365;

extern "C"
{
RxFrame rx_frame{};

	void main_cpp(CAN_HandleTypeDef *const hcan)
	{
		CanManager can_manager{hcan};

		FilterManager::dynamic_initialize();

		Filter<FilterWidth::bit32, FilterMode::mask> filter =
		{
			.masked32 =
			{
				.id = {0x100, 0x0, false, false},
				.mask = {max_std_id, max_ext_id, true, true}
			}
		};

		FilterManager::ConfigFilterArg<FilterWidth::bit32, FilterMode::mask> filter_arg
		{
			.filter = filter,
			.fifo = FifoIndex::fifo0,
			.filter_match_index = 0, // なんでもいい。
			.activate = true
		};

		FilterManager::config_filter_bank(15, filter_arg);
		[[maybe_unused]] const u32 fmi = filter_arg.filter_match_index;

		HAL_CAN_Start(hcan);
		GPIOC->BSRR |= 13 << 16;


		while(true)
		{
			if(!can_manager.letterbox0.empty())
			{
//				can_manager.letterbox0.receive(rx_frame);
				GPIOC->BSRR |= 13;

//				TxFrame tx_frame{{rx_frame.header.dlc}, rx_frame.data};
//
//				if(can_manager.pillarbox.not_full()) can_manager.pillarbox.post(0x101, tx_frame);
			}

			// debug(2022/12/19):
//			TxFrame tx_frame{{8}, {1, 2, 3, 4, 5, 6, 7, 8}};
//			if(can_manager.pillarbox.not_full()) can_manager.pillarbox.post(0x100, tx_frame);
		}
	}
}
