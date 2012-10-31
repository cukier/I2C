/**************************************************************************** 
* NAME:        Interrupt_SSP 
* DESCRIPTION: Invoked whenever the dest address in an ABus 
*              AB message if one has occurred. 
* PARAMETERS:  none 
* RETURN:      none 
* ALGORITHM:   none 
* NOTES:       none 
*****************************************************************************/ 
#int_ssp 
static void Interrupt_SSP(void) 
{ 
  /* Make sure the buffer has not overflowed */ 
  if (SSPCONbits.SSPOV) 
  { 
    I2C_Reset_Hardware(); 
  } 
  else 
  { 
    I2C_Wait_To_Send = 5; 
    // don't recieve a byte if we are already processing a msg 
    if (I2C_Rx_Msg.hdr.status != MSG_READY) 
      I2C_Rx_Byte(); 
  } 

  return; 
} 


/**************************************************************************** 
* NAME:        Interrupt_Timer2 
* DESCRIPTION: Timer2 is set to go off every 1ms. 
* PARAMETERS:  none 
* RETURN:      none 
* ALGORITHM:   none 
* NOTES:       none 
*****************************************************************************/ 
#int_timer2 
static void Interrupt_Timer2(void) 
{ 

  // Global Milisecond timer 
  if (Milliseconds < 0xFF) 
    ++Milliseconds; 

  //----------------------------------------------------------------------------/ 
  //------- These are waitable timers so they must be done in the ISR ----------/ 
  //----------------------------------------------------------------------------/ 
  // Decrement timers 

  if (I2C_Wait_To_Send) 
    --I2C_Wait_To_Send; 
} 
