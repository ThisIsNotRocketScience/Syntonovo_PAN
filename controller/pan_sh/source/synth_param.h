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
	int16_t pad_l_depth;
	int16_t pad_r_depth;
	int16_t sus_l_depth;
	int16_t sus_r_depth;
	int16_t unac_l_depth;
	int16_t unac_r_depth;
} param_t;

enum SubParamFlags_t
{
	SubParamFlags_AdsrRetrigger = 1,
	SubParamFlags_AdRetrigger = 2,
	SubParamFlags_LfoRetrigger = 4,

	SubParamFlags_max = 0xffff
};

#define SYNTH_PARAM_COUNT (256)
extern param_t synth_param[SYNTH_PARAM_COUNT];

#endif /* SYNTH_PARAM_H_ */
