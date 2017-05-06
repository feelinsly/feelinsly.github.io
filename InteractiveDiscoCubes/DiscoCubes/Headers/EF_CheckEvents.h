/****************************************************************************
 Module
     EF_CheckEvents.h
 Description
     header file for use with the data structures to define the event checking
     functions and the function to loop through the array calling the checkers
 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 10/16/11 17:17 jec      started coding
*****************************************************************************/

#ifndef EF_CheckEvents_H
#define EF_CheckEvents_H

#include "misc.h"

typedef boolean CheckFunc( void );

typedef CheckFunc (*pCheckFunc);

boolean EF_CheckUserEvents( void );


#endif  // EF_CheckEvents_H