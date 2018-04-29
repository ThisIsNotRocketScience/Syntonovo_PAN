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

#include "synth_internal_defs.h"

////////////////////////////////////////////////////////////////
// Parameter tables

typedef struct _param_t
{
	uint16_t port;
	uint16_t flags;

	uint16_t last;

	uint16_t value;

	uint16_t lfo_speed;
	int16_t lfo_depth;
	int16_t lfo_shape;
	uint16_t lfo_reset_phase;

	uint16_t adsr_a;
	uint16_t adsr_d;
	uint16_t adsr_s;
	uint16_t adsr_r;
	int16_t adsr_depth;

	uint16_t ad_a;
	uint16_t ad_d;
	int16_t ad_depth;

	int16_t x;
	int16_t y;
	int16_t z;
	int16_t zprime;
	int16_t note;
	int16_t vel;
	int16_t pad_l;
	int16_t pad_r;
} param_t;

enum SubParamFlags_t
{
	SubParamFlags_AdsrRetrigger = 1,
	SubParamFlags_AdRetrigger = 2,
	SubParamFlags_LfoRetrigger = 4,

	SubParamFlags_max = 0xffff
};

#define SYNTH_PARAM_COUNT (256)
param_t synth_param[SYNTH_PARAM_COUNT];

#if 0
#define CTRL(id) (id)
#define INT(id) (0xFFFF)

#define OUTPUT(NAME, PGROUP, PID, CTRLTYPE, CTRLID, MODE, INITVALUE) \
{ .port = BASE_##PGROUP + PID, \
  .last = 0xfff0 },
#define OUTPUT_VIRT(NAME, PGROUP, PID, CTRLTYPE, CTRLID, MODE, INITVALUE) \
{ .port = BASE_##PGROUP + PID, \
  .last = 0xfff0 },

#define SWITCH(NAME, ID)

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
	const int PORT_##NAME = PORTID;
#define MAPPING_INT(ID, NAME, PORTID) \
	const int NAME = ID; \
	const int PORT_##NAME = PORTID;

#define SWITCH(NAME, ID) \
	const int NAME = ID;

#define OUTPUT(NAME, PGROUP, PID, CTRLTYPE, CTRLID, MODE, INITVALUE) \
	MAPPING_##CTRLTYPE(CTRLID, NAME, BASE_##PGROUP + PID)
#define OUTPUT_VIRT(NAME, PGROUP, PID, CTRLTYPE, CTRLID, MODE, INITVALUE) \
	MAPPING_##CTRLTYPE(CTRLID, NAME, 0)

#include "paramdef.h"

#undef MAPPING_CTRL
#undef MAPPING_INT
#undef OUTPUT
#undef OUTPUT_VIRT
#undef SWITCH


////////////////////////////////////////////////////////////////
// Virtual parameter handler forward declarations

#define DO_OUTPUT_LIN(NAME, CTRLID, PID)
#define DO_OUTPUT_INV(NAME, CTRLID, PID)
#define DO_OUTPUT_LOG(NAME, CTRLID, PID)
#define DO_OUTPUT_CUSTOM(NAME, CTRLID, PID) \
	void do_output_##NAME(int ctrlid, int pid);
#define OUTPUT(NAME, PGROUP, PID, CTRLTYPE, CTRLID, MODE, INITVALUE) \
	DO_OUTPUT_##MODE(NAME, CTRLID, BASE_##PGROUP + PID);
#define OUTPUT_VIRT(NAME, PGROUP, PID, CTRLTYPE, CTRLID, MODE, INITVALUE) \
	void virt_##NAME();
#define SWITCH(NAME, ID)

#include "paramdef.h"

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

#define OUTPUT(NAME, PGROUP, PID, CTRLTYPE, CTRLID, MODE, INITVALUE) \
	memset(&synth_param[CTRLID], 0, sizeof(synth_param[0])); \
	synth_param[CTRLID].port = BASE_##PGROUP + PID; \
	synth_param[CTRLID].last = 0xfff0; \
	synth_param[CTRLID].value = INITVALUE;
#define OUTPUT_VIRT(NAME, PGROUP, PID, CTRLTYPE, CTRLID, MODE, INITVALUE) \
	memset(&synth_param[CTRLID], 0, sizeof(synth_param[0])); \
	synth_param[CTRLID].port = BASE_##PGROUP + PID; \
	synth_param[CTRLID].last = 0xfff0; \
	synth_param[CTRLID].value = INITVALUE;

#define SWITCH(NAME, ID)

#include "paramdef.h"

#undef OUTPUT
#undef OUTPUT_VIRT
#undef SWITCH

	synth_mapping_defaultpatch();
}

void synth_mapping_run()
{
#define DO_OUTPUT_LIN(NAME, CTRLID, PID) \
	do_output_lin(CTRLID, PID);
#define DO_OUTPUT_INV(NAME, CTRLID, PID) \
	do_output_inv(CTRLID, PID);
#define DO_OUTPUT_LOG(NAME, CTRLID, PID) \
	do_output_log(CTRLID, PID);
#define DO_OUTPUT_CUSTOM(NAME, CTRLID, PID) \
	do_output_##NAME(CTRLID, PID);
#define OUTPUT(NAME, PGROUP, PID, CTRLTYPE, CTRLID, MODE, INITVALUE) \
	DO_OUTPUT_##MODE(NAME, CTRLID, BASE_##PGROUP + PID);
#define OUTPUT_VIRT(NAME, PGROUP, PID, CTRLTYPE, CTRLID, MODE, INITVALUE) \
	virt_##NAME();

#define SWITCH(NAME, ID)

#include "paramdef.h"

#undef DO_OUTPUT_LIN
#undef DO_OUTPUT_INV
#undef DO_OUTPUT_LOG
#undef DO_OUTPUT_CUSTOM
#undef OUTPUT
#undef OUTPUT_VIRT
#undef SWITCH
}

#endif /* SYNTH_INTERNAL_H_ */
