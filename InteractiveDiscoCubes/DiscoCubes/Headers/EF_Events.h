/****************************************************************************
 Module
     EF_Events.h
 Description
     header file with definitions for the event structure and enumerations
     of events
 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 10/23/11 22:01 jec      customized for Remote Lock problem
 10/17/11 08:47 jec      changed event prefixes to EF_ to match new framework
 08/09/06 14:30 jec      started coding
*****************************************************************************/

#ifndef EF_Events_H
#define EF_Events_H

#include "stdint.h"

// Universal events occupy the lowest entries, followed by user-defined events
typedef enum {  EF_NO_EVENT = 0,
                EF_INIT,   /* used to transition from initial pseudo-state */
                EF_NEW_KEY, /* signals a new key received from terminal */
				    EF_TIMEOUT, /* signals that the timer has expired */
                /* User-defined events start here */
                BUTTON_PUSHED,
                IR_UP_LIMIT_SWITCH,
                IR_DOWN_LIMIT_SWITCH,
                CUBE_INCORRECT,
                CUBE_CORRECT,
                TIME_OUT,
                GAME_OVER} EF_EventTyp_t ;

typedef struct EF_Event_t {
    EF_EventTyp_t EventType;    // what kind of event?
    uint16_t   EventParam;      // parameter value for use w/ this event
}EF_Event;


#endif                                  /* EF_Events_H */
