/*
 * synth.h
 *
 *  Created on: Mar 4, 2018
 *      Author: ld0d
 */

#ifndef SYNTH_INTERNAL_H_
#define SYNTH_INTERNAL_H_

void synth_mapping_defaultpatch();
void do_output_lin(int ctrlid, int port);
void do_output_inv(int ctrlid, int port);
void do_output_log(int ctrlid, int port);

void do_smooth(int ctrlid);

#define PORTID(PGROUP, PID) (((PGROUP)&1) | ((PID)<<1) | (((PGROUP)>>1) << 4))

#include "synth_internal_defs.h"

////////////////////////////////////////////////////////////////
// Parameter tables

#include "synth_param.h"

param_t synth_param[SYNTH_PARAM_COUNT];

// 16* speed mod
// 16* shape mod
param_t lfo_mod[NUM_LFOS*2];

// 7 bits used;
//   0 := DO1, DO2
// 1:3 := S1-S6
// 4:6 := A, B, C
int synth_output[512];

#if 0
#define CTRL(id) (id)
#define INT(id) (0xFFFF)

#define OUTPUT(NAME, PGROUP, PID, CTRLTYPE, CTRLID, MODE, INITVALUE) \
{ .port = PGROUP + (PID << 1), \
  .last = 0xfff0 },
#define OUTPUT_VIRT(NAME, PGROUP, PID, CTRLTYPE, CTRLID, MODE, INITVALUE) \
{ .port = -1, \
  .last = 0xfff0 },

#define SWITCH(NAME, ID, DEFAULT, DESC)

#define SYNTH_PARAM_COUNT (256)
param_t synth_param[SYNTH_PARAM_COUNT] =
{
#include "paramdef.h"
};

#undef CTRL
#undef INT
#undef OUTPUT
#undef OUTPUT_VIRT
#undef SWITCH
#endif
////////////////////////////////////////////////////////////////
// Controller mappings

#include "synth_internal_ctrlmap.h"

#define MAPPING_CTRL(ID, NAME, PORTID) \
	const int NAME = ID; \
	const int PORT_##NAME = PORTID; \
	const int PARAM_##ID = PORTID;
#define MAPPING_INT(ID, NAME, PORTID) \
	const int NAME = ID; \
	const int PORT_##NAME = PORTID; \
	const int PARAM_##ID = PORTID;

#define SWITCH(NAME, ID, DEFAULT, DESC) \
	const int NAME = ID;

#define OUTPUT(NAME, PGROUP, PID, CTRLTYPE, CTRLID, MODE, INITVALUE, dummy1, dummy2) \
	MAPPING_##CTRLTYPE(CTRLID, NAME, PORTID(PGROUP, PID))
#define OUTPUT_VIRT(NAME, PGROUP, PID, CTRLTYPE, CTRLID, MODE, INITVALUE, dummy1, dummy2) \
	MAPPING_##CTRLTYPE(CTRLID, NAME, 0)

#include "../../interface/paramdef.h"

#undef MAPPING_CTRL
#undef MAPPING_INT
#undef OUTPUT
#undef OUTPUT_VIRT
#undef SWITCH


////////////////////////////////////////////////////////////////
// Modmatrix

modmatrix_row_t modmatrix[SYNTH_MODSOURCE_COUNT];
lfo_param_t lfo_param[NUM_LFOS];
env_param_t env_param[NUM_ENVS];
controller_param_t controller_param[NUM_CONTROLLERS];
operator_param_t op_param[NUM_OPERATORS];
key_mapping_t key_mapping[NUM_KEY_MAP_TARGETS];

////////////////////////////////////////////////////////////////
// Virtual parameter handler forward declarations

#define DO_OUTPUT_LIN(NAME, CTRLID, PID)
#define DO_OUTPUT_INV(NAME, CTRLID, PID)
#define DO_OUTPUT_LOG(NAME, CTRLID, PID)
#define DO_OUTPUT_CUSTOM(NAME, CTRLID, PID) \
	void do_output_##NAME(int ctrlid, int pid);
#define OUTPUT(NAME, PGROUP, PID, CTRLTYPE, CTRLID, MODE, INITVALUE, dummy1, dummy2) \
	DO_OUTPUT_##MODE(NAME, CTRLID, PORTID(PGROUP, PID));
#define OUTPUT_VIRT(NAME, PGROUP, PID, CTRLTYPE, CTRLID, MODE, INITVALUE, dummy1, dummy2) \
	void virt_##NAME();
#define SWITCH(NAME, ID, DEFAULT, DESC)

#include "../../interface/paramdef.h"

#undef DO_OUTPUT_LIN
#undef DO_OUTPUT_INV
#undef DO_OUTPUT_LOG
#undef DO_OUTPUT_CUSTOM
#undef OUTPUT
#undef OUTPUT_VIRT
#undef SWITCH

//static int synth_map[0xFF] = {0};

void synth_mapping_init()
{
	memset(synth_param, 0xff, sizeof(synth_param));
	memset(synth_output, 0xff, sizeof(synth_output));

#define OUTPUT(NAME, PGROUP, PID, CTRLTYPE, CTRLID, MODE, INITVALUE, dummy1, dummy2) \
	memset(&synth_param[CTRLID], 0, sizeof(synth_param[0])); \
	synth_param[CTRLID].port = PORTID(PGROUP, PID); \
	synth_param[CTRLID].last = 0xfff0; \
	synth_param[CTRLID].value = INITVALUE; \
	synth_param[CTRLID].target = INITVALUE; \
	synth_param[CTRLID].add = 0; \
	synth_output[synth_param[CTRLID].port] = CTRLID;
#define OUTPUT_VIRT(NAME, PGROUP, PID, CTRLTYPE, CTRLID, MODE, INITVALUE, dummy1, dummy2) \
	memset(&synth_param[CTRLID], 0, sizeof(synth_param[0])); \
	synth_param[CTRLID].port = -1; \
	synth_param[CTRLID].last = 0xfff0; \
	synth_param[CTRLID].value = INITVALUE; \
	synth_param[CTRLID].target = INITVALUE; \
	synth_param[CTRLID].add = 0;

#define SWITCH(NAME, ID, DEFAULT, DESC)

#include "../../interface/paramdef.h"

#undef OUTPUT
#undef OUTPUT_VIRT
#undef SWITCH

	synth_mapping_defaultpatch();
}

void synth_mapping_reset()
{
#define OUTPUT(NAME, PGROUP, PID, CTRLTYPE, CTRLID, MODE, INITVALUE, dummy1, dummy2) \
		synth_param[CTRLID].sum = 0;
#define OUTPUT_VIRT(NAME, PGROUP, PID, CTRLTYPE, CTRLID, MODE, INITVALUE, dummy1, dummy2) \
		synth_param[CTRLID].sum = 0;
#include "../../interface/paramdef.h"
#undef OUTPUT
#undef OUTPUT_VIRT
}

void synth_mapping_run()
{
#define DO_OUTPUT_LIN(NAME, CTRLID, PID) \
	do_smooth(CTRLID); \
	do_output_lin(CTRLID, PID);
#define DO_OUTPUT_INV(NAME, CTRLID, PID) \
	do_smooth(CTRLID); \
	do_output_inv(CTRLID, PID);
#define DO_OUTPUT_LOG(NAME, CTRLID, PID) \
	do_smooth(CTRLID); \
	do_output_log(CTRLID, PID);
#define DO_OUTPUT_CUSTOM(NAME, CTRLID, PID) \
	do_output_##NAME(CTRLID, PID);
#define OUTPUT(NAME, PGROUP, PID, CTRLTYPE, CTRLID, MODE, INITVALUE, dummy1, dummy2) \
	DO_OUTPUT_##MODE(NAME, CTRLID, PORTID(PGROUP, PID));

#define OUTPUT_VIRT(NAME, PGROUP, PID, CTRLTYPE, CTRLID, MODE, INITVALUE, dummy1, dummy2)

#include "../../interface/paramdef.h"

#undef DO_OUTPUT_LIN
#undef DO_OUTPUT_INV
#undef DO_OUTPUT_LOG
#undef DO_OUTPUT_CUSTOM
#undef OUTPUT
#undef OUTPUT_VIRT

#define OUTPUT_VIRT(NAME, PGROUP, PID, CTRLTYPE, CTRLID, MODE, INITVALUE, dummy1, dummy2) \
	virt_##NAME();

#define OUTPUT(NAME, PGROUP, PID, CTRLTYPE, CTRLID, MODE, INITVALUE, dummy1, dummy2)
#define SWITCH(NAME, ID, DEFAULT, DESC)

#include "../../interface/paramdef.h"

#undef OUTPUT
#undef OUTPUT_VIRT
#undef SWITCH
}

#endif /* SYNTH_INTERNAL_H_ */
