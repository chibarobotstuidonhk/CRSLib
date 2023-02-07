#pragma once

#include <type_traits>
#include <CRSLib/include/std_int.hpp>

namespace CRSLib
{
	template<class T>
	requires std::is_arithmetic_v<T>
	struct PidController final
	{
		struct Parameters final
		{
			T gain_p;
			T gain_i;
			T gain_d;
		} parameters;

		T past_error_total{};
		T last_error{};

		T calculate(const T current_error) const noexcept
		{
			return parameters.gain_p * current_error + parameters.gain_i * past_error_total + parameters.gain_d * (current_error - last_error);
		}

		void update(const T current_error) noexcept
		{
			past_error_total += current_error;
			last_error = current_error;
		}
	};
}