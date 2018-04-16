#pragma once

#include <stdint.h>

struct setpara_t
{
	uint16_t paramid;
	uint16_t value;
};

void set(setpara_t& para);

