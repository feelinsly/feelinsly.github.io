/****************************************************************************
 Module
     EF_Framework.h
 Description
     header file for use with the top level functions of the EF Event Framework
 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 10/17/11 07:49 jec      new header to match the rest of the framework
 08/09/11 09:30 jec      started coding
*****************************************************************************/
#ifndef EF_Queue_H
#define EF_Queue_H

#include "misc.h"
#include "EF_Events.h"

/* prototypes for public functions */

unsigned char EF_InitQueue( EF_Event * pBlock, unsigned char BlockSize );
boolean EF_EnQueueFIFO( EF_Event * pBlock, EF_Event Event2Add );
EF_Event EF_DeQueue( EF_Event * pBlock );
boolean EF_EnQueueFromInt( EF_Event * pBlock, EF_Event Event2Add );
//void EF_FlushQueue( unsigned char * pBlock );
boolean EF_IsQueueEmpty( EF_Event * pBlock );

#endif /*EF_Queue_H */

