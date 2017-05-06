/****************************************************************************
 Module
     EF_PostList.h
 Description
     header file for use with the module to post events to lists of state
     machines
 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 10/16/11 12:28 jec      started coding
*****************************************************************************/
#ifndef EF_PostList_H
#define EF_PostList_H

#include "misc.h"
#include "EF_Events.h"

typedef boolean PostFunc( EF_Event );

typedef PostFunc (*pPostFunc);

boolean EF_PostList00( EF_Event);
boolean EF_PostList01( EF_Event);
boolean EF_PostList02( EF_Event);
boolean EF_PostList03( EF_Event);
boolean EF_PostList04( EF_Event);
boolean EF_PostList05( EF_Event);
boolean EF_PostList06( EF_Event);
boolean EF_PostList07( EF_Event);

#endif // EF_PostList_H