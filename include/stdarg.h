// stdarg.h (our copy): variable argument lists for game code, the way MSL defines them. Under
// CodeWarrior a va_list is the PowerPC EABI save area record, filled by the compiler's own
// __builtin_va_info; va_arg goes through MSL's __va_arg. SN ProDG (GCC 2.95, the shared file
// library) predates __builtin_va_list, so it gets the same record and no macros: none of its code
// takes variable arguments. Any other compiler gets its builtins.

#ifndef STDARG_H
#define STDARG_H

#if defined(__MWERKS__) || (defined(__GNUC__) && __GNUC__ < 3)
typedef struct {
    char gpr;                   // 0x00  next integer register to read
    char fpr;                   // 0x01  next float register to read
    char reserved[2];           // 0x02
    char* input_arg_area;       // 0x04  the caller's stack arguments
    char* reg_save_area;        // 0x08  where the prologue saved r3-r10 and f1-f8
} __va_list[1];
typedef __va_list va_list;
#endif

#ifdef __MWERKS__
void* __va_arg(va_list v_list, unsigned char type);

#define va_start(ap, fmt) ((void)fmt, __builtin_va_info(&ap))
#define va_arg(ap, t)     (*((t*)__va_arg(ap, _var_arg_typeof(t))))
#define va_end(ap)        (void)0
#elif !defined(__GNUC__) || __GNUC__ >= 3
typedef __builtin_va_list va_list;
#define va_start(ap, fmt) __builtin_va_start(ap, fmt)
#define va_arg(ap, t)     __builtin_va_arg(ap, t)
#define va_end(ap)        __builtin_va_end(ap)
#endif

#endif
