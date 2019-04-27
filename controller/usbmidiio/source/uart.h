#ifndef UART_H_
#define UART_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*uart_write_complete_cb)(int status, void* data);
typedef void (*uart_read_complete_cb)(int status, void* data);
typedef void (*uart_timer_tick_cb)(void* data);

struct uart_config_t
{
  uart_write_complete_cb write_complete;
  void* write_complete_data;

  uart_read_complete_cb read_complete;
  void* read_complete_data;

  uart_timer_tick_cb timer_tick;
  void* timer_tick_data;
};

typedef void (*uart_waitfor)(int count);
typedef uint8_t (*uart_peek)();
typedef void (*uart_read_func)(uint8_t* data, int count);
typedef int (*uart_write_func)(uint8_t* data, int count);

struct uart_rx_t
{
	uart_waitfor waitfor;
	uart_peek peek;
	uart_read_func read;
};

struct uart_tx_t
{
	uart_write_func write;
};

struct uart_t
{
	uart_config_t config;
	uart_rx_t rx;
	uart_tx_t tx;
};

void uart_init(uart_t* uart);
void uart_configure(uart_config_t* config,
    uart_write_complete_cb write_complete,
    void* write_complete_data,
    uart_read_complete_cb read_complete,
    void* read_complete_data,
    uart_timer_tick_cb timer_tick,
    void* timer_tick_data);


typedef void (*sync_complete_cb)(int status);
typedef void (*sync_data_cb)(int addr, uint8_t* data);
typedef void (*oob_transfer_complete_cb)();
typedef int (*oob_data_cb)(uint8_t cmd, uint32_t data);

struct oob_entry_t
{
  uint8_t cmd;
  uint32_t data;
  oob_transfer_complete_cb complete;
};

enum out_phase_t {
	init = 0,
	data = 1,
	complete = 2
};

struct sync_state_t
{
	uart_t* uart;

	// out
	volatile int out_reset;
	volatile int out_resync;
	volatile out_phase_t phase;
	uint8_t* volatile ptr;
	volatile uint32_t out_dst;
	volatile uint32_t size;
	volatile uint8_t out_running_checksum;
	sync_complete_cb complete;
	volatile int ack_send_pending;
	volatile int ack_send_value;
	volatile int ack_receive_pending;
	volatile int ack_receive_value;
	oob_transfer_complete_cb ack_receive;
	volatile int transmit_active;

	volatile int oob_queue_write;
	volatile int oob_queue_read;
	volatile oob_entry_t oob_queue[16];

	// in
	volatile int in_reset;
	volatile int timer;
	volatile int in_dst;
	volatile uint8_t in_running_checksum;
	sync_data_cb data;
	oob_data_cb oob_data;
};

void sync_init(sync_state_t* state, uart_t* uart, sync_data_cb data, oob_data_cb oobdata);
void sync_block(sync_state_t* state, uint8_t* block, int offset, int size, sync_complete_cb complete_cb);
int sync_oob_word(sync_state_t* state, uint8_t cmd, uint32_t data, oob_transfer_complete_cb complete_cb);

#ifdef __cplusplus
};
#endif

#endif /* UART_H_ */
