#ifndef _UART_H
#define _UART_H

#ifdef __cplusplus
extern "C" {
#endif

/* The serial port routines MetroTRK provides (dolphin_trk_glue.c); 0 means success. */
typedef int UARTError;

enum {
  kBaud57600 = 57600
};

UARTError InitializeUART(unsigned long baudRate);
UARTError WriteUARTN(const void* bytes, unsigned long length);

#ifdef __cplusplus
}
#endif

#endif // _UART_H
