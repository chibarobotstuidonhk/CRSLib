// decobsなデータ [a, b, c, d]
// cobsなデータ [distance_to_next_zero, a, b, c, d, 0]  0終端を忘れずに

#pragma once

#include <vector>

#include <CRSLib/include/std_int.hpp>

namespace CRSLib::Usb
{
	inline std::vector<u8> cobs(const std::vector<u8>& frame) noexcept
	{
		std::vector<u8> ret(frame.size() + 2, 0);
		
		u8 * previous_zero_element = &ret[0];
		size_t count = 1;  // 次の要素が0なら、その0だった要素には1が入る
		for(size_t i = 0; i < frame.size(); ++i)
		{
			if(frame[i] != 0)
			{
				ret[i + 1] = frame[i];
				++count;
			}
			else
			{
				*previous_zero_element = count;
				previous_zero_element = &ret[i + 1];
				count = 1;
			}
		}

		return ret;
	}

	inline std::vector<u8> decobs(const std::vector<u8>& cobsed_frame) noexcept
	{
		std::vector<u8> ret(cobsed_frame.size() - 2);
		
		size_t next_zero = cobsed_frame[0];
		for(size_t i = 0; i < ret.size(); ++i)
		{
			if(next_zero == 1)
			{
				next_zero = cobsed_frame[i + 1];
				ret[i] = 0;
			}
			else
			{
				ret[i] = cobsed_frame[i + 1];
				--next_zero;
			}
		}

		return ret;
	}
}