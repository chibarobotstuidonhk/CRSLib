#include <CRSLib/include/executor.hpp>
#include <CRSLib/Can/CommonAmongMpu/include/pack.hpp>
#include <CRSLib/Can/STM32f1/include/hal_can.hpp>
#include <CRSLib/Can/STM32f1/include/can_manager.hpp>
#include <CRSLib/Can/STM32f1/include/filter_manager.hpp>

#include "../include/sample.hpp"

namespace Chibarobo2022
{
	CRSLib::Executor<void () noexcept, 100> executor{};
}

using namespace CRSLib;
using namespace Chibarobo2022;
using namespace CRSLib::Can;
using namespace CRSLib::Can::STM32f1;


extern "C"
{
	void sample_run(CAN_HandleTypeDef *const hcan) noexcept
	{

#ifndef stew_SOURCETRAIL
		CanManager can_manager{hcan};

		FilterManager::dynamic_initialize();
#endif

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

#ifndef stew_SOURCETRAIL
		FilterManager::config_filter_bank(15, filter_arg);
#endif

		HAL_CAN_Start(hcan);

		// タイマ割り込みでexecutorにreceiver.receiveやtransmitter.transmitを詰め込んでwhile文内はexecutor.run_once()だけのが効率よく動くかも。
		while(true)
		{
#ifndef stew_SOURCETRAIL
			receiver.receive(can_manager.letterbox0, executor);
			receiver.receive(can_manager.letterbox1, executor);
			transmitter.transmit(can_manager.pillarbox);
#endif
			executor.run_once();
		}
	}
}
