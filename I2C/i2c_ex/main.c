#case 
#include <16F876.h> 
#include "SFR16F876.h" 

#fuses XT, WDT, NOPROTECT, PUT, BROWNOUT, NOLVP 
#define CLOCKFREQUENCY    4000000 
#USE DELAY(CLOCK = CLOCKFREQUENCY) 


// used for timing stuff - counts up to 0xFF.  Initialized by main() 
volatile int8 Milliseconds; 

#include "i2c.c" 
#include "isr.c" 

// Note that yours will be different depending on your hardware 
#define  PORT_A_TRIS_MASK        0x0F   /* all inputs except RA4 & RA5      */ 
#define  PORT_B_TRIS_MASK        0x01   /* all outputs except RB0           */ 
#define  PORT_C_TRIS_MASK        0xD9   /* all inputs xcept bit 1 (SHIFTREG_CKL)        
                                           and bit 2 (SHIFTREG_LOAD)        
                                           and bit 5 (Pilot Enable/Reset)   */ 


/* ************************************************************************* 
  DESCRIPTION:  Handles our milisecond counters 
  RETURN: none 
  ALGORITHM:  none 
  NOTES:  none 
 *************************************************************************** */ 
void System_Tick(void) 
{ 
  static int1 LED_State = 0; 
  static int16 count=0; 

  while (Milliseconds) 
  { 
    I2C_Timers(); 
    --Milliseconds; 

// ------------------------------------------------------------------------ 
// This is just a dirty little test to blink an LED on one of my boards 
// You wouldn't have anything between these comment lines nor the vars 
// declared in the begining 
    count++; 
    if (count == 1000) 
    { 
      if (I2C_Tx_Msg.hdr.status == NO_MSG) 
      { 
        if (LED_State) 
          Blink_LED(0x6E,1); 
        else 
          Blink_LED(0x6E,0); 
        LED_State = !LED_State; 
      } 
      count = 0; 
    }    
// ------------------------------------------------------------------------ 
  } 
  return; 
} 

/**************************************************************************** 
  DESCRIPTION: This is the "main" function of the input card software.  It 
               performs the proper initialization, then starts the main loop 
               of the program.                        
  RETURN:      none 
  ALGORITHM:   none 
  NOTES:       none 
*****************************************************************************/ 
void main(void) 
{ 
  // Initialize the ports 
  set_tris_a(PORT_A_TRIS_MASK); 
  set_tris_b(PORT_B_TRIS_MASK); 
  set_tris_c(PORT_C_TRIS_MASK); 
  
  // Setup timer2 to int every milisecond.  This is our system tick 
  setup_timer_2(T2_DIV_BY_4, 0xFA, 1); 
  enable_interrupts(INT_TIMER2); 

  enable_interrupts(GLOBAL); 

  // Start our time from now 
  Milliseconds = 0; 

  // Setup the I2C module 
  I2C_Initialize(); 
  I2C_Reset_Hardware(); 

  // Main loop 
  while (TRUE) 
  { 
    restart_wdt(); 
    System_Tick(); 
    // Handle any of our tasks 
    I2C_Tasks(); 

  } // End of forever loop 
} 
