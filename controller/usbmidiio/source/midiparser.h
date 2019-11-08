#ifndef MIDIPARSER_H_
#define MIDIPARSER_H_

#include <stdint.h>
#include "midicmd.h"

enum {
	NoteOff,
	NoteOn,
	Aftertouch,
	ControlChange,
	PatchChange,
	ChannelPressure,
	PitchWheel,
	SysexBegin,
	MTCQuarterFrame,
	SongPosition,
	SongSelect,
	TuneRequest,
	SysexEnd
} modes;

typedef struct _midiin_state_t
{
	uint8_t midiin_status;
	uint8_t midiin_header;
	uint8_t midiin_uint8_ts[3];
	int midiin_count;
	int midiin_length;

	int (*GetCharsInRxBuf) (void);
	uint8_t (*RecvChar) (uint8_t *Chr);
} midiin_state_t;

void midiin_reset(midiin_state_t* state);
int midiin_process(midicmd_t* cmd, uint8_t b, midiin_state_t* state);

int midiin_receive(midicmd_t* cmd, midiin_state_t* state);

typedef struct _midiout_state_t
{
	int midiout_laststatus;

	int OutBufSize;
	int(*GetCharsInTxBuf) (void);
	uint8_t (*SendChar) (uint8_t);

	uint8_t midiout_counterhandle;
	bool (*CounterExpired) (uint8_t);
	int16_t (*SetCounter)(uint8_t, int16_t);

} midiout_state_t;

void midiout_reset(midiout_state_t *state);
int midiout_can_transmit(int numbytes, midiout_state_t* state);
void midiout_transmit(const midicmd_t cmd, void* _state);

#endif /* MIDIPARSER_H_ */
