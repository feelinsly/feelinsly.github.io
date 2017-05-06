 // Event Checking functions for use in testing the CheckUserEvents 
// functionality

#include <ME218_C32.h>
#include "EF_General.h"
#include "EF_Events.h"
#include "EF_PostList.h"
#include "EF_Timers.h"
#include "EventCheckers.h"
#include <stdio.h>


// in a real application, this would be an include for all of the state
// machine functions
#include "DiscoCubesSM.h"


#define DEBOUNCE_TIME 30
#define NO_OF_CUBES 5


/*********************************************************
EventChecker to check
**********************************************************/
boolean Check4ButtonPushed(void); //Post event: BUTTON_PUSHED
boolean Check4IRUpLimitSwitch(void);//Post event: IR_UP_LIMIT_SWITCH
boolean Check4IRDownLimitSwitch(void);//Post event: IR_DOWN_LIMIT_SWITCH
boolean Check4CubeCorrect(void);//Post event: CUBE_CORRECT, CUBE_INCORRECT
void PulseCLK(void);

//Checks whether the start button has been pressed.
boolean Check4ButtonPushed(void)
{ 
   static char CurrentButtonState;
   static char LastButtonState = 0x0;
   uint16_t TimeOfCurrentSample;
	boolean ReturnVal;
	EF_Event new_event;
   static uint16_t TimeOfLastSample = 0; 
   
   ReturnVal = False;
	CurrentButtonState = PTIAD & BIT2HI;
	//printf("currentbuttonstate is %d",CurrentButtonState);
	TimeOfCurrentSample = EF_Timer_GetTime();

	if((TimeOfCurrentSample - TimeOfLastSample) > DEBOUNCE_TIME) //If more than the debounce interval has elapsed since the last sampling 
	{ 
		if(CurrentButtonState != LastButtonState) //If the current button state is different from the LastButtonState 
		{  
			if(CurrentButtonState > 0)  //If the current button state is DOWN 
			{
			    new_event.EventType = BUTTON_PUSHED;
		       new_event.EventParam = TimeOfCurrentSample;
		       EF_PostList00(new_event);//Post Event
		       ReturnVal = True;
			}
			LastButtonState = CurrentButtonState;  //Set LastButtonState to the CurrentButtonState 
			TimeOfLastSample = TimeOfCurrentSample;  
		}
	}		
	return ReturnVal; 
}

//Checks whether the lower limit of motion has been exceeded.
boolean Check4IRDownLimitSwitch(void)//Post event: IR_DOWN_LIMIT_SWITCH
{
  unsigned char CurrentIRDownState;
  EF_Event new_event;
  boolean ReturnVal;
  static unsigned char LastIRDownState;
  //puts("checkingIRdown");
  
  ReturnVal = False;
  CurrentIRDownState = PTT & BIT6HI;
  
  if (CurrentIRDownState != LastIRDownState)
  {
   if (CurrentIRDownState == BIT6HI)
   {  puts("ir down event");
      new_event.EventType = IR_DOWN_LIMIT_SWITCH;
      EF_PostList00(new_event);
      ReturnVal = True;
   }
  }
  LastIRDownState = CurrentIRDownState;
  return ReturnVal;
}

//Checks whether the upper limit of motion has been exceeded.
boolean Check4IRUpLimitSwitch(void)//Post event: IR_UP_LIMIT_SWITCH
{
  unsigned char CurrentIRUpState;
  EF_Event new_event;
  boolean ReturnVal;
  static unsigned char LastIRUpState;
  //puts("checkingIRup");
  
  ReturnVal = False;
  CurrentIRUpState = PTT & BIT7HI;
  
  if (CurrentIRUpState != LastIRUpState)
  {
   if (CurrentIRUpState == 0x0)
   {  //puts("ir up event");
      new_event.EventType = IR_UP_LIMIT_SWITCH;
      EF_PostList00(new_event);
      ReturnVal = True;
   }
  }
  LastIRUpState = CurrentIRUpState;
  return ReturnVal;
}
      
/**********************************************************************
Magnet Reed Switch and Cube Placement
************************************************************************
We'd like all 6 cube sides be able to be triggered
	
	5 cubes --> 15 LEDs --> 15*2*2 foil contacts
	-->  5*2*3 magnets (2 for each color)
	--> 5*3 Magnet Reed Switches

15 Magnet Reed Switches are connected to two 74HC165s

4th pillar -- C1 B1 A1 (in the order of R G B)
3rd pillar -- F1 E1 D1
2nd pillar -- A2 H1 G1
1st pillar -- D2 C2 B2
0th pillar -- G2 F2 E2

Figure showing positions for magnet reed switch on each pillar cap:
_________
|   G   |
|       |
|R      |
|_____B_|

//example of a computersanwer/usersanswer  [0 2 2 0 1] representing 0 001 100 100 001 010

      |0|   |1|
         |2|
      |3|   |4|  


************************************************************************/

boolean Check4CubeCorrect(void)
{
  EF_Event new_event; 
  char *answer;
  char CurrentReedSwitchState[NO_OF_CUBES];
  static char LastReedSwitchState[NO_OF_CUBES];
  char j;
  char i;
  uint16_t t;
  boolean ReturnVal;
  //puts("checkingcubes");
  
  j=0;
  ReturnVal = False; 
  t = EF_Timer_GetTime(); 
  answer = GetComputersAnswer();
  
  PTAD &= BIT1LO; //now all the data appears on the register
  while(EF_Timer_GetTime() == t); //do nothing for 1 ms.
  PTAD |= BIT1HI; //now we allow the register's new data to be shifted out in series.
 
  for (i = 0; i < 15; i++)
  {
   if ((PTT&BIT4HI) == BIT4HI)
   {  
      printf("PTT&BIT4HI is: %d \n\r",PTT&BIT4HI);
      
      CurrentReedSwitchState[i/3] = i%3;
      
      printf("computer answer is %d \n\r", answer[i/3]);
      printf("CurrentReedSwitchState is %d \n\r", CurrentReedSwitchState[i/3]);
      if(CurrentReedSwitchState[i/3] != LastReedSwitchState[i/3])
      {
         if (answer[i/3] == CurrentReedSwitchState[i/3])
         { 
            puts("cube correct");
            new_event.EventType = CUBE_CORRECT;
            new_event.EventParam = i/3; //says which cube it is.
            printf("Cube #: %d \n\r", i/3);
            EF_PostList00(new_event);
         }else 
         {  
            puts("cube incorrect");
            new_event.EventType = CUBE_INCORRECT;
            new_event.EventParam = i/3;
            EF_PostList00(new_event);
         }
      LastReedSwitchState[i/3] = CurrentReedSwitchState[i/3];
      }
   } 
    //if PTT4 is not HIGH for all 3 bits of a cube 
   else
   {  
      if (i%3 == 0)  //if(((PTT&BIT4HI) == 0))
      {              //### #1# 2## 2## ##0
         j = i;      //444 333 222 111 000
         j++;        //210 210 210 210 210
      }  	                  if (j == i+1)           
      {                     
         j++;
      }
		if (j == i+2)
		{
		 	CurrentReedSwitchState[i/3] = '#';   
		   LastReedSwitchState[i/3] = CurrentReedSwitchState[i/3];
	   }
   }
   ReturnVal = True;
   PulseCLK(); 
  }
  return ReturnVal;
}

//Pulses the clock to shift out data to C32.
void PulseCLK(void)
{
   int i;
   PTM &= BIT5LO;
   for (i = 0; i < 1; i++) 
   {
      volatile int dummy;
   }
   
   PTM |= BIT5HI;
   for (i = 0; i < 1; i++) 
   {
      volatile int dummy;
   }
   printf("");
   
   return;
   
}
  
