/****************************************************************************
 Module
         EF_Timers.h

 Revision
         1.0.1

 Description
         Header File for the ME218 Timer Module

 Notes


 History
 When           Who	What/Why
 -------------- ---	--------
 10/21/11 18:33 jec  Begin conversion for use with the new Event Framework
 09/01/05 12:29 jec  Converted rates and return values to enumerated constants
 06/15/04 09:58 jec  converted all prefixes to EF_Timer
 05/27/04 15:04 jec  revised TMR_RATE macros for the S12
 11/24/00 13:17 jec  revised TMR_RATE macros for the HC12
 02/20/99 14:11 jec   added #defines for TMR_ERR, TMR_ACTIVE, TMR_EXPIRED
 02/24/97 14:28 jec   Began Coding
****************************************************************************/

#ifndef EF_Timers_H
#define EF_Timers_H

#include "Bin_Const.h"

/* these assume an 8MHz OSCCLK, they are the values to be used to program
    the RTICTL regiser
 */
typedef enum { EF_Timer_RATE_OFF  =   (0),
               EF_Timer_RATE_1MS  = B8(00100011), /* 1.024mS */
               EF_Timer_RATE_2MS  = B8(00110011), /* 2.048mS */
               EF_Timer_RATE_4MS  = B8(01000011), /* 4.096mS */
               EF_Timer_RATE_8MS  = B8(01010011), /* 8.196mS */
               EF_Timer_RATE_16MS = B8(01100011), /* 16.384mS */
               EF_Timer_RATE_32MS = B8(01110011)  /* 32.768mS */
} TimerRate_t;

typedef enum { EF_Timer_ERR           = -1,
               EF_Timer_ACTIVE        =  1,
               EF_Timer_OK            =  0,
               EF_Timer_NOT_ACTIVE    =  0,
               //EF_Timer_EXPIRED       =  0,
} EF_TimerReturn_t;

void             EF_Timer_Init(TimerRate_t Rate);
EF_TimerReturn_t EF_Timer_InitTimer(unsigned char Num, unsigned int NewTime);
EF_TimerReturn_t EF_Timer_SetTimer(unsigned char Num, unsigned int NewTime);
EF_TimerReturn_t EF_Timer_StartTimer(unsigned char Num);
EF_TimerReturn_t EF_Timer_StopTimer(unsigned char Num);
EF_TimerReturn_t EF_Timer_IsTimerActive(unsigned char Num);
unsigned int     EF_Timer_GetTime(void);

#endif   /* EF_Timers_H */
/*------------------------------ End of file ------------------------------*/


