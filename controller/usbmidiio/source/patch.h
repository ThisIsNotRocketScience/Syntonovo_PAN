#ifndef PATCH_H_
#define PATCH_H_

typedef struct _param_t
{
	uint16_t flags;
	uint16_t value;
	int16_t note;
	uint16_t reserved;
} param_t;

typedef struct _modtarget_spec_t
{
	uint16_t depth;
	uint16_t outputid;
} modtarget_spec_t;

#define MODTARGET_COUNT (11)

typedef struct _modmatrix_row_t
{
	modtarget_spec_t targets[MODTARGET_COUNT];

	uint16_t reserved[10];
} modmatrix_row_t;

typedef struct _lfo_param_t
{
	uint16_t flags;
	uint16_t speed;
	int16_t depth;
	int16_t shape;
	uint16_t reset_phase;
	uint16_t reserved[3];
} lfo_param_t;

typedef struct _env_param_t
{
	uint16_t flags;
	uint16_t a;
	uint16_t d;
	uint16_t s;
	uint16_t r;
	uint16_t reserved[3];
} env_param_t;

typedef struct _controller_param_t
{
	int16_t scale;
	int16_t deadzone;
	uint16_t reserved[2];
} controller_param_t;

typedef struct _operator_param_t {
	uint8_t modsource1;
	uint8_t modsource2;
	uint16_t op; // tbd plus, minus, mul, diff, sh, lag, etc.
	uint16_t parameter; // parameter for selected op
	uint16_t reserved;
} operator_param_t;


typedef struct _patch_t {
#define NUM_SYNTH_PARAMS (256)
	param_t synth_param[NUM_SYNTH_PARAMS];
#define NUM_MODSOURCES (256)
	modmatrix_row_t modmatrix[NUM_MODSOURCES];
#define NUM_LFOS (16)
	lfo_param_t lfo_param[NUM_LFOS];
#define NUM_ENVS (16)
	env_param_t env_param[NUM_ENVS];
#define NUM_CONTROLLERS (11)
#define NUM_CONTROLLERS_RESERVED (5)
	controller_param_t controller_param[NUM_CONTROLLERS + NUM_CONTROLLERS_RESERVED];
#define NUM_OPERATORS (16)
	operator_param_t op_param[NUM_OPERATORS];

#define NUM_SWITCHES (64)
	uint32_t switches[NUM_SWITCHES/32];
};

#endif /* PATCH_H_ */
