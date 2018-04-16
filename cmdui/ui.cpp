#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include <boost/lexical_cast.hpp>

#include "synth.h"
#include "midi.h"
#include "ui.h"

#define SYNTH_PATH "/dev/ttyACM0"

using std::exception;

void parse_file(char* filename);

int verbose = 0;
int synth_fd = -1;

void out(const char* b, int count)
{
	if (verbose) {
		for (int i = 0; i < count; i++)
		{
			if (i > 0) putc(' ', stdout);
			printf("%02X", (int)(uint8_t)b[i]);
		}
		printf("\n");
	}

	write(synth_fd, b, count);
}

void out_reset()
{
	char r = 255;

	for (int i = 0; i < 16; i++) {
		out(&r, 1);
	}
}

void out_init()
{
	synth_fd = open(SYNTH_PATH, O_WRONLY);
	out_reset();
}

class uint32_from_hex   // For use with boost::lexical_cast
{
    uint32_t value;
public:
    operator uint32_t() const { return value; }
    friend std::istream& operator>>( std::istream& in, uint32_from_hex& outValue )
    {
        in >> std::hex >> outValue.value;
    }
};

enum cmd_t
{
	QUIT = 0,
	SKIP,
	SET,
	MIDI_LEARN,
	MIDI_ASSIGN
};

struct midilearn_t
{
	int enable;
};

struct midiassign_t
{
	uint8_t channel;
	uint8_t cc;
	uint16_t paramid;
	uint16_t min;
	uint16_t max;
};

struct input_t
{
	cmd_t cmd;

	union
	{
		setpara_t setpara;
		midilearn_t midilearnpara;
		midiassign_t midiassignpara;
	};
};

int findsubparam(const char* name)
{
	if (!strcasecmp(name, "value")) return 0;
	if (!strcasecmp(name, "lfo_speed")) return 1;
	if (!strcasecmp(name, "lfo_depth")) return 2;
	if (!strcasecmp(name, "lfo_shape")) return 3;
	if (!strcasecmp(name, "adsr_a")) return 4;
	if (!strcasecmp(name, "adsr_d")) return 5;
	if (!strcasecmp(name, "adsr_s")) return 6;
	if (!strcasecmp(name, "adsr_r")) return 7;
	if (!strcasecmp(name, "adsr_depth")) return 8;
	if (!strcasecmp(name, "ad_a")) return 9;
	if (!strcasecmp(name, "ad_d")) return 10;
	if (!strcasecmp(name, "ad_depth")) return 11;
	if (!strcasecmp(name, "x")) return 12;
	if (!strcasecmp(name, "y")) return 13;
	if (!strcasecmp(name, "z")) return 14;
	if (!strcasecmp(name, "zprime")) return 15;
	if (!strcasecmp(name, "note")) return 16;
	if (!strcasecmp(name, "vel")) return 17;

	return -1;
}

int findparam(char* name)
{
	if (!name) return -1;

	char* p = name;
	while (*p) {
		*p = toupper(*p);
		p++;
	}
	
	if (parammap.find(name) == parammap.end()) {
		return -1;
	}

	return parammap[name];
}

int parseparamvalue(const char* value)
{
	if (!value) return -2000000000;

	int v;
	bool done = false;
	bool invert = false;

	if (value[0] == '-') {
		invert = true;
		value++;
	}

	if (value[0] == '0') {
		if (value[1] == 'x' || value[2] == 'X') {
			v = boost::lexical_cast<uint32_from_hex>(value);
			done = true;
		}
	}

	if (!done) {
		v = boost::lexical_cast<int>(value);
	}

	if (invert) v = -v;

	if (v < -32768 || v > 65535)
	{
		return -2000000000;
	}

	return v;
}

int parsemidichannel(const char* value)
{
	if (!value) return -1;

	int v;

	try {
		v = boost::lexical_cast<int>(value);
	} catch (exception& e) {
		return -1;
	}

	if (v < 1 || v > 16)
	{
		return -1;
	}

	return v - 1;
}

int parsemidicc(const char* value)
{
	if (!value) return -1;

	int v;
	try {
		v = boost::lexical_cast<int>(value);
	} catch (exception& e) {
		return -1;
	}

	if (v < 0 || v > 127)
	{
		return -1;
	}

	return v;
}

int findswitch(char* name)
{
	if (!name) return -1;

	char* p = name;
	while (*p) {
		*p = toupper(*p);
		p++;
	}

	if (switchmap.find(name) == switchmap.end()) {
		printf("No such switch \n");
		return -1;
	}

	return switchmap[name];
}

int findnotemode(const char* value)
{
	if (!value) return -1;

	if (!strcasecmp(value, "low")) {
		return 1;
	}
	if (!strcasecmp(value, "high")) {
		return 2;
	}
	if (!strcasecmp(value, "last")) {
		return 3;
	}

	return -1;
}

int findnote(const char* value)
{
	if (!value) return -1;

	int v;
	try {
		v = boost::lexical_cast<int>(value);
	} catch (exception& e) {
		return -1;
	}
	if (v < 0 || v > 127)
	{
		return -1;
	}

	return v;
}

int findvelocity(const char* value)
{
	if (!value) return -1;

	int v;
	try {
		v = boost::lexical_cast<int>(value);
	} catch (exception& e) {
		return -1;
	}
	if (v < 0 || v > 127)
	{
		return -1;
	}

	return v;
}

void set(setpara_t& para)
{
	char b[4];
	//b[0] = 1;
	b[0] = (para.paramid >> 0) & 0xFF;
	b[1] = (para.paramid >> 8) & 0xFF;
	b[2] = (para.value >> 8) & 0xFF;
	b[3] = (para.value >> 0) & 0xFF;

	out(b, 4);
}

void note_on(int noteid, int notevel)
{
	setpara_t setpara;
	setpara.paramid = 0x02fc;
	setpara.value = noteid | (notevel << 8);
	set(setpara);
}

void note_off(int noteid, int notevel)
{
	setpara_t setpara;
	setpara.paramid = 0x01fc;
	setpara.value = noteid | (notevel << 8);
	set(setpara);
}

int parse(char* cmd, input_t& input)
{
	char* token = strtok(cmd, " ");
	if (!token) return 1;

	if (!strcasecmp(token, "q"))
	{
		input.cmd = QUIT;
		return 0;
	}
	
	if (!strcasecmp(token, "file"))
	{
		input.cmd = SKIP;

		token = strtok(NULL, " ");
		if (!token) return 1;

		parse_file(token);
		return 0;
	}
	
	if (!strcasecmp(token, "autotune"))
	{
		input.cmd = SET;
		input.setpara.paramid = 0xfefe;
		input.setpara.value = 0;
		return 0;
	}
	
	if (!strcasecmp(token, "p"))
	{
		input.cmd = SET;
		
		token = strtok(NULL, " ");
		int subparamid = findsubparam(token);
		if (subparamid >= 0) {
			token = strtok(NULL, " ");
		} else {
			subparamid = 0;
		}

		int paramid = findparam(token);
		if (paramid < 0) {
			printf("No such parameter\n");
			return 1;
		}
		
		token = strtok(NULL, " ");
		int paramvalue = parseparamvalue(token);
		if (paramvalue < -100000) return 1;

		input.setpara.paramid = paramid | (subparamid << 8);
		input.setpara.value = paramvalue;
		
		return 0;
	}
	
	if (!strcasecmp(token, "s"))
	{
		input.cmd = SET;

		token = strtok(NULL, " ");
		int switchid = findswitch(token);
		if (switchid < 0) return 1;
		
		input.setpara.paramid = 0xfdfe;
		input.setpara.value = switchid | 0x200;

		midi_setswitch(switchid);
		
		return 0;
	}

	if (!strcasecmp(token, "c"))
	{
		input.cmd = SET;

		token = strtok(NULL, " ");
		int switchid = findswitch(token);
		if (switchid < 0) return 1;
		
		input.setpara.paramid = 0xfdfe;
		input.setpara.value = switchid | 0x100;
		
		midi_clearswitch(switchid);
		
		return 0;
	}

	if (!strcasecmp(token, "notemode"))
	{
		input.cmd = SET;

		token = strtok(NULL, " ");
		int notemode = findnotemode(token);
		if (notemode < 0) return 1;
		
		input.setpara.paramid = 0xfcfe;
		input.setpara.value = notemode;
		
		return 0;
	}

	if (!strcasecmp(token, "non"))
	{
		input.cmd = SKIP;

		token = strtok(NULL, " ");
		int noteid = findnote(token);
		if (noteid < 0) return 1;
		
		token = strtok(NULL, " ");
		int notevel = findvelocity(token);
		if (notevel < 0) notevel = 127;

		note_on(noteid, notevel);
		
		return 0;
	}

	if (!strcasecmp(token, "noff"))
	{
		input.cmd = SKIP;

		token = strtok(NULL, " ");
		int noteid = findnote(token);
		if (noteid < 0) return 1;
		
		token = strtok(NULL, " ");
		int notevel = findvelocity(token);
		if (notevel < 0) notevel = 127;
		
		note_off(noteid, notevel);
		
		return 0;
	}
	
	if (!strcasecmp(token, "midi"))
	{
		token = strtok(NULL, " ");
		if (!token) return 1;

		if (!strcasecmp(token, "learn"))
		{
			input.cmd = MIDI_LEARN;
			
			token = strtok(NULL, " ");
			if (!token || !strcasecmp(token, "on")) {
				input.midilearnpara.enable = 1;
				return 0;
			}
			else if (!strcasecmp(token, "off")) {
				input.midilearnpara.enable = 0;
				return 0;
			}
			return 1;
		}
		else if (!strcasecmp(token, "cc")
			 || !strcasecmp(token, "sw"))
		{
			int stateful = 0;
			if (!strcasecmp(token, "sw")) {
				stateful = 1;
			}

			input.cmd = MIDI_ASSIGN;

			token = strtok(NULL, " ");
			if (!token) return 1;

			uint8_t midichannel = 254;
			uint8_t midicc = 254;
			if (!strcasecmp(token, ":")) {
			}
			else {
				midichannel = parsemidichannel(token);
				if (midichannel < 0) {
					return 1;
				}

				token = strtok(NULL, " ");
				midicc = parsemidicc(token);
				if (midicc < 0) {
					return 1;
				}
			}

			token = strtok(NULL, " ");
			int subparamid = findsubparam(token);
			if (subparamid >= 0) {
				token = strtok(NULL, " ");
			} else {
				subparamid = 0;
			}

			int paramid = findparam(token);
			if (paramid < 0) {
				// maybe it's a switch?

				paramid = findswitch(token);
				if (paramid < 0) {
					printf("No such parameter or switch\n");
					return 1;
				}
				input.midiassignpara.channel = midichannel;
				input.midiassignpara.cc = midicc;

				if (stateful) {
					input.midiassignpara.paramid =
						(0xfe << 8) | paramid;
				}
				else {
					input.midiassignpara.paramid =
						(0xff << 8) | paramid;
				}
			}
			else {
				input.midiassignpara.channel = midichannel;
				input.midiassignpara.cc = midicc;
				input.midiassignpara.paramid =
					paramid | (subparamid << 8);

				input.midiassignpara.min = 0;
				input.midiassignpara.max = 0xffff;

				token = strtok(NULL, " ");
				int minvalue = parseparamvalue(token);
				if (minvalue >= 0) {
					input.midiassignpara.min = minvalue;
					token = strtok(NULL, " ");
					int maxvalue = parseparamvalue(token);
					if (maxvalue >= 0) {
						input.midiassignpara.max = maxvalue;
					}
				}
			}

			return 0;
		}
	}

	return 1;
}

void cmdloop()
{
	bool quit = false;
	while (!quit)
	{
		char cmdbuf[512];
		fgets(cmdbuf, sizeof(cmdbuf), stdin);
		cmdbuf[sizeof(cmdbuf)-1] = 0;

		*strchrnul(cmdbuf, '\n') = 0;

		// stop midi learn
		midi_learn(0);

		if (strlen(cmdbuf) == 0) {
			continue;
		}

		input_t input;
		if (parse(cmdbuf, input))
		{
			printf("Parse error\n");
			continue;
		}

		switch (input.cmd)
		{
		case QUIT:
			quit = true;
			break;
		case SET:
			set(input.setpara);
			break;
		case SKIP:
			break;
		case MIDI_LEARN:
			midi_learn(input.midilearnpara.enable);
			break;
		case MIDI_ASSIGN:
			midi_assign(input.midiassignpara.channel,
					input.midiassignpara.cc,
					input.midiassignpara.paramid,
					input.midiassignpara.min,
					input.midiassignpara.max);
			break;
		}
	}
}

void parse_file(char* filename)
{
	FILE *f = fopen(filename, "rb");
	if (!f) {
		printf("Failed to open file '%s'\n", filename);
		return;
	}

	int line = 0;
	while(!feof(f)) {
		usleep(1000);
		char cmdbuf[512];
		cmdbuf[0] = 0;
		fgets(cmdbuf, sizeof(cmdbuf), f);
		cmdbuf[sizeof(cmdbuf)-1] = 0;

		line++;

		*strchrnul(cmdbuf, '\n') = 0;

		if (strlen(cmdbuf) == 0) {
			continue;
		}

		input_t input;
		if (parse(cmdbuf, input))
		{
			printf("Parse error on line %d\n", line);
			continue;
		}

		switch (input.cmd)
		{
		case SET:
			set(input.setpara);
			break;
		case SKIP:
			break;
		case MIDI_ASSIGN:
			midi_assign(input.midiassignpara.channel,
					input.midiassignpara.cc,
					input.midiassignpara.paramid,
					input.midiassignpara.min,
					input.midiassignpara.max);
			break;
		}
	}

	fclose(f);
}

int main(int argc, char *argv[])
{
	synth_init();
	out_init();
	player_init();

	if (argc > 1) {
		parse_file(argv[1]);
	}
	
	cmdloop();

	player_deinit();
	return 0;
}

