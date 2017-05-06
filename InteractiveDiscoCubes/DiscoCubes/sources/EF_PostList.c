/****************************************************************************
 Module
     EF_PostList.c
 Description
     source file for the module to post events to lists of state
     machines
 Notes
     Users should only modify the contents of the List0x arrays.
 History
 When           Who     What/Why
 -------------- ---     --------
 10/16/11 12:32 jec      started coding
*****************************************************************************/

#include "misc.h"
#include "EF_General.h"
#include "EF_PostList.h"

// Include the header files for the state machines. 
// This gets you the prototypes for the post functions.

#include "DiscoCubesSM.h"

// Fill in these arrays with the lists of posting funcitons for the state
// machines that will have common events delivered to them.

static PostFunc * const List00[] = {PostDiscoCubesSM };
static PostFunc * const List01[] = {PostDiscoCubesSM };
static PostFunc * const List02[] = {PostDiscoCubesSM };
static PostFunc * const List03[] = {PostDiscoCubesSM };
static PostFunc * const List04[] = {PostDiscoCubesSM };
static PostFunc * const List05[] = {PostDiscoCubesSM };
static PostFunc * const List06[] = {PostDiscoCubesSM };
static PostFunc * const List07[] = {PostDiscoCubesSM };

/****************************************************************************
 Leave Everything below this line alone!!
*****************************************************************************/
// Prototypes for private functions
static boolean PostToList(  PostFunc *const*FuncList, unsigned char ListSize, EF_Event NewEvent);

// Implementation for public functions

// Each of these list-specific functions is a wrapper that calls the generic
// function to walk through the list, calling the listed posting functions

/****************************************************************************
 Function
   PostListxx
 Parameters
   EF_Event NewEvent : the new event to be passed to each of the state machine
   posting functions in list xx
 Returns
   True if all the post functions succeeded, False if any failed
 Description
   Posts NewEvent to all of the state machines listed in the list
 Notes
   
 Author
   J. Edward Carryer, 10/24/11, 07:48
****************************************************************************/
boolean EF_PostList00( EF_Event NewEvent) {
  return PostToList( List00, ARRAY_SIZE(List00), NewEvent);
}

boolean EF_PostList01( EF_Event NewEvent) {
  return PostToList( List01, ARRAY_SIZE(List01), NewEvent);
}

boolean EF_PostList02( EF_Event NewEvent) {
  return PostToList( List02, ARRAY_SIZE(List02), NewEvent);
}

boolean EF_PostList03( EF_Event NewEvent) {
  return PostToList( List03, ARRAY_SIZE(List03), NewEvent);
}

boolean EF_PostList04( EF_Event NewEvent) {
  return PostToList( List04, ARRAY_SIZE(List04), NewEvent);
}

boolean EF_PostList05( EF_Event NewEvent) {
  return PostToList( List05, ARRAY_SIZE(List05), NewEvent);
}

boolean EF_PostList06( EF_Event NewEvent) {
  return PostToList( List06, ARRAY_SIZE(List06), NewEvent);
}

boolean EF_PostList07( EF_Event NewEvent) {
  return PostToList( List07, ARRAY_SIZE(List07), NewEvent);
}

// Implementations for private functions
/****************************************************************************
 Function
   PostToList
 Parameters
   PostFunc *const*List : pointer to the list of posting functions
   unsigned char ListSize : number of elements in the list array 
   EF_Event NewEvent : the new event to be passed to each of the state machine
   posting functions in the list
 Returns
   True if all the post functions succeeded, False if any failed
 Description
   Posts NewEvent to all of the state machines listed in the list
 Notes
   
 Author
   J. Edward Carryer, 10/24/11, 07:52
****************************************************************************/
static boolean PostToList( PostFunc *const*List, unsigned char ListSize, EF_Event NewEvent){
  unsigned char i;
  // loop through the list executing the post functions
  for ( i=0; i< ListSize; i++) {
    if ( List[i](NewEvent) != True )
      break; // this is a failed post
  }
  if ( i != ListSize ) // if no failures, i = ListSize
    return (False);
  else
    return(True);
}