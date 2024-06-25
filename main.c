/*
 Final Project
 Team  11
 */

#include "gpt.h"
#include "gpt_private.h"
#include "Bit_Operations.h"
#include "Gpio.h"
#include "Rcc.h"
#include "Std_Types.h"

//.....................................................................................................................................................................................
#define   Door_Handle_Button         0                   //simulate the handle lock/unlock button
#define   Door_Unlock_Button         3                   //simulate the door lock/unlock

#define  Vehicle_lock_LED     0
#define  Hazard_LED              5
#define Ambient_light_LED   10

#define LOCKED   0
#define UNLOCKED   1

#define OPENED   0
#define CLOSED   1


uint8 DOOR_STATUS = LOCKED ;   // Initially the door is locked
uint8 DOOR = CLOSED;                     // Initially the door is closed

#define HALF_SEC_TICKS    (500)
#define SEC_TICKS                (1000)
#define TWO_SEC_TICKS    (2000)
#define TEN_SEC_TICKS     (10000)

uint8 Hazard_State = HIGH;

void Hazard_Blink(void);

//............................................................................................................................................................................................

int main() {

	// Enable clocks
	Rcc_Init();
	Rcc_Enable(RCC_GPIOA);    // Interrupts  Port
	Rcc_Enable(RCC_GPIOB);    // LEDS  Port

	// Configure push buttons as inputs with pull-up resistors
	Gpio_ConfigPin(GPIO_A, Door_Handle_Button, GPIO_INPUT, GPIO_PULL_UP);
	Gpio_ConfigPin(GPIO_A, Door_Unlock_Button, GPIO_INPUT, GPIO_PULL_UP);

	// Configure LEDS as outputs
	Gpio_ConfigPin(GPIO_B, Vehicle_lock_LED, GPIO_OUTPUT, GPIO_PUSH_PULL);
	Gpio_ConfigPin(GPIO_B, Hazard_LED, GPIO_OUTPUT, GPIO_PUSH_PULL);
	Gpio_ConfigPin(GPIO_B, Ambient_light_LED, GPIO_OUTPUT, GPIO_PUSH_PULL);

	 GPT_Init();

	while (1) {

		// ......................................................no Button is pressed Case......................................................................................................................
		if ((Gpio_ReadPin(GPIO_A , Door_Handle_Button) != 0) && (Gpio_ReadPin(GPIO_A , Door_Unlock_Button) != 0)){
			 //  Check if no Button is pressed
			 if ((Gpio_ReadPin(GPIO_A , Door_Handle_Button) != 0) && (Gpio_ReadPin(GPIO_A , Door_Unlock_Button) != 0)){

				 // ....................................................Default State Case....................................................

				 Gpio_WritePin(GPIO_B, Vehicle_lock_LED, LOW );          //Vehicle lock LED is off
				 Gpio_WritePin(GPIO_B, Ambient_light_LED, LOW );  // Ambient light LED is off


				 //............................................Anti theft vehicle lock Case ......................................
				 if ((DOOR_STATUS == UNLOCKED)  && ( DOOR == CLOSED)){

					 GPT_StartTimer(TEN_SEC_TICKS);            // Start timer for 10 seconds

					 while (!GPT_CheckTimeIsElapsed() );    //No buttons pressed for 10 seconds.

					 Hazard_Blink();
			    }

				 else{
					 Gpio_WritePin(GPIO_B, Hazard_LED, LOW);             // Hazard LED is off    //Default State Case
				 }

			   DOOR_STATUS = LOCKED;    // Vehicle door is locked
		}
	}

        // ..................................................Door Unlock Case..........................................................................................................................

		else if ((DOOR_STATUS == LOCKED) && ((Gpio_ReadPin(GPIO_A , Door_Handle_Button)) == 0)){
			if (Gpio_ReadPin(GPIO_A , Door_Handle_Button) == 0){        //  Check if Door_Handle_Button is pressed

				DOOR_STATUS = UNLOCKED ;      // Vehicle door is unlocked
				DOOR = CLOSED;    // but it is closed

				GPT_StartTimer(TWO_SEC_TICKS);            // Start timer for 2 seconds

				Gpio_WritePin(GPIO_B, Vehicle_lock_LED, HIGH );          //Vehicle lock LED is on.

				Gpio_WritePin(GPIO_B, Ambient_light_LED, HIGH );    // Ambient light LED is on for 2 seconds

				  Hazard_Blink();

				  // Check if  2 second has elapsed since the door was unlocked
			       if ( GPT_GetElapsedTime() >= TWO_SEC_TICKS - 100 ) {

			    	   Gpio_WritePin(GPIO_B, Ambient_light_LED, LOW );  // Ambient light LED is off after 2 seconds
			    }
			}
		}

		// .............................................................Door  is open Case ..........................................................................................................................

		else if ((DOOR_STATUS == UNLOCKED)  && ( DOOR == CLOSED) && ((Gpio_ReadPin(GPIO_A , Door_Unlock_Button)) == 0)){
			if (Gpio_ReadPin(GPIO_A , Door_Unlock_Button) == 0){        //  Check if Door_Unlock_Button is pressed

				DOOR_STATUS = UNLOCKED ;      // Vehicle door is unlocked
				DOOR = OPENED;    // Door is opened
				Gpio_WritePin(GPIO_B, Ambient_light_LED, HIGH );  // Ambient light LED is on
			}
		}

		//...........................................Closing the door Case......................................................................................................................................

		else if ((DOOR_STATUS == UNLOCKED)  && ( DOOR == OPENED) && (Gpio_ReadPin(GPIO_A , Door_Unlock_Button) == 0)){
			if (Gpio_ReadPin(GPIO_A , Door_Unlock_Button) == 0){        //  Check if Door_lock_Button is pressed

				DOOR_STATUS = UNLOCKED ;      // Vehicle door is unlocked
				DOOR = CLOSED;    // but it is closed

				Gpio_WritePin(GPIO_B, Vehicle_lock_LED, LOW );          //Vehicle lock LED is off
				Gpio_WritePin(GPIO_B, Hazard_LED, LOW );                  //Hazard LED is off

				GPT_StartTimer(SEC_TICKS );            // Start timer for 1 second

				while (GPT_GetRemainingTime() > 0 ) {
					Gpio_WritePin(GPIO_B, Ambient_light_LED, HIGH );  // Ambient light LED is on
				}

				  Gpio_WritePin(GPIO_B, Ambient_light_LED, LOW );  // Ambient light LED is off
			}
		}

		//........................................................Locking the door Case............................................................................................................................

		else if ((DOOR_STATUS == UNLOCKED)  && ( DOOR == CLOSED) && (Gpio_ReadPin(GPIO_A , Door_Handle_Button) == 0)){
					if (Gpio_ReadPin(GPIO_A , Door_Handle_Button) == 0){        //  Check if Door_Handle_Button is pressed

						DOOR_STATUS = LOCKED ;      // Vehicle door is locked
						DOOR = CLOSED;    // but it is closed

						Gpio_WritePin(GPIO_B, Vehicle_lock_LED, LOW );          //Vehicle lock LED is off
						Hazard_Blink();
						Gpio_WritePin(GPIO_B, Ambient_light_LED, LOW );  // Ambient light LED is off
					}
		}
	}

	return 0;
}

//...................................................................................................................................................................................................

void Hazard_Blink(void){                                          // Blink Hazard LED each 0.5 Sesond  for 2 seconds

	GPT_StartTimer(TWO_SEC_TICKS);            // Start timer for 2 seconds

	Gpio_WritePin(GPIO_B, Hazard_LED, Hazard_State );    // Hazard LED is on

	uint32 LastTime_Hazard_Blink  = GPT_GetElapsedTime();

	while ( GPT_GetElapsedTime() <= TWO_SEC_TICKS  ) {      //Blink 2 Times

		 uint32 current_time = GPT_GetElapsedTime();
		  if(current_time - LastTime_Hazard_Blink >= HALF_SEC_TICKS){              // Check if 0.5 seconds have elapsed
			  Hazard_State=!(Hazard_State);
			  Gpio_WritePin(GPIO_B, Hazard_LED, Hazard_State );   //Toggle Led
			  LastTime_Hazard_Blink = GPT_GetElapsedTime();
		  }

	 }
	Gpio_WritePin(GPIO_B, Hazard_LED, LOW);   //Off after 2 seconds (Blink 2 Times)
}

