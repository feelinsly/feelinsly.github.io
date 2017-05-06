//#define TEST
/****************************************************************************
 Module
     EF_Timers.c

 Description
     This is a module implementing  8 16 bit timers all using the RTI
     timebase

 Notes
     Everything is done in terms of RTI Ticks, which can change from
     application to application.

 History
 When           Who     What/Why
 -------------- ---     --------
 10/21/11 18:26 jec      begin conversion to work with the new Event Framework
 09/01/05 13:16 jec      converted the return types and parameters to use the
                         enumerated constants from the new header.
 08/31/05 10:23 jec      converted several return value tests in the test harness
                         to use symbolic values.
 06/15/04 09:56 jec      converted all external prefixes to TMRS12 to be sure
                         that we don't have any conflicts with the old libs
 05/28/04 13:53 jec      converted for 9S12C32 processor
 12/11/02 14:53 dos      converted for ICC11V6, unadorned char needs to be
                         called out as signed char, default is now unsigned
                         for a plain char.
 11/24/99 14:45 rmo		 updated to compile under ICC11v5.
 02/24/97 17:13 jec      added new function TMR_SetTimer. This will allow one
 			             function to set up the time, while another function
            			 actually initiates the timing.
 02/24/97 13:34 jec      Began Coding
****************************************************************************/

/*----------------------------- Include Files -----------------------------*/
#include <hidef.h>         /* common defines and macros */
#include <mc9s12c32.h>     /* derivative information */
#include <S12C32bits.h>    /* bit definitions  */
#include <bitdefs.h>
#include "EF_Timers.h"
#include "EF_General.h"
#include "EF_Events.h"
#include "EF_Framework.h"
/*--------------------------- External Variables --------------------------*/

/*----------------------------- Module Defines ----------------------------*/
#define BITS_PER_BYTE 8

/*------------------------------ Module Types -----------------------------*/

/*
   the size of Tflag sets the number of timers, char = 8, int =16 ...)
   to add more timers, you will need to change the data type and modify
   the initializaiton of TMR_TimerArray and TMR_MaskArray
*/

typedef unsigned char Tflag_t;

typedef unsigned int Timer_t;


/*---------------------------- Module Functions ---------------------------*/
void EF_Timer_RTI_Resp(void);

/*---------------------------- Module Variables ---------------------------*/
static Timer_t TMR_TimerArray[sizeof(Tflag_t)*BITS_PER_BYTE]=
                                            { 0x0,
                                              0x0,
                                              0x0,
                                              0x0,
                                              0x0,
                                              0x0,
                                              0x0,
                                              0x0 };

// make this one const to get it put into flash, since it will never change
static Tflag_t const TMR_MaskArray[sizeof(Tflag_t)*BITS_PER_BYTE]=
                                            { BIT0HI,
                                              BIT1HI,
                                              BIT2HI,
                                              BIT3HI,
                                              BIT4HI,
                                              BIT5HI,
                                              BIT6HI,
                                              BIT7HI };

static Tflag_t TMR_ActiveFlags;
static unsigned int time;  /* this is used by the default RTI routine */

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     EF_Timer_Init
 Parameters
     unsigned char Rate set to one of the TMR_RATE_XX values to set the
     RTI rate
 Returns
     None.
 Description
     Initializes the timer module by setting up the RTI with the requested
    rate
 Notes
     None.
 Author
     J. Edward Carryer, 02/24/97 14:23
****************************************************************************/
void EF_Timer_Init(TimerRate_t Rate)
{
   RTICTL = (unsigned char)Rate;     /* set RTI Rate */
   CRGFLG = _S12_RTIF;              /* clear any pending int */
   CRGINT |= _S12_RTIE;             /* enable the RTI int */
   EnableInterrupts;                /* make sure that interrupts are enabled*/
}

/****************************************************************************
 Function
     EF_Timer_SetTimer
 Parameters
     unsigned char Num, the number of the timer to set.
     unsigned int NewTime, the new time to set on that timer
 Returns
     EF_Timer_ERR if requested timer does not exist
     EF_Timer_OK  otherwise
 Description
     sets the time for a timer, but does not make it active.
 Notes
     None.
 Author
     J. Edward Carryer, 02/24/97 17:11
****************************************************************************/
EF_TimerReturn_t EF_Timer_SetTimer(unsigned char Num, unsigned int NewTime)
{
   if( Num >= ARRAY_SIZE(TMR_TimerArray) )
      return EF_Timer_ERR;  /* tried to set a timer that doesn't exist */
   TMR_TimerArray[Num] = NewTime;
   return EF_Timer_OK;
}

/****************************************************************************
 Function
     EF_Timer_StartTimer
 Parameters
     unsigned char Num the number of the timer to start
 Returns
     EF_Timer_ERR for error EF_Timer_OK for success
 Description
     simply sets the active flag in TMR_ActiveFlags to (re)start a
     stopped timer.
 Notes
     None.
 Author
     J. Edward Carryer, 02/24/97 14:45
****************************************************************************/
EF_TimerReturn_t EF_Timer_StartTimer(unsigned char Num)
{
   if( Num >= ARRAY_SIZE(TMR_TimerArray) )
      return EF_Timer_ERR;  /* tried to set a timer that doesn't exist */
   TMR_ActiveFlags |= TMR_MaskArray[Num]; /* set timer as active */
   return EF_Timer_OK;
}

/****************************************************************************
 Function
     EF_Timer_StopTimer
 Parameters
     unsigned char Num the number of the timer to stop.
 Returns
     EF_Timer_ERR for error (timer doesn't exist) EF_Timer_OK for success.
 Description
     simply clears the bit in TMR_ActiveFlags associated with this
     timer. This will cause it to stop counting.
 Notes
     None.
 Author
     J. Edward Carryer, 02/24/97 14:48
****************************************************************************/
EF_TimerReturn_t EF_Timer_StopTimer(unsigned char Num)
{
   if( Num >= ARRAY_SIZE(TMR_TimerArray) )
      return EF_Timer_ERR;  /* tried to set a timer that doesn't exist */
   TMR_ActiveFlags &= ~(TMR_MaskArray[Num]); /* set timer as inactive */
   return EF_Timer_OK;
}

/****************************************************************************
 Function
     EF_Timer_InitTimer
 Parameters
     unsigned char Num, the number of the timer to start
     unsigned int NewTime, the number of ticks to be counted
 Returns
     EF_Timer_ERR if the requested timer does not exist, EF_Timer_OK otherwise.
 Description
     sets the NewTime into the chosen timer and sets the timer actice to 
     begin counting.
 Notes
     None.
 Author
     J. Edward Carryer, 02/24/97 14:51
****************************************************************************/
EF_TimerReturn_t EF_Timer_InitTimer(unsigned char Num, unsigned int NewTime)
{
   if( Num >= ARRAY_SIZE(TMR_TimerArray) )
      return EF_Timer_ERR;  /* tried to set a timer that doesn't exist */
   TMR_TimerArray[Num] = NewTime;
   TMR_ActiveFlags |= TMR_MaskArray[Num]; /* set timer as active */
   return EF_Timer_OK;
}

/****************************************************************************
 Function
     EF_Timer_IsTimerActive
 Parameters
     unsigned char Num the number of the timer to check
 Returns
     EF_Timer_ERR if requested timer is not valid
     EF_Timer_NOT_ACTIVE if timer is not active
     EF_Timer_ACTIVE if it is active
 Description
     used to determine if a timer is currently counting.
 Notes
     None.
 Author
     J. Edward Carryer, 02/24/97 14:53
****************************************************************************/
EF_TimerReturn_t EF_Timer_IsTimerActive(unsigned char Num)
{
    if( Num >= ARRAY_SIZE(TMR_TimerArray) )
      return EF_Timer_ERR;  /* tried to set a timer that doesn't exist */
    else
      if ((TMR_ActiveFlags & TMR_MaskArray[Num])!= 0)
        return EF_Timer_ACTIVE;
      else
        return EF_Timer_NOT_ACTIVE;
}

/****************************************************************************
 Function
     GetTime
 Parameters
     None.
 Returns
     the current value of the module variable 'time'
 Description
     Provides the ability to grab a snapshot time as an alternative to using
      the library timers. Can be used to determine how long between 2 events.
 Notes
     this functionality is ancient, though this implementation in the library
     is new.
 Author
     J. Edward Carryer, 06/01/04 08:04
****************************************************************************/
unsigned int EF_Timer_GetTime(void)
{
   return (time);
}

/****************************************************************************
 Function
     EF_Timer_RTI_Resp
 Parameters
     None.
 Returns
     None.
 Description
     This is the new RTI response routine to support the timer module.
     It will increment time, to maintain the functionality of the
     GetTime() timer and it will check through the active timers,
     decrementing each active timers count, if the count goes to 0, it
     will post an event to the corresponding SM and clear the active flag to
     prevent further counting.
 Notes
     None.
 Author
     J. Edward Carryer, 02/24/97 15:06
****************************************************************************/
void interrupt 7 EF_Timer_RTI_Resp(void)
{
unsigned char i;

   CRGFLG = _S12_RTIF;   /* clear the source of the int */
   ++time;             /* keep the GetTime() timer running */
   if (TMR_ActiveFlags != 0) /* if !=0 , then at least 1 timer is active */
   {
      for(i=0; i<(sizeof(Tflag_t)*BITS_PER_BYTE); i++) /* find active ones*/
      {
        if ((TMR_ActiveFlags & TMR_MaskArray[i]) != 0) /* if active */
            if(--TMR_TimerArray[i] == 0) /* decrement, check if timed out */
            {
                EF_Event NewEvent;
                NewEvent.EventType = EF_TIMEOUT;
                EF_PostToSM(i, NewEvent); /* post event to the right SM*/
               /* i is the index into the SMDescList array in EF_Framework.c*/
                TMR_ActiveFlags &= ~(TMR_MaskArray[i]); /* stop counting */
            }
      }
   }
}
/*------------------------------- Footnotes -------------------------------*/
#ifdef TEST

#include <termio.h>
#include <stdio.h>
#include <timerS12.h>


#define TIME_OUT_DELAY 1221
signed char Message[]={0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x0d, 0};

void main(void)
{
   int p;
   puts("\rStarting, should delay for 10 seconds\r");
   EF_Timer_Init(EF_Timer_RATE_8MS);
   EF_Timer_InitTimer(0, TIME_OUT_DELAY); /* TIME_OUT_DELAY  = 10s w/ 8.19mS interval */
   EF_Timer_InitTimer(1, TIME_OUT_DELAY);
   EF_Timer_InitTimer(2, TIME_OUT_DELAY);
   EF_Timer_InitTimer(3, TIME_OUT_DELAY);
   EF_Timer_InitTimer(4, TIME_OUT_DELAY);
   EF_Timer_InitTimer(5, TIME_OUT_DELAY);
   EF_Timer_InitTimer(6, TIME_OUT_DELAY);
   EF_Timer_InitTimer(7, TIME_OUT_DELAY);
   while(EF_Timer_IsTimerExpired(0) != EF_Timer_EXPIRED)
      ;
   puts("Timed Out\r");

   EF_Timer_InitTimer(7, TIME_OUT_DELAY);
   for (p=0;p<10000;p++ ); /* kill some time */
   EF_Timer_StopTimer(7);
   if (EF_Timer_IsTimerActive(7) != EF_Timer_NOT_ACTIVE)
      puts("Timer Stop Failed\r");
   else
      puts("Timer Stop Succeded, restarting timeout\r");
   EF_Timer_StartTimer(7);
   while(EF_Timer_IsTimerExpired(7) != EF_Timer_EXPIRED)
      ;

   puts("Timed Out Again\r");

   DisableInterrupts;
}
#endif
/*------------------------------ End of file ------------------------------*/

