#pragma once
#include <stdint.h>

namespace buma
{
namespace input
{

struct INPUT_PRESS_PARAMS
{
	uint64_t press_count;	// frame
	float press_time;		// ms
	bool is_release;
};

}
}
