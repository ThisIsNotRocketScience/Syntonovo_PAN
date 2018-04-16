
#include <sys/poll.h>
#include <pthread.h>
#include <alsa/asoundlib.h>
#include <map>
//#include <multimap>

#include "midi.h"
#include "ui.h"

void note_on(int noteide, int notevel);
void note_off(int noteid, int notevel);

#define TICKS_PER_QUARTER (96)
#define SEQ_MAX_LEN 16384

struct Song *song;

static int queue_id, port_in_id, port_out_id;
static snd_seq_t *seq_handle;
//static snd_seq_tick_time_t tick;
static int do_exit = 0;

static bool midi_learn_enable = false;
static uint8_t midi_learn_channel = 255;
static uint8_t midi_learn_cc = 255;

static std::map<uint16_t, uint16_t> midi_assigned_ccs;
static std::map<uint16_t, uint16_t> midi_assigned_ccs_min;
static std::map<uint16_t, uint16_t> midi_assigned_ccs_max;
static std::multimap<uint16_t, uint16_t> ctrl_assigned_ccs;

static uint16_t midi_cc_to_controller(uint8_t channel, uint8_t cc)
{
	uint16_t ccid = (channel << 8) | cc;

	auto ctrl = midi_assigned_ccs.find(ccid);
	if (ctrl == midi_assigned_ccs.end()) {
		return 65535;
	}

	return ctrl->second;
}

static int find_port(snd_seq_addr_t* addr, const char* name, int inout, int index)
{
	snd_seq_client_info_t *cinfo;
	snd_seq_port_info_t *pinfo;
	snd_seq_client_info_alloca(&cinfo);
	snd_seq_port_info_alloca(&pinfo);
	snd_seq_client_info_set_client(cinfo, -1);

	unsigned int caps;
	int search_index = 0;

	while (snd_seq_query_next_client(seq_handle, cinfo) == 0) {

		snd_seq_port_info_set_client(pinfo, snd_seq_client_info_get_client(cinfo));
		snd_seq_port_info_set_port(pinfo, -1);
		while (snd_seq_query_next_port(seq_handle, pinfo) == 0) {
			if (strcmp(name, snd_seq_port_info_get_name(pinfo))) {
				continue; // name doesn't match
			}


			if (snd_seq_port_info_get_client(pinfo) == SND_SEQ_CLIENT_SYSTEM)
				continue; // ignore timer, announce ports
			caps = snd_seq_port_info_get_capability(pinfo);
			if (!(caps & SND_SEQ_PORT_CAP_SUBS_READ))
				continue; // cannot read or write port

			if ((inout == 0 && (caps & SND_SEQ_PORT_CAP_READ))
			 || (inout == 1 && (caps & SND_SEQ_PORT_CAP_WRITE))) {

				if (search_index == index) {
					addr->client = snd_seq_port_info_get_client(pinfo);
					addr->port = snd_seq_port_info_get_port(pinfo);
					return 0;
				}
				search_index++;
			}
		}
	}

	return -1;
}

static int connect_port(snd_seq_addr_t *sender, snd_seq_addr_t *dest)
{
	snd_seq_port_subscribe_t *subs;
	snd_seq_port_subscribe_alloca(&subs);
	snd_seq_port_subscribe_set_sender(subs, sender);
        snd_seq_port_subscribe_set_dest(subs, dest);
	return snd_seq_subscribe_port(seq_handle, subs);
}

static void open_seq()
{
	if (snd_seq_open(&seq_handle, "default", SND_SEQ_OPEN_DUPLEX, 0) < 0) {
		fprintf(stderr, "Error opening ALSA sequencer.\n");
		exit(1);
	}
	snd_seq_set_client_name(seq_handle, "miditracker");
	if ((port_out_id = snd_seq_create_simple_port(seq_handle, "output",
			SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ,
			SND_SEQ_PORT_TYPE_APPLICATION)) < 0) {
		fprintf(stderr, "Error creating sequencer port.\n");
	}
	if ((port_in_id = snd_seq_create_simple_port(seq_handle, "input",
			SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE,
			SND_SEQ_PORT_TYPE_APPLICATION)) < 0) {
		fprintf(stderr, "Error creating sequencer port.\n");
		exit(1);
	}

	snd_seq_addr_t midi_device;
	snd_seq_addr_t port_in;
	/*if (find_port(&midi_device, "MK-249C USB MIDI keyboard", 1, 0) < 0) {
		fprintf(stderr, "Could not find input port.\n");
		return;
	}

	port_in.client = snd_seq_client_id(seq_handle);
	port_in.port = port_in_id;
	if (connect_port(&midi_device, &port_in) < 0) {
		fprintf(stderr, "Could not connect input port.\n");
		return;
	}*/
	
	if (find_port(&midi_device, "usbmidi2 4x4 master MIDI 2", 0, 0) < 0) {
	//if (find_port(&midi_device, "nanoKONTROL2 MIDI 1", 1, 0) < 0) {
		fprintf(stderr, "Could not find input port.\n");
		return;
	}
	port_in.client = snd_seq_client_id(seq_handle);
	port_in.port = port_in_id;
	if (connect_port(&midi_device, &port_in) < 0) {
		fprintf(stderr, "Could not connect input port.\n");
		return;
	}
	
	if (find_port(&midi_device, "nanoKONTROL2 MIDI 1", 0, 0) < 0) {
		fprintf(stderr, "Could not find input port.\n");
		return;
	}
	port_in.client = snd_seq_client_id(seq_handle);
	port_in.port = port_in_id;
	if (connect_port(&midi_device, &port_in) < 0) {
		fprintf(stderr, "Could not connect input port.\n");
		return;
	}
	
	if (find_port(&midi_device, "nanoKONTROL2 MIDI 1", 1, 0) < 0) {
		fprintf(stderr, "Could not find output port.\n");
		return;
	}
	snd_seq_addr_t port_out;
	port_out.client = snd_seq_client_id(seq_handle);
	port_out.port = port_out_id;
	if (connect_port(&port_out, &midi_device) < 0) {
		fprintf(stderr, "Could not connect output port.\n");
		return;
	}
}

static void init_queue()
{
	queue_id = snd_seq_alloc_queue(seq_handle);
	snd_seq_set_client_pool_output(seq_handle, (SEQ_MAX_LEN<<1) + 4);
}

static void clear_queue()
{
	snd_seq_remove_events_t *remove_ev;

	snd_seq_remove_events_malloc(&remove_ev);
	snd_seq_remove_events_set_queue(remove_ev, queue_id);
	snd_seq_remove_events_set_condition(remove_ev,
		SND_SEQ_REMOVE_OUTPUT | SND_SEQ_REMOVE_IGNORE_OFF);
	snd_seq_remove_events(seq_handle, remove_ev);
	snd_seq_remove_events_free(remove_ev);
}

static void input_note(snd_seq_ev_note_t& note, bool noteon)
{
	if (noteon && note.velocity != 0) {
		note_on(note.note, note.velocity);
	}
	else {
		note_off(note.note, note.velocity);
	}
}

static std::map<uint16_t, bool> stateful_values;
static int stateful_switch(int channel, int param, int value)
{
	uint16_t ccid = (channel << 8) | param;

	bool val = stateful_values[ccid];
	if (value > 63) {
		val = !val;
		stateful_values[ccid] = val;
	}
	return val ? 127 : 0;
}

static void input_cc(snd_seq_ev_ctrl_t& control)
{
	if (midi_learn_enable) {
		printf("channel %d cc %d value %d\n",
				control.channel + 1,
				control.param,
				control.value);
		midi_learn_channel = control.channel;
		midi_learn_cc = control.param;
	}
	//lan_output_cc(control.channel, control.param, control.value);

	setpara_t setpara;
	uint16_t ccid = (control.channel << 8) | control.param;
	setpara.paramid = midi_cc_to_controller(control.channel, control.param);
	if (setpara.paramid == 65535) {
		//return;
	}

	if ((setpara.paramid >> 8) == 0xff
	    || (setpara.paramid >> 8) == 0xfe)
	{
		// switch
		bool stateful = (setpara.paramid >> 8) == 0xfe;
		int switchid = setpara.paramid & 0xff;

		if (stateful) {
			control.value = stateful_switch(control.channel,
				       control.param,
				       control.value);
		}

		if (control.value > 63) {
			setpara.paramid = 0xfdfe;
			setpara.value = switchid | 0x200;
			set(setpara);
			midi_setswitch(switchid & 0xff);
		}
		else {
			setpara.paramid = 0xfdfe;
			setpara.value = switchid | 0x100;
			set(setpara);
			midi_clearswitch(switchid & 0xff);
		}
	}
	else {
		// normal controller
		int32_t min = midi_assigned_ccs_min[ccid];
		int32_t max = midi_assigned_ccs_max[ccid];
		int32_t range = max - min;
		setpara.value = min + ((int)control.value * range) / 127;
		set(setpara);
	}
}

static void input_pgmchange(snd_seq_ev_ctrl_t& control)
{
	//lan_output_pgmchange(control.channel, control.value);
}

int queued = 0;

static void midi_action(void)
{
	snd_seq_event_t *ev;

	do {
		snd_seq_event_input(seq_handle, &ev);

		switch(ev->type) {
		case SND_SEQ_EVENT_NOTEON:
		case SND_SEQ_EVENT_NOTEOFF:
			input_note(ev->data.note, ev->type == SND_SEQ_EVENT_NOTEON);
			break;
		case SND_SEQ_EVENT_CONTROLLER:
			input_cc(ev->data.control);
			break;
		case SND_SEQ_EVENT_ECHO:
			//printf("echo\n");
			//schedule_playback();
			break;
		case SND_SEQ_EVENT_SENSING:
		case SND_SEQ_EVENT_PORT_CHANGE:
			break;
		case SND_SEQ_EVENT_PGMCHANGE:
			input_pgmchange(ev->data.control);
			break;
		default:
			//printf("%d\n", ev->type);
			break;
		}
		
		snd_seq_free_event(ev);
	} while(snd_seq_event_input_pending(seq_handle, 0) > 0);
	queued = 1;
}

static void* player_thread(void *arg)
{
	int npfd;
	struct pollfd *pfd;
	int i;

	snd_seq_start_queue(seq_handle, queue_id, NULL);
	snd_seq_drain_output(seq_handle);

	npfd = snd_seq_poll_descriptors_count(seq_handle, POLLIN);
	pfd = (struct pollfd *)alloca(npfd * sizeof(struct pollfd));
	snd_seq_poll_descriptors(seq_handle, pfd, npfd, POLLIN);

	int first_in_queue = 1;
	while(!do_exit) {
		int timeout = 100000;
		if (queued) timeout = 1;
		if(poll(pfd, npfd, timeout) > 0) {
			for(i = 0; i < npfd; i++) {
				if(pfd[i].revents > 0) {
					midi_action(); 
					if (first_in_queue) {
						//lan_flush();
					}
					first_in_queue = 0;
				}
			}
		} else {
			if (queued) {
				//lan_flush();
				queued = 0;
				first_in_queue = 1;
			}
		}
	}

	return 0;
}

static void init_thread(void)
{
	pthread_t thread;
	pthread_create(&thread, NULL, player_thread, 0);
}

static void deinit_thread(void)
{
	do_exit = 1;
}

void player_init(void)
{
	//init_song();
	
	open_seq();
	init_queue();
	//set_tempo(132);
	init_thread();
}

void player_deinit(void)
{
	deinit_thread();
}

void player_play(void)
{
	//player_stop();
	//schedule_playback();

}

void player_stop(void)
{
	clear_queue();
}

void midi_learn(int enable)
{
	midi_learn_enable = !!enable;
}

void midi_unmap(uint16_t ccid, uint16_t paramid)
{
	auto iter = ctrl_assigned_ccs.find(paramid & 0xff);
	while (iter != ctrl_assigned_ccs.end() && iter->first == paramid & 0xff) {
		if (iter->second == ccid) {
			ctrl_assigned_ccs.erase(iter);
			return;
		}

		++iter;
	}
}

void midi_assign(uint8_t channel, uint8_t cc, uint16_t paramid, uint16_t min, uint16_t max)
{
	if (channel == 254 && cc == 254) {
		channel = midi_learn_channel;
		cc = midi_learn_cc;
	}
	uint16_t ccid = (channel << 8) | cc;
	midi_unmap(ccid, midi_assigned_ccs[ccid]);
	midi_assigned_ccs[ccid] = paramid;
	midi_assigned_ccs_min[ccid] = min;
	midi_assigned_ccs_max[ccid] = max;
	ctrl_assigned_ccs.insert(std::make_pair(paramid & 0xff, ccid));
}

void midi_cc_out(uint8_t channel, uint8_t cc, uint8_t value)
{
	snd_seq_event_t ev;
	snd_seq_ev_clear(&ev);
	snd_seq_ev_set_source(&ev, port_out_id);
	snd_seq_ev_set_subs(&ev);
	snd_seq_ev_set_direct(&ev);
	snd_seq_ev_set_controller(&ev, channel, cc, value);
	snd_seq_event_output_direct(seq_handle, &ev);
}

void midi_setswitch(int switchid)
{
	auto iter = ctrl_assigned_ccs.find(switchid & 0xff);

	while (iter != ctrl_assigned_ccs.end() && iter->first == switchid & 0xff) {
		int ccid = iter->second;
		midi_cc_out(ccid >> 8, ccid & 0xff, 127);

		++iter;
	}
}

void midi_clearswitch(int switchid)
{
	auto iter = ctrl_assigned_ccs.find(switchid & 0xff);

	while (iter != ctrl_assigned_ccs.end() && iter->first == switchid & 0xff) {
		int ccid = iter->second;
		midi_cc_out(ccid >> 8, ccid & 0xff, 0);

		++iter;
	}
}

