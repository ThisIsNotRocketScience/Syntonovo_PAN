#pragma once

#include <boost/unordered_map.hpp>
#include <string>

boost::unordered_map<std::string, int> parammap;
boost::unordered_map<std::string, int> switchmap;

void synth_init()
{
#define OUTPUT(NAME, PGROUP, PID, CTRLTYPE, CTRLID, MODE, INITVALUE) \
	parammap[#NAME] = CTRLID;
#define OUTPUT_VIRT(NAME, PGROUP, PID, CTRLTYPE, CTRLID, MODE, INITVALUE) \
	parammap[#NAME] = CTRLID;
#define SWITCH(NAME, ID) \
	switchmap[#NAME] = ID;
#include "paramdef.h"
#undef OUTPUT
#undef OUTPUT_VIRT
#undef SWITCH
}
