#ifndef RPI_H_
#define RPI_H_

#ifdef __cplusplus
extern "C" {
#endif

void rpi_init();
void rpi_reset();
void rpi_write(uint8_t* data, int count);

// callback: 4 bytes of data
void rpi_cb(uint8_t* data);

#ifdef __cplusplus
};
#endif

#endif /* RPI_H_ */
