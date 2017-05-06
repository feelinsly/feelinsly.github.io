/****************************************************************************
 Module
     EF_Framework.h
 Description
     header file for use with the top level functions of the EF Event Framework
 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 10/17/06 07:41 jec      started coding
*****************************************************************************/

#ifndef EF_Framework_H
#define EF_Framework_H

#include "misc.h"
#include "EF_General.h"
#include "EF_Events.h"

typedef enum {
              Success = 0,
              FailedPost = 1,
              FailedRun,
              FailedPointer,
              FailedIndex,
              FailedInit
} EF_Return_t;

EF_Return_t EF_Initialize( void );
EF_Return_t EF_Run( void );
boolean EF_PostAll( EF_Event ThisEvent );
boolean EF_PostToSM( unsigned char WhichMachine, EF_Event ThisEvent);

#endif   // EF_Framework_H
