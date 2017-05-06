/****************************************************************************
 Module
   d:\me218\DevProjects\EventFramrwork\EF_SMTemplate.c

 Revision
   1.0.1

 Description
   This is a template file for implementing state machines under the EF
   Event Framework.

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 11/07/11 11:26 jec      made the queue static
 10/30/11 17:59 jec      fixed references to CurrentEvent in RunTemplateSM()
 10/23/11 18:20 jec      began conversion from SMTemplate.c (02/20/07 rev)
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "misc.h"
#include "EF_General.h"
#include "EF_Queue.h"
#include "EF_Events.h"
#include "TemplateSM.h"

/*----------------------------- Module Defines ----------------------------*/
#define QUEUE_SIZE 3

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine
*/

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well.
// type of state variable should match htat of enum in header file
static TemplateState_t CurrentState;

// everybody needs a queue too!
static EF_Event SMQueue[QUEUE_SIZE+1];


/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitTemplateSM

 Parameters
     None

 Returns
     boolean, False if error in initialization, True otherwise

 Description
     Sets up the event queue and does any rother equired initialization for
     this state machine
 Notes

 Author
     J. Edward Carryer, 10/23/11, 18:55
****************************************************************************/
boolean InitTemplateSM ( void )
{
  unsigned char ucResult1;
  EF_Event ThisEvent;

  ucResult1 = EF_InitQueue( SMQueue, ARRAY_SIZE(SMQueue) );
  if ( ucResult1 ==  QUEUE_SIZE ) // successful queue init
  {
    // put us into the Initial PseudoState
    CurrentState = InitPState;
    // post the initial transition event
    ThisEvent.EventType = EF_INIT;
    if (PostTemplateSM( ThisEvent ) == True)
    {
        return True;
    }else
    {
        return False;
    }
  }else
  {
    return False;
  }
}

/****************************************************************************
 Function
     PostTemplateSM

 Parameters
     EF_Event ThisEvent , the event to post to the queue

 Returns
     boolean False if the Enqueue operation failed, True otherwise

 Description
     Posts an event to the state machine's queue
 Notes

 Author
     J. Edward Carryer, 10/23/11, 19:25
****************************************************************************/
boolean PostTemplateSM( EF_Event ThisEvent )
{
  boolean bResult3;

  bResult3 = EF_EnQueueFIFO( SMQueue, ThisEvent );

  return bResult3;
}

/****************************************************************************
 Function
    RunTemplateSM

 Parameters
   void

 Returns
   boolean, Flase if an error encountered, True otherwise

 Description
   add your description here
 Notes
   uses nested switch/case to implement the machine.
 Author
   J. Edward Carryer, 10/23/11, 18:38
****************************************************************************/
boolean RunTemplateSM( void )
{
  EF_Event ThisEvent;

  // pull an event from the queue
  ThisEvent = EF_DeQueue( SMQueue );

  // we should only call the Run function if we know that there is something in
  // the queue, so if we found EF_NO_EVENT something is wrong
  if (ThisEvent.EventType == EF_NO_EVENT )
  {
    return False;
  }else
  {
    switch ( CurrentState )
    {
        case InitPState :       // If current state is initial Psedudo State
            if ( ThisEvent.EventType == EF_INIT )// only respond to EF_Init
            {
                // this is where you would put any actions associated with the
                // transition from the initial pseudo-state into the actual
                // initial state

                // now put the machine into the actual initial state
                CurrentState = UnlockWaiting;
             }
             break;

        case UnlockWaiting :       // If current state is state one
            switch ( ThisEvent.EventType )
            {
                case EF_LOCK : //If event is event one
                {
                    // Execute action function for state one : event one
                    CurrentState = Locked;//Decide what the next state will be
                 }
                 break;

                // repeat cases as required for relevant events
            }                           // end switch on CurrentEvent
            break;
        // repeat state pattern as required for other states

    }                                   // end switch on Current State
    return True;
  }
}

 /****************************************************************************
 Function
     IsTemplateSMQEmpty

 Parameters
     void

 Returns
     boolean True if the Queue is empty, False if the queue has something in it

 Description
     tests if this state machine's queue is empty
 Notes

 Author
     J. Edward Carryer, 10/23/11, 19:28
****************************************************************************/
boolean IsTemplateSMQEmpty( void ){

  return EF_IsQueueEmpty( SMQueue );
}

/****************************************************************************
 Function
     QueryTemplateSM

 Parameters
     None

 Returns
     TemplateState_t The current state of the Template state machine

 Description
     returns the current state of the Template state machine
 Notes

 Author
     J. Edward Carryer, 10/23/11, 19:21
****************************************************************************/
TemplateState_t QueryTemplateSM ( void )
{
   return(CurrentState);
}

/***************************************************************************
 private functions
 ***************************************************************************/

