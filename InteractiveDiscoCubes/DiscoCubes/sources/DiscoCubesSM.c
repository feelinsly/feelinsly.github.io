/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "misc.h"
#include "EF_General.h"
#include "EF_Queue.h"
#include "EF_Events.h"
#include "EF_Timers.h"
#include "EF_PostList.h"
#include "DiscoCubesSM.h"
#include <Bin_Const.h>
#include "ADS12.h"
#include "PWMS12.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ME218_C32.h>

/*----------------------------- Module Defines ----------------------------*/
#define QUEUE_SIZE 3
#define NO_OF_CUBES 5
#define BLUE B16(00000000,00000010)
#define GREEN B16(00000000,00000100)
#define RED B16(00000000,00001000)

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine*/
void InitAnswers(void);
unsigned int ReadDifficultyKnob( void);
void PulseSCKU4U5(void);
void PulseRCKU4U5(void);
void PulseSCKU1(void);
void PulseRCKU1(void);
void StartMotorUp(unsigned int difficulty);
void StartMotorDown(unsigned int difficulty);
void DisplayAnswer( void);
char RandColor(void);
void TurnOnCubeLEDs(void);
unsigned int IncreaseMotorSpeed(int difficulty);
void Switch2Colors( void); 
void ActivateBuzzer(uint16_t param);
void ClearAllLEDs(void);
void UpdateUsersAnswer(uint16_t param);
void YouWinLEDMessage(void);
void AlternatePillars(void);
void StopAllMotors(void);
char* GetComputersAnswer(void);
uint16_t GetStartTime(void);
boolean CompareAnswers(void);




/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well.
// type of state variable should match htat of enum in header file
static DiscoCubesState_t CurrentState;

static char computersanswer[NO_OF_CUBES];
static char usersanswer[NO_OF_CUBES];
static uint16_t start_time;
static unsigned int PanelLED;

// everybody needs a queue too!
static EF_Event SMQueue[QUEUE_SIZE+1];


/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitDiscoCubesSM

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
boolean InitDiscoCubesSM ( void )
{
  unsigned char ucResult1;
  EF_Event ThisEvent;

  ucResult1 = EF_InitQueue( SMQueue, ARRAY_SIZE(SMQueue) );
  if ( ucResult1 ==  QUEUE_SIZE ) // successful queue init
  {
    // put us into the Initial PseudoState
    CurrentState = Dormant; //Initialize to the dormant state
    
    //Ports 0 1 and 2 are set to PWM in the PWM init method.
    //Port 0 controls motor 1, Port 1 controls motors 2,3.
    DDRT &= BIT4LO;//Set PTT4 as input for 74HC165 for Magnetic Reed Switches
    DDRT |= BIT5HI;//Set PTT5 as output to L293B for enable
    DDRT &= BIT6LO;//Set PTT6 as input for sensing IR limit switch state (down)
    DDRT &= BIT7LO;//Set PTT7 as input for sensing IR limit switch state (up) 
    
    
    DDRM |= BIT0HI;//Set PTM0 as output to RCK of shift register for Cube LEDs
    DDRM |= BIT1HI;//Set PTM1 as output to datainput of shift register for Cube LEDs
    
    /*Port initialization for instruction LED in front panel*/
    DDRM |= BIT2HI;// Set Port M2 as output to Datainput of Shift Register for LED panel 
    DDRM |= BIT3HI;//Set Port M3 as output to RCK of both 74hc595s for LED panel
    DDRM |= BIT4HI;//Set Port M4 as output to buzzer
    DDRM |= BIT5HI;//Set Port M5 as output to all SCKs.
    
    /*PTM &= BIT4LO;//Initialize buzzer to off state.
    PTT &= BIT3LO;//Initialize motor 1 to off state.
    PTM &= BIT5LO;//Initialize shift clocks to low state.
    PTM &= BIT3LO;*/
    PTT = 0x0;
    PTM = 0x0; 
    
    if (ADS12_Init("OOOOIIOA") == ADS12_OK) 
    //AD0 is the knob, AD1 is shift/~load for the 74HC165, AD2 is the start button, 
    //AD3 is NOTHING and AD4 to AD7 are the buzzers.
    {
    puts("init ok");
    }else
    {
    puts("adsinit error");
    }//initialize AD0 as analog input of Difficulty Level

	PTAD = 0x0;

    PWMS12_Init(); //initialize PWM capabilities.  
    ClearAllLEDs();
    
    
    // post the initial transition event
    ThisEvent.EventType = EF_INIT;
    if (PostDiscoCubesSM( ThisEvent ) == True)
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
     PostDiscoCubesSM

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
boolean PostDiscoCubesSM( EF_Event ThisEvent )
{
  boolean bResult3;

  bResult3 = EF_EnQueueFIFO( SMQueue, ThisEvent );

  return bResult3;
}

/****************************************************************************
 Function
    RunDiscoCubesSM

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
boolean RunDiscoCubesSM( void )
{
  EF_Event ThisEvent;
  static int difficulty; //difficulty level
  static unsigned int led = 0; //tells us which front panel LED to light
  static int n; //number of times the user puts a wrong cube    
  ThisEvent = EF_DeQueue( SMQueue );// pull an event from the queue
  
  
  // we should only call the Run function if we know that there is something in
  // the queue, so if we found EF_NO_EVENT something is wrong
  if (ThisEvent.EventType == EF_NO_EVENT )
  { puts("failed on runSM");
    return False;
  }else
  {
    switch ( CurrentState )
    {
         case Dormant :
		   	n = 0; //number of mistakes
		   	if ((PTT & BIT6HI) != BIT6HI )
		   	{
		   	   StartMotorDown(30);
		   	}
		   	if(ThisEvent.EventType == IR_DOWN_LIMIT_SWITCH)
		   	{
		   	   StopAllMotors();
		   	}
            if (ThisEvent.EventType == BUTTON_PUSHED)    //if start button has been pushed
            {  
               puts("Got in Button Pushed event");
               ClearAllLEDs();
               InitAnswers(); //after button press, not every cycle
               difficulty = ReadDifficultyKnob();
               printf("difficulty %d",difficulty); //reads current level of difficulty knob ONCE
			      CurrentState = Game_Initializing; //takes us to game initializing state
               StartMotorUp(difficulty); //starts motor and raises pillars.  IR sensor has a limit switch to stop                        
            }
            if ((ThisEvent.EventType == EF_INIT) | (ThisEvent.EventType == EF_TIMEOUT))
            {
               switch (led%5)
               {
                  int i;
                  case 0:
                     PanelLED = RED+GREEN+BLUE;
                     for(i = 0; i < 16; i++) //Repeat 16 times
                     {
                        if (PanelLED & B16(00000000,00000001) == 0x1)   
                        {
                           PTM |= BIT2HI; //BIT2 on port M is connected to the data input for the 74HC595
                        }else
                        {
                           PTM &= BIT2LO;
                        }
                        PulseSCKU4U5();//pulse the SCK line 
                        PanelLED = PanelLED >> 1;  
                     }
                     PulseRCKU4U5();
                     led++;
                     EF_Timer_InitTimer(0, 500);
                     break;
               
                  case 1:
                     PanelLED = (RED+GREEN+BLUE) << 3;
                     for(i = 0; i < 16; i++) //Repeat 16 times
                     {
                        if (PanelLED & B16(00000000,00000001) == 0x1)   
                        {
                           PTM |= BIT2HI; //BIT2 on port M is connected to the data input for the 74HC595
                        }else
                        {
                           PTM &= BIT2LO;
                        }
                        PulseSCKU4U5();//pulse the SCK line 
                        PanelLED = PanelLED >> 1;  
                     }
                     PulseRCKU4U5();
                     led++;
                     EF_Timer_InitTimer(0, 500);
                     break;
                     
                  case 2:
                     PanelLED = (RED+GREEN+BLUE) << 6;
                     for(i = 0; i < 16; i++) //Repeat 16 times
                     {
                        if (PanelLED & B16(00000000,00000001) == 0x1)   
                        {
                           PTM |= BIT2HI; //BIT2 on port M is connected to the data input for the 74HC595
                        }else
                        {
                           PTM &= BIT2LO;
                        }
                        PulseSCKU4U5();//pulse the SCK line 
                        PanelLED = PanelLED >> 1;  
                     }
                     PulseRCKU4U5();
                     led++;
                     EF_Timer_InitTimer(0, 500);
                     break;
                  
                  case 3:
                     PanelLED = (RED+GREEN+BLUE) << 9;
                     for(i = 0; i < 16; i++) //Repeat 16 times
                     {
                        if (PanelLED & B16(00000000,00000001) == 0x1)   
                        {
                           PTM |= BIT2HI; //BIT2 on port M is connected to the data input for the 74HC595
                        }else
                        {
                           PTM &= BIT2LO;
                        }
                        PulseSCKU4U5();//pulse the SCK line 
                        PanelLED = PanelLED >> 1;  
                     }
                     PulseRCKU4U5();
                     led++;
                     EF_Timer_InitTimer(0, 500);
                     break;
                  
                  case 4:
                     PanelLED = (RED+GREEN+BLUE) << 12;
                     for(i = 0; i < 16; i++) //Repeat 16 times
                     {
                        if (PanelLED & B16(00000000,00000001) == 0x1)   
                        {
                           PTM |= BIT2HI; //BIT2 on port M is connected to the data input for the 74HC595
                        }else
                        {
                           PTM &= BIT2LO;
                        }
                        PulseSCKU4U5();//pulse the SCK line 
                        PanelLED = PanelLED >> 1;  
                     }
                     PulseRCKU4U5();
                     led++;
                     EF_Timer_InitTimer(0, 500);
                     break;
                  
               }
            }
		   break;                                
            
         case Game_Initializing :
            if (ThisEvent.EventType == IR_UP_LIMIT_SWITCH) 
            {  
               StopAllMotors();
               DisplayAnswer();//lights up the LED display on the front panel showing answer
               //puts("displayanswers after");
               TurnOnCubeLEDs();                                 
            }else if (ThisEvent.EventType == CUBE_INCORRECT) 
		      {
		         ActivateBuzzer(ThisEvent.EventParam);
		      }else if (ThisEvent.EventType == CUBE_CORRECT)
		      {
			      CurrentState = Game_Running;
			      //start_time = EF_Timer_GetTime();
			      EF_Timer_InitTimer(0, 35000);
               UpdateUsersAnswer(ThisEvent.EventParam);
			      StartMotorDown(difficulty); //starts motor to lower pillars
		      }
		   break;                                  
           
         case Game_Running : 
            puts("game running");              
            if (ThisEvent.EventType == CUBE_CORRECT) //user has placed a cube correctly during game play
            {  puts("cube correct in game");
               UpdateUsersAnswer(ThisEvent.EventParam);
			      if (CompareAnswers() == True){
				      CurrentState = Game_Over_Win;
				      ThisEvent.EventType = GAME_OVER;
   			      EF_PostList00(ThisEvent);
			      }                    
            }else if (ThisEvent.EventType == CUBE_INCORRECT)
		      {  puts("cube wrong in game");
               n++;
		         ActivateBuzzer(ThisEvent.EventParam);
			      if (n == 1){
			         puts("n = 1");
				      //difficulty = IncreaseMotorSpeed(difficulty);
			      }
			      if (n >= 2){
			         puts("n>=2");
				      Switch2Colors(); //switches 2 colors on the LED display
   			   }
   		   }else if (ThisEvent.EventType == IR_DOWN_LIMIT_SWITCH)
   		   {
   		      StopAllMotors();
   			   CurrentState = Game_Over_Lose;
   			   ThisEvent.EventType = GAME_OVER;
   			   EF_PostList00(ThisEvent);
   		   }else if (ThisEvent.EventType == EF_TIMEOUT)
   		   {
   		      StopAllMotors();
   			   CurrentState = Game_Over_Lose;
   			   ThisEvent.EventType = GAME_OVER;
   			   EF_PostList00(ThisEvent);
   		   }
   		break;

         case Game_Over_Win :
            puts ("in gameover win"); 
            if (ThisEvent.EventType == GAME_OVER)
            {
               YouWinLEDMessage();
               StartMotorUp(difficulty);
            }           
            if (ThisEvent.EventType == IR_UP_LIMIT_SWITCH)
            {
               AlternatePillars();
               StartMotorDown(difficulty);
            }else if (ThisEvent.EventType == IR_DOWN_LIMIT_SWITCH)
            {
               StopAllMotors();
               ClearAllLEDs();
               CurrentState = Dormant;
               ThisEvent.EventType = EF_INIT;
               PostDiscoCubesSM(ThisEvent);
            }
         break;

   		case Game_Over_Lose :
   		   if (ThisEvent.EventType == GAME_OVER)
   		   {
   		      puts("gameover");
   		      ClearAllLEDs();
   		      CurrentState = Dormant;
               ThisEvent.EventType = EF_INIT;
               PostDiscoCubesSM(ThisEvent);  
   		   }
   		break;
    }               // end switch on Current State
    return True;
  }
}

 /****************************************************************************
 Function
     IsDiscoCubesSMQEmpty

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
boolean IsDiscoCubesSMQEmpty( void ){

  return EF_IsQueueEmpty( SMQueue );
}

/****************************************************************************
 Function
     QueryDiscoCubesSM

 Parameters
     None

 Returns
     DiscoCubesState_t The current state of the DiscoCubes state machine

 Description
     returns the current state of the DiscoCubes state machine
 Notes

 Author
     J. Edward Carryer, 10/23/11, 19:21
****************************************************************************/
DiscoCubesState_t QueryDiscoCubesSM ( void )
{
   return(CurrentState);
}

/***************************************************************************/
char* GetComputersAnswer(void)
{
  return computersanswer;
}

/**************************************************************************/
uint16_t GetStartTime(void)
{
   return start_time;
}

/***************************************************************************
 private functions
 ***************************************************************************/

void InitAnswers()
{
   int i;
   for(i = 0; i<NO_OF_CUBES; i++)//Generate a Random Array of colors
   {
      computersanswer[i] = RandColor();
      printf("computersanswer %d",computersanswer[i]);          
   } 
   for(i = 0; i<NO_OF_CUBES; i++)//Generate a Random Array of colors
   {
      usersanswer[i] = '#';
      printf("usersanswer %d",usersanswer[i]);
                
   } 
}
/***************************************************************************
This Function Read Difficulty Level, deciding the initial Falling Speed of the Pillars
****************************************************************************/ 
unsigned int ReadDifficultyKnob( void)
{
    unsigned int difficulty;//initialize local difficulty
  
    difficulty = ADS12_ReadADPin(0);//Read AD0
    difficulty = (10*difficulty/1024)*5;
    
    return difficulty;
}


/*******************************************************************
This Function Light up LEDs in the Front Panel successively in dormant state 

This explain the way how tri-color LEDs are connected to Shift Register
**********************************************************************

5 Tri-Color LEDs -- in total 15 LEDs 
are connected to the outputs of two 74HC595s

The LEDs shown in the front panel are numbered as shown below
      |0|   |1|
         |2|
      |3|   |4|                                                 
0th LEDs are connected to QA1 QB1 QC1 (R G B)
1st LEDs are connected to QD1 QE1 QF1 (R G B) 
2nd LEDs are connected to QG1 QH1 QA2 (R G B)
3rd LEDs are connected to QB2 QC2 QD2 (R G B)
4th LEDs are connected to QE2 QF2 QG2 (R G B)

We use
M2 for Datainput
M4 for SCK
M3 for RCK

**********************************************************************
End Of Explain the way how tri-color LEDs are connected to Shift Register
******************************************************************/

/*********************************************************************
Display a random LED pattern on the front panel
*****************************************************************/
void DisplayAnswer( void)
{  
   unsigned char i;
   unsigned char j;
   unsigned int cube_pattern = 0x0;
   
   puts("starts display answer");
   //Defines an integer, cube_pattern, that stores the bitwise representation of the computer's answer.
   for (j = 0; j < NO_OF_CUBES; j++)
   {
      char color = computersanswer[j];
      printf("color is: %d \r\n",color);
      switch (color)
      {
         case 0 :
           cube_pattern += (BLUE<<(j*3));
           printf("case0--cube_pattern is: %d\r\n",cube_pattern); 
           break;
         case 1 :
           cube_pattern += (GREEN<<(j*3));
           printf("case1--cube_pattern is: %d\r\n",cube_pattern); 
           break;
         case 2 : 
           cube_pattern += (RED<<(j*3));
           printf("case2--cube_pattern is: %d\r\n",cube_pattern); 
           break;
      }
   }
   //physically displays our answer on the LED panel.
   for(i = 0; i < 16; i++) //Repeat 16 times
   {
      if ((cube_pattern & B16(00000000,00000001)) == 0x1)//place FrontPanelLED onto the data output pin 
      ///make into a shiftin function 
      {
         PTM |= BIT2HI; //BIT2 on port M is connected to the data input for the 74HC595
      }else
      {
         PTM &= BIT2LO;
      }
      PulseSCKU4U5();//pulse the SCK line
      cube_pattern = cube_pattern >> 1;
   }
   PulseRCKU4U5();
}




//argument will be an integer, 0 through 4, corresponding to one of the five cubes.
void UpdateUsersAnswer(uint16_t param)
{
   usersanswer[param] = computersanswer[param]; 
}




char RandColor(void)  //generate a random 0,1 or 2 to represent red, green and blue.
{
   char randomcolor;
   srand(EF_Timer_GetTime());
   randomcolor = ((unsigned char)rand())%3;   // R=2 G=1 B=0
   return randomcolor; 
}

/*******************************************************************
Cube topview:
      |0|   |1|
         |2|
      |3|   |4|   

Cube LEDs are connected to U1 74HC595 shift register

0th cube -- QA
1st cube -- QB
2nd cube -- QC
3rd cube -- QD
4th cube -- QE
********************************************************************/

//turns on all cube LED circuits so that they light up when you place them on the pillars.
void TurnOnCubeLEDs(void)
{
   int i;
   
   for (i = 0; i < NO_OF_CUBES; i++)
   {
      PTM |= BIT1HI; //PTM1 outputs HI to Datainput of U1
      PulseSCKU1();
   }
   PulseRCKU1();
}





void Switch2Colors( void) 
{
   char temp;
   char randomcube2;
   char randomcube1;
   
   srand(EF_Timer_GetTime());
   
   randomcube1 = ((unsigned char)rand())%5; 
   randomcube2 = ((unsigned char)rand())%5; 
   
   printf("rando1 %d,rando2%d",randomcube1,randomcube2 );
   
   temp = computersanswer[randomcube1];
   computersanswer[randomcube1] = computersanswer[randomcube2];
   computersanswer[randomcube2] = temp;
   
   DisplayAnswer();
}




//starts motors pushing the pillars up
void StartMotorUp(unsigned int difficulty)
{
  PTT |= BIT5HI; //enables L293NE
  
  //N.B. at a duty cycle of 50, the motor will not move, since in the H-bridge configuration, low corresponds 
  //to moving in one direction, and high in another.  Therefore, we modify "difficulty" to reflect this fact.  
  //Furthermore, we can exploit this phenomenon in startmotor up and start motor down.  Lastly, motor 1 is wired 
  //opposite to motors 2,3 and so a duty cycle of 70 for all motors sets motors 2,3 moving in one direction and motor
  //1 moving in the opposite direction.    
  PWMS12_SetDuty((unsigned char)(50+difficulty),PWMS12_CHAN0);
  PWMS12_SetDuty((unsigned char)(50+difficulty),PWMS12_CHAN1);
  PWMS12_SetPeriod(20510,PWMS12_GRP0); //250 Hz.
}



//starts motors pushing the pillars down
void StartMotorDown(unsigned int difficulty)
{
  PTT |= BIT5HI; //enables L293NE
  //N.B. at a duty cycle of 50, the motor will not move, since in the H-bridge configuration, low corresponds 
  //to moving in one direction, and high in another.  Therefore, we modify "difficulty" to reflect this fact.  
  //Furthermore, we can exploit this phenomenon in startmotor up and start motor down.  Lastly, motor 1 is wired 
  //opposite to motors 2,3 and so a duty cycle of 70 for all motors sets motors 2,3 moving in one direction and motor
  //1 moving in the opposite direction.    
  PWMS12_SetDuty((unsigned char)(50-difficulty/4),PWMS12_CHAN0);
  PWMS12_SetDuty((unsigned char)(50-difficulty/4),PWMS12_CHAN1);
  PWMS12_SetPeriod(20510,PWMS12_GRP0); //250 Hz.
}




//increases motor speed as a result of the user placing a cube incorrectly 
unsigned int IncreaseMotorSpeed(int difficulty)
{
  if ((difficulty + 20) > 50)
  {
      difficulty = 50;
  }else
  {
      difficulty = difficulty + 20;
      printf("difficulty %d", difficulty);
  }
  PWMS12_SetDuty((unsigned char)(50-difficulty),PWMS12_CHAN0);
  PWMS12_SetPeriod(20510,PWMS12_GRP0); 
  
  return difficulty;
}




//turns off all cube LEDs and panel LEDs
void ClearAllLEDs(void)
{
   int j; 
   int k;
   
   for (j = 0; j < 16; j++)
   {
     PTM &= BIT2LO;  //actually turns off all LEDs in front panel
     PulseSCKU4U5(); 
   }    
   PulseRCKU4U5();
      
   for (k = 0; k < NO_OF_CUBES; k++)
   {
     PTM &= BIT1LO; //turns off all the cubes
     PulseSCKU1(); 
   }
   PulseRCKU1();
}




 //Pulses the shift clock on U4/U5. PTM4
 void PulseSCKU4U5(void)
 {
   int i;
   PTM &= BIT5LO;
   for (i = 0; i < 1; i++) 
   {
      volatile int dummy;
   }
   
   PTM |= BIT5HI;
   
   return;
   
 }
 
 
 
 
 //Pulses the shift clock on U1. PTT0
 void PulseSCKU1(void)
 {
   int i;
   PTM &= BIT5LO;
   for (i = 0; i < 1; i++) 
   {
      volatile int dummy;
   }
   
   PTM |= BIT5HI;
   
   return;
   
 }





 //Pulses the register clock on U4/U5. PTM3
 void PulseRCKU4U5(void)
 {
   int i;
   PTM &= BIT3LO;
   for (i = 0; i < 1; i++) 
   {
      volatile int dummy;
   }
   PTM |= BIT3HI;
   
   return;
   
 }
 
 
 
 
 //Pulses the register clock on U1. PTM0
 void PulseRCKU1(void)
 {
   int i;
   PTM &= BIT0LO;
   for (i = 0; i < 1; i++) 
   {
      volatile int dummy;
   }
   PTM |= BIT0HI;
   
   return;
   
   
 }




//activates the buzzer under the pillar of the cube that was placed incorrectly for 0.5 seconds.  
 void ActivateBuzzer(uint16_t param)
 {
   uint16_t t = EF_Timer_GetTime();
   puts("inbuzzer");
   printf("param %d \n\r", param);
   switch(param)
   {  
      case 0:
         PTAD |= BIT4HI;
         while (EF_Timer_GetTime() - t < 500)
         {
            //do nothing
         }
         PTAD &= BIT4LO;
      break;
         
      case 1:
         PTAD |= BIT5HI;
         while (EF_Timer_GetTime() - t < 500)
         {
            //do nothing
         }
         PTAD &= BIT5LO;
      break;
         
      case 2:
         PTAD |= BIT6HI;
         while (EF_Timer_GetTime() - t < 500)
         {
            //do nothing
         }
         PTAD &= BIT6LO;
      break;
         
      case 3:
         PTAD |= BIT7HI;
         while (EF_Timer_GetTime() - t < 500)
         {
            //do nothing
         }
         PTAD &= BIT7LO;
      break;   
         
      case 4:
         PTM |= BIT4HI;
         while (EF_Timer_GetTime() - t < 500)
         {
            //do nothing
         }
         PTM &= BIT4LO;
      break;
   }
 }
 
 
 
 
 //turns all cubes off sequentially, and then back on sequentially to indicate a win.
 void YouWinLEDMessage(void)
 {
   char i;
   char j;
  
   uint16_t startTime;
   
   unsigned int cubesoff = ~0x1;
   unsigned int maskoff = ~0x1;
   
   unsigned int cubeson = 0x1;
   unsigned int maskon = 0x1;
   
   puts("in win thing");
   
   for(j = 0; j < NO_OF_CUBES; j++)
   {      
      cubesoff = cubesoff & maskoff;
      for (i = 0; i < NO_OF_CUBES; i++)
      {
         if ((cubesoff>>i)&1 == 1)//finds the ith bit of cubes off
         {
            PTM |= BIT1HI;
         }else
         {
            PTM &= BIT1LO;
         }
         PulseSCKU1();
      }
      PulseRCKU1();
      startTime=EF_Timer_GetTime();//wait for 500 milliseconds and therefore 0.5 sec
      while(EF_Timer_GetTime()<(startTime+500));
      cubesoff = cubesoff << 1;
   }
   
   for(j = 0; j < NO_OF_CUBES; j++)
   {      
      cubeson = cubeson | maskon;
      for (i = 0; i < NO_OF_CUBES; i++)
      {
         if ((cubeson>>i)&1 == 1)
         {
            PTM |= BIT1HI;
         }else
         {
            PTM &= BIT1LO;
         }
         PulseSCKU1();
      }
      PulseRCKU1();
      startTime=EF_Timer_GetTime();//wait for 500 milliseconds and therefore 0.5 sec
      while(EF_Timer_GetTime()<(startTime+500));
      cubeson = cubeson << 1;
   }
 }
 
 
 

//brings all 5 pillars down to about the halfway point, then pushes the outer pillars and inner pillar
//up and down in an alternating fashion for 2 complete loops. 
 void AlternatePillars(void)
 {
   uint16_t startTime;
   int i;

   PTT |= BIT5HI; //enables LB293
   PTT &= BIT3LO; //turns on motor 1 --assume LO is in motor-down direction
   PTT &= BIT4LO; //turns on motors 2,3 -- assume LO is in motor-down direction
   
   startTime=EF_Timer_GetTime();//wait for 1000 milliseconds and therefore 1 sec
   while(EF_Timer_GetTime()<(startTime+1000));
   
   //splits the two groups of pillars first to set up for alternation.
   PTT |= BIT3HI; //turns on motor 1 --assume HI is in motor-up direction
   PTT &= BIT4LO; //turns on motors 2,3 -- assume LO is in motor-down direction
   
   startTime=EF_Timer_GetTime();//wait for 1000 milliseconds and therefore 1 sec
   while(EF_Timer_GetTime()<(startTime+1000)); //we use 1000 instead of 1000 to make sure no limits are exceeded

   for (i = 0; i<2; i++)                                                                         
   {
      PTT &= BIT3LO; //turns on motor 1 --assume LO is in motor-down direction
      PTT |= BIT4HI; //turns on motors 2,3 -- assume HI is in motor-up direction
   
      startTime=EF_Timer_GetTime();//wait for 2000 milliseconds and therefore 2 sec
      while(EF_Timer_GetTime()<(startTime+2000)); //we use 2000 instead of 2000 to make sure no limits are exceeded
   
      PTT |= BIT3HI; //turns on motor 1 --assume HI is in motor-up direction
      PTT &= BIT4LO; //turns on motors 2,3 -- assume LO is in motor-down direction
   
      startTime=EF_Timer_GetTime();//wait for 2000 milliseconds and therefore 2 sec
      while(EF_Timer_GetTime()<(startTime+2000)); //we use 2000 instead of 2000 to make sure no limits are exceeded
   }
   
   //brings the two groups of pillars back together so that they are level again.
   PTT &= BIT3LO; //turns on motor 1 --assume HI is in motor-up direction
   PTT |= BIT4HI; //turns on motors 2,3 -- assume LO is in motor-down direction
   
   startTime=EF_Timer_GetTime();//wait for 1000 milliseconds and therefore 1 sec
   while(EF_Timer_GetTime()<(startTime+1000)); //we use 1000 instead of 1000 to make sure no limits are exceeded

 }
 
 
 
 
 //stops all the motors from running at all.
 void StopAllMotors(void)
 {
   PTT &= BIT5LO; //disables LB293.
   return;
 }
 
 //compares the user's current answer with the computer's answer to check whether or not the user has won.
 boolean CompareAnswers(void)
 {
   int i;
   
   for (i = 0; i < NO_OF_CUBES; i++)
   {
      if (usersanswer[i] != computersanswer[i])
      {
         return False;
      }
   }
   return True;
 }
 
 
 