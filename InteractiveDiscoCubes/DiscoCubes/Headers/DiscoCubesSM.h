/****************************************************************************
 
  Header file for Remote Lock Example Finite Sate Machine 
  based on the Event Framework

 ****************************************************************************/

#ifndef DISCOCUBESSM_H
#define DISCOCUBESSM_H

// Event Definitions
#include "EF_Events.h"
#include "misc.h"



// typedefs for the states
// State definitions for use with the query function
typedef enum { Dormant, Game_Initializing, 
         Game_Running, Game_Over_Win, Game_Over_Lose } DiscoCubesState_t ;


// Public Function Prototypes

boolean InitDiscoCubesSM ( void );
boolean PostDiscoCubesSM( EF_Event ThisEvent );
boolean IsDiscoCubesSMQEmpty( void );
boolean RunDiscoCubesSM( void );
DiscoCubesState_t QueryDiscoCubesSM ( void );

char* GetComputersAnswer(void);
uint16_t GetStartTime(void);


#endif /* DISCOCUBESSM_H */

