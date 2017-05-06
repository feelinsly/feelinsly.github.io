/****************************************************************************
 Module
     EF_Framework.c
 Description
     source file for the top level functions of the EF framework
 Notes
     Users should only modify the contents of the SMList array.
 History
 When           Who     What/Why
 -------------- ---     --------
 10/17/11 12:24 jec      started coding
*****************************************************************************/

#include <stdio.h>
#include <ME218_C32.h>
#include "misc.h"
#include "EF_General.h"
#include "EF_PostList.h"
#include "EF_CheckEvents.h"
#include "EF_Framework.h"
#include "EF_FrameworkPrivate.h"

// Include the header files for the state machines.
// This gets you the prototypes for the public state machine functions.

#include "DiscoCubesSM.h"

// You fill in this array with the names of the state machine functions
// for each state machine that you use.
// The order is: InitFunction, PostFunction, QueueQueryFunction, RunFunction
// The first enry, at index 0, is the highest priority, with decreasing priority
// with higher indices
static EF_SMDesc const SMDescList[] =
{ {InitDiscoCubesSM, PostDiscoCubesSM, IsDiscoCubesSMQEmpty, RunDiscoCubesSM }, /* highest priority */
};

// Initialize this variable with the name of the posting function that you
// want executed when a new keystroke is detected. The default initialization
// distributes keystrokes to all state machines
static pPostFunc const pPostKeyFunc = EF_PostAll;


/****************************************************************************
 Leave Everything below this line alone!!
*****************************************************************************/
//*********************************
// private function prototypes
//*********************************

static boolean CheckSystemEvents( void );

//*********************************
// Implementation
//*********************************

/****************************************************************************
 Function
   EF_Initialize
 Parameters
   None
 Returns
   EF_Return_t : FailedPointer if any of the function pointers are NULL
                 FailedInit if any of the initialization functions failed
 Description
   Initialize all the state machines and tests for NULL pointers in the array
 Notes

 Author
   J. Edward Carryer, 10/23/11,
****************************************************************************/
EF_Return_t EF_Initialize( void ){
  unsigned char i;
  // loop through the list testing for NULL pointers and
  // executing the init functions
  for ( i=0; i< ARRAY_SIZE(SMDescList); i++) {
    if ( (SMDescList[i].InitFunc == (pInitFunc)0) ||
         (SMDescList[i].PostFunc == (pPostFunc)0) ||
         (SMDescList[i].QEmptyFunc == (pQEmptyFunc)0) ||
         (SMDescList[i].RunFunc == (pRunFunc)0) )
      return FailedPointer; // protect against NULL pointers

    if ( SMDescList[i].InitFunc() != True )
      return FailedInit; // this is a failed initialization
  }
  return Success;
}

/****************************************************************************
 Function
   EF_Run
 Parameters
   None
 Returns
   EF_Return_t : FailedRun is any of the run functions failed during execution
 Description
   This is the main framework function. It searches through the state
   machines to find one with a non-empty queue and then executes the
   state machine to process the event in its queue.
   while all the queues are empty, it searches for system generated or
   user generated events.
 Notes
   this function only returns in case of an error
 Author
   J. Edward Carryer, 10/23/11,
****************************************************************************/
EF_Return_t EF_Run( void ){

  unsigned char i;
  while(1){ // stay here unless we detect an error condition

    // loop through the list executing the run functions for state machines
    // with a non-empty queue
    do{
      for ( i=0; i< ARRAY_SIZE(SMDescList); i++) {
        if ( SMDescList[i].QEmptyFunc() != True ) { // queue not empty, run SM
            if( SMDescList[i].RunFunc() != True ) {
              return FailedRun;
            }
            break; // if state machine ran, it might have posted an event
                   // so re-start the queue checking
        }
      }

    }while ( i != ARRAY_SIZE(SMDescList) ); // any queue not empty
    
    // all the queues are empty, so look for new system or user detected events
    if (CheckSystemEvents() == False)
      EF_CheckUserEvents();
  }
}

/****************************************************************************
 Function
   EF_PostAll
 Parameters
   EF_Event : The Event to be posted
 Returns
   boolean : False if any of the post functions failed during execution
 Description
   posts to all of the state machine's queues by calling all of the post
   functions in SMDescList
 Notes

 Author
   J. Edward Carryer, 10/23/11,
****************************************************************************/
boolean EF_PostAll( EF_Event ThisEvent){

  unsigned char i;
  // loop through the list executing the post functions
  for ( i=0; i< ARRAY_SIZE(SMDescList); i++) {
    if ( SMDescList[i].PostFunc == (pPostFunc)0)
      break; // protect against NULL pointers
    if ( SMDescList[i].PostFunc( ThisEvent ) != True )
      break; // this is a failed post
  }
  if ( i == ARRAY_SIZE(SMDescList) ) // if no failures
    return (True);
  else
    return(False);
}

/****************************************************************************
 Function
   EF_PostToSM
 Parameters
   unsigned char : Which state machine to post to (index into SMDescList)
   EF_Event : The Event to be posted
 Returns
   boolean : False if the post function failed during execution
 Description
   posts to one of the state machine's queues by calling the post
   functions in SMDescList[ WhichMachine ]
 Notes
   used by the timer library to associate a timer with a state machine
 Author
   J. Edward Carryer, 10/23/11,
****************************************************************************/
boolean EF_PostToSM( unsigned char WhichMachine, EF_Event TheEvent){
  if ( WhichMachine < ARRAY_SIZE(SMDescList) )
    return SMDescList[WhichMachine].PostFunc( TheEvent );
  else
    return False;
}


//*********************************
// private functions
//*********************************
/****************************************************************************
 Function
   CheckSystemEvents
 Parameters
   None
 Returns
   boolean : True if a system event was detected
 Description
   check for system generated events and uses pPostKeyFunc to post to one
   of the state machine's queues
 Notes
   currently only tests for incoming keystrokes
 Author
   J. Edward Carryer, 10/23/11, 
****************************************************************************/
static boolean CheckSystemEvents( void ){
  
  if ( kbhit() != 0 ) // new key waiting?
  {
    EF_Event ThisEvent;
    ThisEvent.EventType = EF_NEW_KEY;
    ThisEvent.EventParam = getchar();
    (*pPostKeyFunc)( ThisEvent );
    return True;
  }
  return False;
}

