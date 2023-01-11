#pragma once

namespace CRSLib
{
	// 参照: https://invensense.tdk.com/wp-content/uploads/2015/02/RM-MPU-9250A-00-v1.6.pdf
	class Mpu9250 final
	{
		enum class Axis
		{
			x,
			y,
			z
		};

		template<Axis axis>
		float read_gyro() const noexcept
		{
			/*TODO*/return 0;
		}

		void 
	};
}