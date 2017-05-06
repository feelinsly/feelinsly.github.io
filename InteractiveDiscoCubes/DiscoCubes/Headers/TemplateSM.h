/****************************************************************************
 
  Header file for Remote Lock Example Finite Sate Machine 
  based on the Event Framework

 ****************************************************************************/

#ifndef SMTemplate_H
#define SMTemplate_H

// Event Definitions
#include "EF_Events.h"
#include "misc.h"

// typedefs for the states
// State definitions for use with the query function
typedef enum { InitPState, UnlockWaiting, _1UnlockPress, 
               _2UnlockPresses, Locked } TemplateState_t ;


// Public Function Prototypes

boolean InitTemplateSM ( void );
boolean PostTemplateSM( EF_Event ThisEvent );
boolean IsTemplateSMQEmpty( void );
boolean RunTemplateSM( void );
TemplateState_t QueryTemplateSM ( void );


#endif /* SMTemplate_H */

