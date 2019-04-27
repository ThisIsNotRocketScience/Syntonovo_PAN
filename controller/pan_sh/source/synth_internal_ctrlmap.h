/*
 * synth_internal_ctrlmap.h
 *
 *  Created on: Apr 1, 2018
 *      Author: ld0d
 */

#ifndef SYNTH_INTERNAL_CTRLMAP_H_
#define SYNTH_INTERNAL_CTRLMAP_H_

#include "synth_internal_defs.h"

////////////////////////////////////////////////////////////////
// Controller mappings

#define MAPPING_CTRL(ID, NAME, PORTID) \
	extern const int NAME; \
	extern const int PORT_##NAME;
#define MAPPING_INT(ID, NAME, PORTID) \
	extern const int PORT_##NAME;

#define OUTPUT(NAME, PGROUP, PID, CTRLTYPE, CTRLID, MODE, INITVALUE) \
	MAPPING_##CTRLTYPE(CTRLID, NAME, BASE_##PGROUP + PID)
#define OUTPUT_VIRT(NAME, PGROUP, PID, CTRLTYPE, CTRLID, MODE, INITVALUE) \
	MAPPING_##CTRLTYPE(CTRLID, NAME, 0)

#define SWITCH(NAME, ID, DEFAULT) \
	extern const int NAME;

#include "../../interface/paramdef.h"

#undef MAPPING_CTRL
#undef MAPPING_INT
#undef OUTPUT
#undef OUTPUT_VIRT
#undef SWITCH

#endif /* SYNTH_INTERNAL_CTRLMAP_H_ */
