#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "fsl_device_registers.h"

#include "uart.h"
#include "FreeRTOS.h"
#include "task.h"

//#define ENTER_CRITICAL_SECTION() int __last_int_status = __get_PRIMASK(); __disable_irq();
//#define LEAVE_CRITICAL_SECTION() if (__last_int_status) { __enable_irq(); }

#define ENTER_CRITICAL_SECTION() taskENTER_CRITICAL()
#define LEAVE_CRITICAL_SECTION() taskEXIT_CRITICAL()

//////////////////////////////////////////////////

// https://users.ece.cmu.edu/~koopman/pubs/KoopmanCRCWebinar9May2012.pdf
// https://barrgroup.com/Embedded-Systems/How-To/CRC-Calculation-C-Code

#define POLYNOMIAL 0x97
#define WIDTH 8
#define TOPBIT 0x80

uint8_t crcTable[256];

static void crc_init(void)
{
    uint8_t remainder;

    /*
     * Compute the remainder of each possible dividend.
     */
    for (int dividend = 0; dividend < 256; ++dividend)
    {
        /*
         * Start with the dividend followed by zeros.
         */
        remainder = dividend << (WIDTH - 8);

        /*
         * Perform modulo-2 division, a bit at a time.
         */
        for (uint8_t bit = 8; bit > 0; --bit)
        {
            /*
             * Try to divide the current data bit.
             */     
            if (remainder & TOPBIT)
            {
                remainder = (remainder << 1) ^ POLYNOMIAL;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }

        /*
         * Store the result into the table.
         */
        crcTable[dividend] = remainder;
    }

}   /* crcInit() */

uint8_t crc(uint8_t firstbyte, uint8_t const* message, int nBytes)
{
    uint8_t data;
    uint8_t remainder = 0;

    data = firstbyte ^ (remainder >> (WIDTH - 8));
    remainder = crcTable[data] ^ (remainder << 8);

    /*
     * Divide the message by the polynomial, a byte at a time.
     */
    for (int byte = 0; byte < nBytes; ++byte)
    {
        data = message[byte] ^ (remainder >> (WIDTH - 8));
        remainder = crcTable[data] ^ (remainder << 8);
    }

    /*
     * The final remainder is the CRC.
     */
    return (remainder);

}   /* crcFast() */

//////////////////////////////////////////////////

void sync_out_write_ack(sync_state_t* state, uint8_t checksum);
void sync_out_ack_received(sync_state_t* state, uint8_t checksum);

void sync_reset(sync_state_t* state);
void sync_resync(sync_state_t* state);

//////////////////////////////////////////////////

void sync_in_read_complete(int status, void* stateptr)
{
	uint8_t sync_read_buf[16];

    ENTER_CRITICAL_SECTION();

	sync_state_t* state = (sync_state_t*)stateptr;
	state->timer = 0;

	if (state->in_reset) {
		uint8_t data = state->uart->rx.peek();
		if (data == 0xFF) {
			state->uart->rx.read(sync_read_buf, 1);
			state->uart->rx.waitfor(1);
		    LEAVE_CRITICAL_SECTION();
			return;
		}
		state->in_running_checksum = 0;
		state->in_reset = 0;
		state->uart->rx.waitfor(6);
	    LEAVE_CRITICAL_SECTION();
		return;
	}

	if (status != 0) {
		sync_reset(state);
		state->uart->rx.waitfor(6);
	    LEAVE_CRITICAL_SECTION();
		return;
	}

	uint8_t* data = &sync_read_buf[3];
	state->uart->rx.read(data, 6);
	if (data[0] == 0xFF
			&& data[1] == 0xFF
			&& data[2] == 0xFF
			&& data[3] == 0xFF
			&& data[4] == 0xFF
			&& data[5] == 0xFF)
	{
		state->in_reset = 1;
		state->ack_send_pending = 0;
		state->uart->rx.waitfor(1);
		//sync_reset(state);
	    LEAVE_CRITICAL_SECTION();
		return;
	}

	state->in_running_checksum = crc(state->in_running_checksum, data, 5);
	if (state->in_running_checksum != data[5]) {
		sync_reset(state);
		state->uart->rx.waitfor(6);
	    LEAVE_CRITICAL_SECTION();
		return;
	}

	int cmd = data[0];

	switch (cmd) {
	case 1: // SET WRITE ADDRESS
		// set write address
		memcpy((void*)&state->in_dst, &data[1], 4);
		sync_out_write_ack(state, state->in_running_checksum);
		break;
	case 2: { // WRITE
			int in_dst = state->in_dst;
			state->in_dst += 4;
			state->data(in_dst, &data[1]);
			sync_out_write_ack(state, state->in_running_checksum);
		}
		break;
	case 3: // ACK
		// trigger next send
		sync_out_ack_received(state, data[1]);
		break;
	case 4: // RESYNC
		sync_resync(state);
		sync_out_write_ack(state, state->in_running_checksum);
		break;
	default:
		if (state->oob_data) {
			uint32_t d =
				data[1] | (data[2] << 8) | (data[3] << 16) | (data[4] << 24);
			if (state->oob_data(cmd, d) == 0) {
				sync_out_write_ack(state, state->in_running_checksum);
				break;
			}
		}
		printf("unexpected %d\n", cmd);
		sync_reset(state);
		state->uart->rx.waitfor(6);
	    LEAVE_CRITICAL_SECTION();
		return;
	}

	state->uart->rx.waitfor(6);
    LEAVE_CRITICAL_SECTION();
}

//sync_state_t* sync_state;

void sync_out_write_begin(sync_state_t* state, int cmd, uint8_t* data, int expectack)
{
  uint8_t sync_write_buf[8];

	state->timer = 0;

  sync_write_buf[0] = cmd;
  memcpy(&sync_write_buf[1], data, 4);
  state->out_running_checksum = crc(state->out_running_checksum, sync_write_buf, 5);
  sync_write_buf[5] = state->out_running_checksum;

  if (expectack) {
    state->ack_receive_pending = 1;
    state->ack_receive_value = state->out_running_checksum;
  }

  int size = 6;
  int write_count = state->uart->tx.write(sync_write_buf, size);
  if (write_count < size) {
    sync_reset(state);
  }
}

void sync_out_write_complete(int status, void* stateptr)
{
  sync_state_t* state = (sync_state_t*)stateptr;

	ENTER_CRITICAL_SECTION();

  if (status != 0) {
    state->transmit_active = 0;
    state->complete(1);
    LEAVE_CRITICAL_SECTION();
    return;
  }

  if (state->out_reset > 0) {
    if (state->out_reset >= 12) {
      state->out_reset = 0;
      state->ack_receive_pending = 0;
      state->transmit_active = 0;
      state->out_running_checksum = 0;
      state->oob_queue_read = 0;
      state->oob_queue_write = 0;
      state->ack_receive = 0;
      state->phase = complete;

      if (!state->out_resync) {
		uint8_t data[4];
		data[0] = 0;
		data[1] = 0;
		data[2] = 0;
		data[3] = 0;
		sync_out_write_begin(state, 4, data, 1);
      }
      state->out_resync = 0;

      if (state->complete) {
    	  state->complete(1);
      }
      LEAVE_CRITICAL_SECTION();
      return;
    }
    else {
      uint8_t data[4];
      data[0] = 0xFF;
      data[1] = 0xFF;
      data[2] = 0xFF;
      data[3] = 0xFF;
      state->out_reset += 4;
      state->uart->tx.write(data, 4);
      LEAVE_CRITICAL_SECTION();
      return;
    }
  }

  if (state->ack_send_pending) {
    uint8_t data[4];
    data[0] = state->ack_send_value;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    state->ack_send_pending = 0;
    sync_out_write_begin(state, 3, data, 0);
    LEAVE_CRITICAL_SECTION();
    return;
  }

  if (state->ack_receive_pending) {
    state->transmit_active = 0;
    LEAVE_CRITICAL_SECTION();
    return;
  }

  if (state->oob_queue_write != state->oob_queue_read) {
    state->ack_receive = state->oob_queue[state->oob_queue_read].complete;
    uint8_t cmd = state->oob_queue[state->oob_queue_read].cmd;
    uint32_t data = state->oob_queue[state->oob_queue_read].data;
    state->oob_queue_read = (state->oob_queue_read + 1) & 15;
    sync_out_write_begin(state, cmd, (uint8_t*)&data, 1);
    LEAVE_CRITICAL_SECTION();
    return;
  }

  if (state->phase == init) {
    state->phase = data;
    state->transmit_active = 1;
    sync_out_write_begin(state, 1, (uint8_t*)&state->out_dst, 1);
    LEAVE_CRITICAL_SECTION();
    return;
  }

  if (state->phase == complete) {
	  state->transmit_active = 0;
	    LEAVE_CRITICAL_SECTION();
    return;
  }

  if (state->size == 0) {
    state->transmit_active = 0;
    state->phase = complete;
    state->complete(0);
    LEAVE_CRITICAL_SECTION();
    return;
  }

  // else if (sync_state->phase == data) {
  
  uint8_t* volatile data = state->ptr;

  if (state->size > 4) {
    state->ptr += 4;
    state->out_dst += 4;
    state->size -= 4;
  }
  else {
    state->size = 0;
  }

  state->transmit_active = 1;
  sync_out_write_begin(state, 2, data, 1);

  LEAVE_CRITICAL_SECTION();
}

void sync_reset(sync_state_t* state)
{
	if (state->out_reset != 0) {
		return;
	}

	//printf("sync_reset()\n");
	state->out_reset = 1;
	state->out_resync = 0;
	if (!state->transmit_active) {
		sync_out_write_complete(0, state);
	}
}

void sync_resync(sync_state_t* state)
{
	if (state->out_reset != 0) {
		return;
	}

	//printf("sync_resync()\n");
	state->out_reset = 1;
	state->out_resync = 1;
	if (!state->transmit_active) {
		sync_out_write_complete(0, state);
	}
}

void sync_out_write_ack(sync_state_t* state, uint8_t checksum)
{
	state->ack_send_value = checksum;
	state->ack_send_pending = 1;

	//if (!state->transmit_active) {
		sync_out_write_complete(0, state);
	//}
}

void sync_out_ack_received(sync_state_t* state, uint8_t checksum)
{
	if (!state->ack_receive_pending) {
		sync_reset(state);
		return;
	}

	if (state->ack_receive_value != checksum) {
		sync_reset(state);
		return;
	}

	if (state->ack_receive) {
		state->ack_receive();
		state->ack_receive = 0;
	}
	state->ack_receive_pending = 0;
	// restart transmitting
	sync_out_write_complete(0, state);
}

void sync_timer_tick(void* stateptr)
{
	ENTER_CRITICAL_SECTION();
	//printf("tick\n");
	sync_state_t* state = (sync_state_t*)stateptr;
	state->timer++;
	if (state->timer == 3) {
		state->timer = 0;
		sync_reset(state);
	}
	LEAVE_CRITICAL_SECTION();
}

void sync_block(sync_state_t* state, uint8_t* block, int offset, int size, sync_complete_cb complete_cb)
{
	ENTER_CRITICAL_SECTION();
	if (state->phase != complete) {
		// not ready
		LEAVE_CRITICAL_SECTION();
		return;
	}

	state->phase = init;
	state->ptr = block + offset;
	state->out_dst = offset;
	state->size = size;
	state->complete = complete_cb;
	state->transmit_active = 1;
	sync_out_write_complete(0, state);
	LEAVE_CRITICAL_SECTION();
}

int sync_oob_word(sync_state_t* state, uint8_t cmd, uint32_t data, oob_transfer_complete_cb complete_cb)
{
	ENTER_CRITICAL_SECTION();
	if (((state->oob_queue_write + 1) & 15) == state->oob_queue_read) {
		LEAVE_CRITICAL_SECTION();
		return 1;
	}

	state->oob_queue[state->oob_queue_write].cmd = cmd;
	state->oob_queue[state->oob_queue_write].data = data;
	state->oob_queue[state->oob_queue_write].complete = complete_cb;

	state->oob_queue_write = (state->oob_queue_write + 1) & 15;

	sync_out_write_complete(0, state);
	LEAVE_CRITICAL_SECTION();

	return 0;
}

//////////////////////////////////////////////////

#if 0
sync_state_t sync_state;

void oob_complete()
{
  printf("oob complete\n");
}

void datafunc(int addr, uint8_t* data)
{
  printf("data(%d, %x %x %x %x)\n", addr, data[0], data[1], data[2], data[3]);

  if (random() % 4095 < 30) {
    printf("oob out\n");
    sync_oob_word(&sync_state, 0x50, 0x33445566, oob_complete);
  }
}

int oobfunc(uint8_t cmd, uint32_t data)
{
  printf("oob data(cmd=%d, data=%x)\n", cmd, data);
  return 0;
}

uint8_t datablock[32];

int done = 0;
void completed(int status)
{
  printf("sync_complete_cb(%d)\n", status);

  if (status == 1) {
    sync_block(&sync_state, datablock, 0, 32, completed);
  }
  else if (status == 0) {
    done = 1;
  }
}
#endif

void uart_configure(uart_config_t* config,
    uart_write_complete_cb write_complete,
    void* write_complete_data,
    uart_read_complete_cb read_complete,
    void* read_complete_data,
    uart_timer_tick_cb timer_tick,
    void* timer_tick_data)
{
  config->write_complete = write_complete;
  config->write_complete_data = write_complete_data;
  config->read_complete = read_complete;
  config->read_complete_data = read_complete_data;
  config->timer_tick = timer_tick;
  config->timer_tick_data = timer_tick_data;
}

typedef void (*sync_data_cb)(int addr, uint8_t* data);
typedef int (*oob_data_cb)(uint8_t cmd, uint32_t data);

void sync_init(sync_state_t* state, uart_t* uart, sync_data_cb data, oob_data_cb oobdata)
{
	crc_init();

	state->uart = uart;
	state->out_reset = 1;
	state->phase = complete;

	state->timer = 0;
	state->data = data;
	state->oob_data = oobdata;
	state->oob_queue_write = 0;
	state->oob_queue_read = 0;
	state->uart->rx.waitfor(6);

	uart_configure(&uart->config,
		sync_out_write_complete,
		state,
		sync_in_read_complete,
		state,
		sync_timer_tick,
		state);
}

#if 0
void test()
{
  sync_state.out_reset = 1;

  uart_configure(&uart0,
    sync_out_write_complete,
    &sync_state,
    sync_in_read_complete,
    &sync_state,
    &sync_timer_tick,
    &sync_state);

  sync_state.timer = 0;
  sync_state.data = datafunc;
  sync_state.oob_data = oobfunc;
  sync_state.oob_queue_write = 0;
  sync_state.oob_queue_read = 0;
  state->uart->waitfor(6);

  for (int i = 0; i < 32; i++) datablock[i] = i + 5;

  sync_block(&sync_state, datablock, 0, 32, completed);

  while (!done) {
    uart_run(&uart0);
  }
}

int main(void)
{
  crc_init();

  //while(1) {
  for (int i = 0; i < 5000; i++) {
    uart_byte_counter = 0;
    uart_dropbyte = random() % 255;
    uart_errorbit = random() % 2047;
    databuf_read = 0;
    databuf_write = 0;
    done = 0;
    test();
    printf("--------\n");
  }

  return 0;
}

#endif

