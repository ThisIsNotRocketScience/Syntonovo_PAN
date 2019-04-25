#pragma once
#include <stdint.h>

#pragma pack(1)

#ifdef WIN32
#define PACK
#else
#define PACK  __attribute__((packed))
#endif


typedef struct {
	uint8_t param;
	uint16_t depth;
} PACK ModTarget_t;

typedef struct {
	ModTarget_t target[16];
	uint16_t speed;
	uint16_t shape;
} PACK LfoModulation_t;

typedef struct {
	ModTarget_t target[16];
	uint16_t a;
	uint16_t d;
	uint16_t s;
	uint16_t r;
} PACK AdsrModulation_t;

typedef struct {
	ModTarget_t target[16];
	uint16_t a;
	uint16_t d;
} PACK  AdModulation_t;

enum ModSource_t : unsigned char {
	Source_none,

	Source_left_mod,
	Source_right_mod,
	Source_x,
	Source_y,
	Source_z,
	Source_zprime,
	Source_note,
	Source_vel,
	Source_left_sus,
	Source_right_sus,
	Source_left_unac,
	Source_right_unac,
	__ModSource_COUNT
};

typedef struct {
	ModTarget_t target[16];
	ModSource_t source;
} ControlModulation_t;

#define PRESET_NAME_LENGTH 16

typedef struct {
	uint32_t switches[2];
	uint16_t paramvalue[256];

	LfoModulation_t lfomod[16];
	AdsrModulation_t adsrmod[16];
	AdModulation_t admod[16];
	ControlModulation_t ctrlmod[16];
	char Name[PRESET_NAME_LENGTH];
} PACK PanPreset_t;