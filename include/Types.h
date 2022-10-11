#pragma once

#include <stdint.h>

//namespace Lf
//{
	struct Coordinate
	{
		int x = 0;
		int y = 0;

		Coordinate(int x, int y)
			: x(x), y(y) {}

	};

	typedef uint8_t state_t;
//}
