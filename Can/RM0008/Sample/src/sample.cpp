#include <CRSLib/Can/RM0008/include/can_manager.hpp>
#include <CRSLib/Can/RM0008/include/filter_manager.hpp>
#include <CRSLib/Can/RM0008/include/letterbox.hpp>
#include <CRSLib/Can/RM0008/include/pillarbox.hpp>

using namespace CRSLib::Can;
using namespace CRSLib::Can::RM0008;

extern "C"
{
	int main_cpp(CAN_HandleTypeDef *const hcan)
	{
		CanManager can_manager{hcan};

		FilterManager::dynamic_initialize();

		FilterManager::ConfigFilterArg<FilterWidth::bit32, FilterMode::mask> filter_arg
		{
			{
				{
					.id = {0x101, 0x0, false, false},
					.mask = {max_std_id, max_ext_id, true, true}
				}
			},
			FifoIndex::fifo0,
			0,
			true
		};

		FilterManager::config_filter_bank(15, filter_arg);

		HAL_CAN_Start(hcan);

		while(true)
		{
			RxFrame rx_frame{};
			if(!can_manager.letterbox0.empty())
			{
				can_manager.letterbox0.receive(rx_frame);

				TxFrame tx_frame{{rx_frame.header.dlc}, rx_frame.data};

				if(can_manager.pillarbox.not_full()) can_manager.pillarbox.post(0x100, tx_frame);
			}
		}
	}
}
