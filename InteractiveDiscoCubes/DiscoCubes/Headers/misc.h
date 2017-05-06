#ifndef MISC_H
#define MISC_H

typedef unsigned char boolean;

#define False ((boolean) 0)
#define True  ((boolean) !False)

/* Standard ANSI  99 C types */
#ifndef int8_t
typedef signed char int8_t;
#endif
#ifndef int16_t
typedef signed int   int16_t;
#endif
#ifndef int32_t
typedef signed long int    int32_t;
#endif

#ifndef uint8_t
typedef unsigned char       uint8_t;
#endif
#ifndef uint16_t
typedef unsigned int  uint16_t;
#endif
#ifndef uint32_t
typedef unsigned long int   uint32_t;
#endif

// these macros provide the wrappers for critical regions, where ints will be off
// but the state of the interrupt enable prior to entry will be restored.
extern unsigned char _CCR_temp;

#define EnterCritical()     { __asm pshc; __asm sei; __asm movb 1,SP+,_CCR_temp; } /* This macro saves CCR register and disables global interrupts. */
#define ExitCritical()  { __asm movb _CCR_temp, 1,-SP ; __asm pulc; } /* This macro restores CCR register saved EnterCritical(). */


#endif
