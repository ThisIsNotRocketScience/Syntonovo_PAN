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

struct sync_state_t
{
	uart_t* uart;

	// out
	int out_reset;
	int phase;
	uint8_t* ptr;
	uint32_t out_dst;
	uint32_t size;
	uint8_t out_running_checksum;
	sync_complete_cb complete;
	int ack_send_pending;
	int ack_send_value;
	int ack_receive_pending;
	int ack_receive_value;
	oob_transfer_complete_cb ack_receive;
	int transmit_active;

	int oob_queue_write;
	int oob_queue_read;
	oob_entry_t oob_queue[16];

	// in
	int in_reset;
	int timer;
	int in_dst;
	uint8_t in_running_checksum;
	sync_data_cb data;
	oob_data_cb oob_data;
};

void sync_init(sync_state_t* state, uart_t* uart, sync_data_cb data, oob_data_cb oobdata);

#ifdef __cplusplus
};
#endif

#endif /* UART_H_ */
