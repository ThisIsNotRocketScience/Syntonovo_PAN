#pragma once

enum output_t {
#define OUTPUT(name,codecport,codecpin, type,id, style,defaultvalue,dummy1,dummy2) \
	output_##name = id,

#define OUTPUT_VIRT(name,codecport,codecpin, type,id, style,defaultvalue,dummy1,dummy2) \
	output_##name = id,

#include "../../../interface/paramdef.h"

#undef OUTPUT
#undef OUTPUT_VIRT
};


enum switch_t {
#define SWITCH(name,id,DEFAULT) \
	switch_##name = id,

#include "../../../interface/paramdef.h"

#undef SWITCH
};

enum modsource_t {
#define MODSOURCE(name,type,id) \
	modsource_##name = id,

#include "../../../interface/paramdef.h"

#undef MODSOURCE
};
