/****************************************************************************
 Module
     EF_CheckEvents.c
 Description
     source file for the module to call the User event checking routines
 Notes
     Users should only modify the contents of the EF_EventList array.
 History
 When           Who     What/Why
 -------------- ---     --------
 10/16/11 12:32 jec      started coding
*****************************************************************************/

#include "misc.h"
#include "EF_Events.h"
#include "EF_General.h"
#include "EF_CheckEvents.h"

// Include the header files for the module(s) with your event checkers. 
// This gets you the prototypes for the post functions.

#include "EventCheckers.h"

// Fill in this array with the names of your event checking functions

static CheckFunc * const EF_EventList[]={Check4ButtonPushed,Check4IRUpLimitSwitch,Check4IRDownLimitSwitch,Check4CubeCorrect };

/****************************************************************************
 Leave Everything below this line alone!!
*****************************************************************************/

// Implementation for public functions

/****************************************************************************
 Function
   EF_CheckUserEvents
 Parameters
   None
 Returns
   True if any of the user event checkers returned True, False otherwise
 Description
   loop through the EF_EventList array executing the event checking functions
 Notes
   
 Author
   J. Edward Carryer, 10/25/11, 08:55
****************************************************************************/
boolean EF_CheckUserEvents( void ) 
{
  unsigned char i;
  // loop through the array executing the event checking functions
  for ( i=0; i< ARRAY_SIZE(EF_EventList); i++) {
    if ( EF_EventList[i]() == True )
      break; // found a new event, so process it first
  }
  if ( i == ARRAY_SIZE(EF_EventList) ) // if no new events
    return (False);
  else
    return(True);
}
