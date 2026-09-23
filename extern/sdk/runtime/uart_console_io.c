#include "console_io.h"
#include "uart.h"
#include "dolphin/os.h"

int __TRK_write_console(__file_handle handle, unsigned char* buffer, size_t* count,
                        __idle_proc idle_proc);

static inline int __init_uart_console(void) {
  int err = 0;
  static int initialized = 0;

  if (initialized == 0) {
    err = InitializeUART(kBaud57600);

    if (err == 0)
      initialized = 1;
  }

  return (err);
}

/* This game's version: on a TDEV unit the output goes only to the debugger (TRK), not the UART. */
int __write_console(__file_handle handle, unsigned char* buffer, size_t* count,
                    __idle_proc idle_proc) {
  if ((OSGetConsoleType() & OS_CONSOLE_TDEV) == 0) {
    if (__init_uart_console() != 0) {
      return 1;
    }

    if (WriteUARTN(buffer, *count) != 0) {
      *count = 0;
      return 1;
    }
  }

  __TRK_write_console(handle, buffer, count, idle_proc);
  return 0;
}

int __close_console() { return 0; }
