//-----------------------------------------------------------------------------
// serial.h - common function declarations for different serial implementations
//-----------------------------------------------------------------------------

#ifndef SERIAL_H_
#define SERIAL_H_
#ifdef __cplusplus
extern "C" {
#endif
static const int VER = 0x0101;

void serial_initialize(const uint32_t baud_rate_divisor);
int getchar(void);
int putchar(int);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SERIAL_H_ */
