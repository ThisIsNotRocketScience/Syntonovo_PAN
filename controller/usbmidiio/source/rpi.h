#ifndef RPI_H_
#define RPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "uart.h"

void rpi_init(uart_t* uart);
void rpi_start();

#ifdef __cplusplus
};
#endif

#endif /* RPI_H_ */
