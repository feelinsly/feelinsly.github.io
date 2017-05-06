/****************************************************************************
 Module
     EF_Queue.c
 Description
     Implements a FIFO circular buffer of EF_Event in a block of memory
 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 08/09/11 18:16 jec      started coding
*****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
#include "EF_Queue.h"
#include "misc.h"
#include <ME218_C32.h>

/*----------------------------- Module Defines ----------------------------*/
// QueueSize is max number of entries in the queue
// CurrentIndex is the 'read-from' index,
// actually CurrentIndex + sizeof(EF_Queue_t)
// entries are made to CurrentIndex + NumEntries + sizeof(EF_Queue_t)
typedef struct {  unsigned char QueueSize;
                  unsigned char CurrentIndex;
                  unsigned char NumEntries;
} EF_Queue_t;

typedef EF_Queue_t * pQueue_t;

/*---------------------------- Module Functions ---------------------------*/

/*---------------------------- Module Variables ---------------------------*/

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
   EF_InitQueue
 Parameters
   EF_Event * pBlock : pointer to the block of memory to use for the Queue
   unsigned char BlockSize: size of the block pointed to by pBlock
 Returns
   max number of entries in the created queue
 Description
   Initializes a queue structure at the beginning of the block of memory
 Notes
   you should pass it a block that is at least sizeof(EF_Queue_t) larger than 
   the number of entries that you want in the queue. Since the size of an 
   EF_Event (at 4 bytes; 2 enum, 2 param) is greater than the 
   sizeof(EF_Queue_t), you only need to declare an array of EF_Event
   with 1 more element than you need for the actual queue.
 Author
   J. Edward Carryer, 08/09/11, 18:40
****************************************************************************/
unsigned char EF_InitQueue( EF_Event * pBlock, unsigned char BlockSize )
{
   pQueue_t pThisQueue;
   // initialize the Queue by setting up initial values for elements
   pThisQueue = (pQueue_t)pBlock;
   // use all but the structure overhead as the Queue
   pThisQueue->QueueSize = BlockSize - 1;
   pThisQueue->CurrentIndex = 0;
   pThisQueue->NumEntries = 0;
   return(pThisQueue->QueueSize);
}

/****************************************************************************
 Function
   EF_EnQueueFIFO
 Parameters
   EF_Event * pBlock : pointer to the block of memory in use as the Queue
   EF_Event Event2Add : event to be added to the Queue
 Returns
   boolean : True if the add was successful, False if not
 Description
   if it will fit, adds Event2Add to the Queue
 Notes

  Author
   J. Edward Carryer, 08/09/11, 18:59
****************************************************************************/
boolean EF_EnQueueFIFO( EF_Event * pBlock, EF_Event Event2Add )
{
   pQueue_t pThisQueue;
   pThisQueue = (pQueue_t)pBlock;
   // index will go from 0 to QueueSize-1 so use '<'
   if ( pThisQueue->NumEntries < pThisQueue->QueueSize)
   {  // save the new event, use % to create circular buffer in block
      // 1+ to step past the Queue struct at the beginning of the
      // block
      EnterCritical();   // save interrupt state, turn ints off
      pBlock[ 1 + ((pThisQueue->CurrentIndex + pThisQueue->NumEntries)
               % pThisQueue->QueueSize)] = Event2Add;
      pThisQueue->NumEntries++;          // inc number of entries
      ExitCritical();  // restore saved interrupt state
      
      return(True);
   }else
      return(False);
}


/****************************************************************************
 Function
   EF_DeQueue
 Parameters
   unsigned char * pBlock : pointer to the block of memory in use as the Queue
 Returns
   next available entry from Queue, EF_NO_EVENT if Queue was empty
 Description
   reads from the Queue
 Notes

 Author
   J. Edward Carryer, 08/09/11, 19:11
****************************************************************************/
EF_Event EF_DeQueue( EF_Event * pBlock )
{
   pQueue_t pThisQueue;
   EF_Event ReturnEvent;

   pThisQueue = (pQueue_t)pBlock;
   if ( pThisQueue->NumEntries > 0)
   {
      EnterCritical();   // save interrupt state, turn ints off
      ReturnEvent = pBlock[ 1 + pThisQueue->CurrentIndex ];
      // inc the index
      pThisQueue->CurrentIndex++;
      // this way we only do the modulo operation when we really need to
      if (pThisQueue->CurrentIndex >=  pThisQueue->QueueSize)
         pThisQueue->CurrentIndex = (unsigned char)(pThisQueue->CurrentIndex % pThisQueue->QueueSize);
      pThisQueue->NumEntries--; //dec number of elements since we took 1 out
      ExitCritical();  // restore saved interrupt state
   }else {
      ReturnEvent.EventType = EF_NO_EVENT;
      ReturnEvent.EventParam = 0;
   }
   return(ReturnEvent);
}

/****************************************************************************
 Function
   EF_IsQueueEmpty
 Parameters
   unsigned char * pBlock : pointer to the block of memory in use as the Queue
 Returns
   boolean : True if Queue is empty
 Description
   see above
 Notes

 Author
   J. Edward Carryer, 08/10/11, 13:29
****************************************************************************/
boolean EF_IsQueueEmpty( EF_Event * pBlock )
{
   pQueue_t pThisQueue;

   pThisQueue = (pQueue_t)pBlock;
   return(pThisQueue->NumEntries == 0);
}

#if 0
/****************************************************************************
 Function
   QueueFlushQueue
 Parameters
   unsigned char * pBlock : pointer to the block of memory in use as the Queue
 Returns
   nothing
 Description
   flushes the Queue by reinitializing the indecies
 Notes

 Author
   J. Edward Carryer, 08/12/06, 19:24
****************************************************************************/
void QueueFlushQueue( unsigned char * pBlock )
{
   pQueue_t pThisQueue;
   // doing this with a Queue structure is not strictly necessary
   // but makes it clearer what is going on.
   pThisQueue = (pQueue_t)pBlock;
   pThisQueue->CurrentIndex = 0;
   pThisQueue->NumEntries = 0;
   return;
}


#endif
/***************************************************************************
 private functions
 ***************************************************************************/

























































