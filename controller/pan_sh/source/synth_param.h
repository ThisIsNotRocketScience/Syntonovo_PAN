/*
 * synth_param.h
 *
 *  Created on: Oct 14, 2018
 *      Author: Lauri
 */

#ifndef SYNTH_PARAM_H_
#define SYNTH_PARAM_H_

typedef struct _param_t
{
	uint16_t port;
	uint16_t flags;

	uint16_t last;
	int32_t sum;
	uint16_t value;
	uint16_t target;
	int32_t add;

	int16_t note;
} param_t;

enum SubParamFlags_t
{
	SubParamFlags_AdsrRetrigger = 1,
	SubParamFlags_AdRetrigger = 2,
	SubParamFlags_LfoRetrigger = 4,
	SubParamFlags_SmoothUpdate = 8,

	SubParamFlags_max = 0xffff
};

#define SYNTH_PARAM_COUNT (256)
extern param_t synth_param[SYNTH_PARAM_COUNT];

#define MODTARGET_COUNT (11)

typedef struct _modtarget_spec_t
{
	uint16_t depth;
	uint16_t outputid;
	uint8_t sourceid;
} modtarget_spec_t;

typedef struct _modmatrix_row_t
{
	modtarget_spec_t targets[MODTARGET_COUNT];

	//uint16_t last;
	//uint16_t value;
} modmatrix_row_t;

#define SYNTH_MODSOURCE_COUNT (64)

extern modmatrix_row_t modmatrix[SYNTH_MODSOURCE_COUNT];

typedef struct _lfo_param_t
{
	uint16_t flags; // including key range
	uint16_t speed;
	uint16_t depth;
	int16_t shape;
	uint16_t reset_phase;
} lfo_param_t;

#define NUM_LFOS (16)
extern lfo_param_t lfo_param[NUM_LFOS];

typedef struct _env_param_t
{
	uint16_t flags; // including env type, key range
	uint16_t a;
	uint16_t d;
	uint16_t s;
	uint16_t r;
	uint16_t curve;
	uint16_t depth;
} env_param_t;

#define NUM_ENVS (16)
extern env_param_t env_param[NUM_ENVS];

typedef struct _controller_param_t
{
	int16_t scale;
	int16_t deadzone;
	//int16_t shape;
} controller_param_t;

#define NUM_CONTROLLERS (11)
extern controller_param_t controller_param[NUM_CONTROLLERS];

typedef struct _operator_param_t {
	uint8_t modsource1;
	uint8_t modsource2;
	uint16_t op; // tbd plus, minus, mul, diff, sh, lag, etc.
	uint16_t parameter; // parameter for selected op
} operator_param_t;

#define NUM_OPERATORS (16)
extern operator_param_t op_param[NUM_OPERATORS];

#endif /* SYNTH_PARAM_H_ */
