#include <stdio.h>
#include <vector>
#include "serial.hpp"
#include "serial.cpp"
#include "sync/uart.h"
#include "sync/uart.cpp"

Serial S;

uart_t uart;
sync_state_t rpi_sync;

static pthread_mutex_t serial_critical_section_mutex;

#define SERIAL_ENTER_CRITICAL_SECTION() pthread_mutex_lock(&serial_critical_section_mutex);
#define SERIAL_LEAVE_CRITICAL_SECTION() pthread_mutex_unlock(&serial_critical_section_mutex);

std::vector<uint8_t> in_queue;

int waitfor_level = 60;

static void checkqueue()
{
	int do_call = 0;
	SERIAL_ENTER_CRITICAL_SECTION();
	if (in_queue.size() >= waitfor_level) do_call = 1;
	SERIAL_LEAVE_CRITICAL_SECTION();
	if (do_call) {
		uart.config.read_complete(0, uart.config.read_complete_data);
	}
}

static uint8_t serial_peek()
{
	SERIAL_ENTER_CRITICAL_SECTION();
	uint8_t b = in_queue.front();
	SERIAL_LEAVE_CRITICAL_SECTION();
	return b;
}

static void serial_read(uint8_t* data, int count)
{
	SERIAL_ENTER_CRITICAL_SECTION();
	for (int i = 0; i < count; i++) {
		data[i] = in_queue.front();
		in_queue.erase(in_queue.begin());
	}
	SERIAL_LEAVE_CRITICAL_SECTION();
}

static int serial_write(uint8_t* data, int count)
{
	SERIAL_ENTER_CRITICAL_SECTION();
	//printf("w ");
	for (int i = 0; i < count; i++) {
		//printf("%x ", data[i]);
		S.Put(data[i]);
	}
	//printf("\n");
	SERIAL_LEAVE_CRITICAL_SECTION();

	uart.config.write_complete(0, uart.config.write_complete_data);

	return count;
}

static void serial_waitfor(int level)
{
	SERIAL_ENTER_CRITICAL_SECTION();
	waitfor_level = level;
	SERIAL_LEAVE_CRITICAL_SECTION();
	checkqueue();
}

pthread_t serial_thread;

void* SerialThread(void*)
{
	while(1) {
		uint8_t b = S.Blocking_get();
		SERIAL_ENTER_CRITICAL_SECTION();
		in_queue.push_back(b);
		SERIAL_LEAVE_CRITICAL_SECTION();
		checkqueue();
	}

	return 0;
}


void sync_data_func(int addr, uint8_t* data)
{
	//printf("sync_data(%x, %x %x %x %x)\n", addr, data[0], data[1], data[2], data[3]);
}

#define OOB_UI_PAUSE		(0x41)
#define OOB_UI_CONTINUE		(0x42)
#define OOB_BUTTON_DOWN		(0x50)
#define OOB_BUTTON_UP		(0x51)
#define OOB_ENCODER_CCW		(0x56)
#define OOB_ENCODER_CW		(0x57)
#define OOB_ENCODER_DOWN	(0x58)
#define OOB_ENCODER_UP		(0x59)
#define OOB_SWITCH_CHANGE	(0x5F)

int show_switchchanges = 0;

int sync_oobdata_func(uint8_t cmd, uint32_t data)
{
	switch (cmd) {
	case OOB_UI_PAUSE:
		printf("OOB_UI_PAUSE\n");
		break;
	case OOB_UI_CONTINUE:
		printf("OOB_UI_CONTINUE\n");
		break;
	case OOB_BUTTON_DOWN:
		printf("button down %d\n", data);
		break;
	case OOB_BUTTON_UP:
		printf("button up %d\n", data);
		break;
	case OOB_ENCODER_CCW:
		printf("encoder dec %d\n", data);
		break;
	case OOB_ENCODER_CW:
		printf("encoder inc %d\n", data);
		break;
	case OOB_ENCODER_DOWN:
		printf("encoder down %d\n", data);
		break;
	case OOB_ENCODER_UP:
		printf("encoder up %d\n", data);
		break;
	case OOB_SWITCH_CHANGE:
		if (show_switchchanges) {
			printf("switch change %d\n", data);
		}
		break;
	default:	
		printf("sync_oobdata(%d, %x)\n", cmd, data);
	}

	return 0;
}

void sync_complete(int status)
{
}

int main(int argc, char *argv[])
{
	if (argc > 1) {
		show_switchchanges = 1;
	}
	uart.rx.waitfor = serial_waitfor;
	uart.rx.peek = serial_peek;
	uart.rx.read = serial_read;
	uart.tx.write = serial_write;
	sync_init(&rpi_sync, &uart, sync_data_func, sync_oobdata_func);

	S.Connect("/dev/ttyAMA0", 115200);

	pthread_create(&serial_thread, NULL, SerialThread, 0);

	int tmp = 0;
	sync_block(&rpi_sync, (uint8_t*)&tmp, 1, 1, sync_complete);

        while (1)
        {
		usleep(500000);
		uart.config.timer_tick(uart.config.timer_tick_data);
	}

	return 0;
}

