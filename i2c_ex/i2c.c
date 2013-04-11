
/*$F*************************************************************************** 
* 
* Copyright (C) 2004 Mark Norton and Steve Karg 
* 
* Permission is hereby granted, free of charge, to any person obtaining 
* a copy of this software and associated documentation files (the 
* "Software"), to deal in the Software without restriction, including 
* without limitation the rights to use, copy, modify, merge, publish, 
* distribute, sublicense, and/or sell copies of the Software, and to 
* permit persons to whom the Software is furnished to do so, subject to 
* the following conditions: 
* 
* The above copyright notice and this permission notice shall be included 
* in all copies or substantial portions of the Software. 
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
* 
* Functional 
* Description:  Defines the ACCESS.bus implementation for the Microchip 
*               microprocessor 
* 
* $Log:  $ 
* 
*********************************************************************/ 
// Define this to use the hardware MSSP I2C module 
//#define USE_HARDWARE 

//------------------- Commands Go Here -------------------------------------- 
#define CMD_BLINK_LED            0x01 

//---------------- Define your address here --------------------------------- 
#define  MY_I2C_ADDR             0x6E 


//------------------- Synchronous Serial Port I2C --------------------------- 
#ifdef USE_HARDWARE 
  #define  I2C_MASTER_MASK       0x38    // SSPCON.SSPM3-0 = 1000 -- 
                                         // Master mode, slave idle 
#else 
  #define  I2C_MASTER_MASK       0x0B    // SSPCON.SSPM3-0 = 1011 -- 
                                         // firmware master mode, slave idle 
#endif 

#define  I2C_SLAVE_MASK         0x0E    // SSPCON.SSPM3-0 = 1110 -- 
                                         // slave mode, 7-bit addr, S & P 
                                         // interrupts enabled 
// This controls the clock speed 
#define  I2C_BAUD_100K           100000 
#define  I2C_BAUD_400K           400000 

#define  I2C_BAUD_VALUE          ((CLOCKFREQUENCY/2)/I2C_BAUD_100K) 
#define  I2C_CLK_ENABLE          0x10 
#define  I2C_MODE_SETUP          0x20 


//---------------------- Message flags ------------------------------------ 
#define MSGACK                   0xA5 
#define MSGNACK                  0xFF 
#define MAX_TRIES                10     // max number to attempt tx message 

//--------------------- Message Status -------------------------------------- 
enum MSG_STATUS 
{ 
  NO_MSG, 
  MSG_READY, 
  MSG_ERROR, 
  CHECKSUM_OK 
}; 


// For the PIC18's we will use the LAT registers 
#if defined(__PCH__) 
  #define I2C_CLK_LATCH         LATCbits.LATC3 
  #define I2C_DATA_LATCH        LATCbits.LATC4 
#else 
  #define I2C_CLK_LATCH         PORTCbits.RC3 
  #define I2C_DATA_LATCH        PORTCbits.RC4 
#endif 

#define I2C_CLK                 PORTCbits.RC3 
#define I2C_DATA                PORTCbits.RC4 
#define I2C_CLK_HI_Z            TRISCbits.TRISC3 
#define I2C_SDA_HI_Z            TRISCbits.TRISC4 


// Used to define the amount of data for largest command 
#define MAX_MSG_LEN             10 

union I2C_MSG 
{ 
  struct 
  { 
    enum MSG_STATUS status;   // Status 
    int8 dest;                // Message destination 
    int8 src;                 // Message source 
    int8 len;                 // Message length 
    int8 opcode;              // Message command 
    int8 data[MAX_MSG_LEN];   // Message data 
  } hdr; 
  int8 buf[MAX_MSG_LEN + 5]; 
}; 



//--------------------- I2C recieve states ----------------------------------- 
enum I2C_RX_STATES 
{ 
  I2C_IDLE, 
  I2C_RxING, 
  I2C_SLAVE_TxING, 
  I2C_TxING, 
  I2C_WAITING 
}; 

enum I2CRESULTS 
{ 
  BYTE_NACKED, 
  BYTE_ACKED, 
  LOST_ARBITRATION, 
  NO_RESPONSE, 
  MSG_TRANSMITTED 
}; 

typedef enum I2CRESULTS I2C_RESULTS; 


//----------------------------- globals ------------------------------------- 
// I2C recieved buffer 
union I2C_MSG I2C_Rx_Msg; 
// I2C transmit buffer 
union I2C_MSG I2C_Tx_Msg; 
// Status of I2C bus 
enum I2C_RX_STATES I2C_State = I2C_IDLE; 
// wait time before sending a message 
int8 I2C_Wait_To_Send; 



//----------------------------- locals ------------------------------------- 
// this is our current i2c address 
static int8 I2C_Address = MY_I2C_ADDR; 
// used to timeout receiving a message 
static int8 I2C_Bus_Timeout; 


/**************************************************************************** 
* DESCRIPTION: Send a byte across the i2c bus. 
* RETURN:      Three possible states (ACKed, [spam], Lost Arbitratiom) 
* ALGORITHM:   none 
* NOTES:       none 
*****************************************************************************/ 
static I2C_RESULTS I2C_Write( 
  int8 data) // byte sent over the I2C 
{ 
#ifdef USE_HARDWARE 
  SSPBUF = (data); 
  while (SSPSTATbits.BF) 
  { 
    #asm 
    nop 
    #endasm 
  } 

  // Wait until the bus is idle 
  while ((SSPCON2 & 0x1F) || (SSPSTATbits.R_W)); 

  if (PIR2bits.BCLIF)           // test for bus collision 
    return (LOST_ARBITRATION);          // return with Bus Collision error 

  if ( !SSPCON2bits.ACKSTAT )   // test for ACK condition received 
    return(BYTE_ACKED); 
  else 
    return(BYTE_NACKED); 

#else 
  I2C_RESULTS value;       // Value to be returned 
  int8 i;                  // Index to counter 
  int8 loop_count;         // Clock stretch loop counter 

  I2C_CLK_LATCH=0; 
  I2C_DATA_LATCH=0; 
  for (i=0; i<8; i++) 
  { 
    delay_us(5); 
    I2C_CLK_HI_Z=0; 
    if (data & 0x80) 
    { 
      I2C_SDA_HI_Z=1; 
      delay_us(3); 
      if (!I2C_DATA) 
      { 
        I2C_CLK_HI_Z=1; 
        return (LOST_ARBITRATION); 
      } 
    } 
    else 
    { 
      I2C_SDA_HI_Z=0; 
      delay_us(3); 
    } 
    delay_us(5); 
    I2C_CLK_HI_Z=1; 
    //  Allow the clock to be stretched 
    loop_count = 255; 
    while (loop_count > 0) 
    { 
      if (I2C_CLK) 
        break; 
      --loop_count; 
      delay_us(1); 
    } 
    data <<= 1; 
  } 
  delay_us(6); 
  I2C_CLK_HI_Z=0; 
  delay_us(1); 
  I2C_SDA_HI_Z=0; 
  delay_us(1); 
  I2C_SDA_HI_Z=1; 
  delay_us(4); 
  I2C_CLK_HI_Z=1; 
  //  Allow the clock to be stretched 
  loop_count = 255; 
  while (loop_count > 0) 
  { 
    if (I2C_CLK) 
      break; 
    --loop_count; 
    delay_us(1); 
  } 
  if (I2C_DATA) 
    value = BYTE_NACKED; 
  else 
    value = BYTE_ACKED; 
  delay_us(5); 
  I2C_CLK_HI_Z=0; 
  delay_us(1); 
  I2C_SDA_HI_Z=0; 
  return (value); 
#endif 
} 

/**************************************************************************** 
* DESCRIPTION: Sends a START condition over the i2c bus 
* RETURN:      Three possible states (ACKed, [spam], Lost Arbitratiom) 
* ALGORITHM:   none 
* NOTES:       none 
*****************************************************************************/ 
static I2C_RESULTS I2C_Start(void) 
{ 
#ifdef USE_HARDWARE 
  // Wait until the bus is idle 
  while ((SSPCON2 & 0x1F) || (SSPSTATbits.R_W)); 

  SSPCON2bits.SEN = 1; 
  while (SSPCON2bits.SEN) 
  { 
    #asm 
    nop 
    #endasm 
  } 
  if (PIR2bits.BCLIF)           // test for bus collision 
  { 
    return (LOST_ARBITRATION);          // return with Bus Collision error 
  } 
  return (BYTE_ACKED); 
#else 
  int8 loop_count;    // Clock stretch loop counter 

  // Set both lines to inputs 
  I2C_SDA_HI_Z=1; 
  delay_us(1); 
  I2C_CLK_HI_Z=1; 

  // Check for a low condition on the data line.  If it is low then someone 
  // else is already communicating so exit 
  if (!I2C_DATA) 
  { 
    return (LOST_ARBITRATION); 
  } 

  //  Allow the clock to be stretched 
  loop_count = 255; 
  while (loop_count > 0) 
  { 
    if (I2C_CLK) 
      break; 
    --loop_count; 
    delay_us(1); 
  } 
  // If loop count is 0 then the clock was held low for more than 255us 
  // so exit with an arbitration error 
  if (loop_count == 0) 
  { 
    return (LOST_ARBITRATION); 
  } 

  I2C_SDA_HI_Z=0; 
  I2C_DATA_LATCH=0; 
  delay_us(2); 
  I2C_CLK_HI_Z=0; 
  I2C_CLK_LATCH=0; 

  return (BYTE_ACKED); 
#endif 
} 

/**************************************************************************** 
* DESCRIPTION: Sends a START condition over the I2C 
* RETURN:      Three possible states (ACKed, [spam], Lost Arbitratiom) 
* ALGORITHM:   none 
* NOTES:       none 
*****************************************************************************/ 
static I2C_RESULTS I2C_Restart(void) 
{ 
#ifdef USE_HARDWARE 
  // Wait until the bus is idle 
  while ((SSPCON2 & 0x1F) || (SSPSTATbits.R_W)); 

  SSPCON2bits.RSEN = 1; 
  while (SSPCON2bits.RSEN) 
  { 
    #asm nop #endasm 
  } 
  if (PIR2bits.BCLIF)           // test for bus collision 
  { 
    return (LOST_ARBITRATION);          // return with Bus Collision error 
  } 
  return (BYTE_ACKED); 
#else 
  return(I2C_Start()); 
#endif 
} 

/**************************************************************************** 
* DESCRIPTION: Sends a STOP condition over the I2C 
* RETURN:      none 
* ALGORITHM:   none 
* NOTES:       none 
*****************************************************************************/ 
static void I2C_Stop(void) 
{ 
#ifdef USE_HARDWARE 
  // Wait until the bus is idle 
  while ((SSPCON2 & 0x1F) || (SSPSTATbits.R_W)); 

  SSPCON2bits.PEN = 1; 
  while (SSPCON2bits.PEN) 
  { 
    #asm nop #endasm 
  } 
#else 
  int8 loop_count;    // Clock stretch loop counter 

  I2C_CLK_HI_Z=0; 
  I2C_CLK_LATCH=0; 
  delay_us(1); 
  I2C_SDA_HI_Z=0; 
  I2C_DATA_LATCH=0; 

  delay_us(4); 
  I2C_CLK_HI_Z=1; 
  loop_count = 255; 
  //  Allow the clock to be stretched 
  while (loop_count > 0) 
  { 
    delay_us(1); 
    if (I2C_CLK) 
      break; 
    --loop_count; 
  } 
  I2C_SDA_HI_Z=1; 
#endif 
} 

/**************************************************************************** 
* DESCRIPTION: Reads a byte from the ACCESS.bus i2c. 
* RETURN:      byte of data from the ACCESS.bus 
* ALGORITHM:   none 
* NOTES:       bit bang the i2c 
*****************************************************************************/ 
static int8 I2C_Read( 
  int8 read_ack) // TRUE if we should ACK the byte 
{ 
#ifdef USE_HARDWARE 

  SSPCON2bits.RCEN = 1; 
  while (SSPCON2bits.RCEN) 
  { 
    #asm nop #endasm 
  } 
  SSPCON2bits.ACKDT = read_ack; 
  SSPCON2bits.ACKEN = 1; 
  while (SSPCON2bits.ACKEN) 
  { 
    #asm nop #endasm 
  } 
  return(SSPBUF); 
#else 
  int8 value=0;       // Value to be returned 
  int8 i;             // Index to counter 
  int8 loop_count;    // Clock stretch loop counter 

  I2C_SDA_HI_Z=1; 
  delay_us(1); 
  I2C_SDA_HI_Z=1; 
  I2C_DATA_LATCH=0; 
  I2C_CLK_LATCH=0; 
  for (i=0; i<8; i++) 
  { 
    delay_us(10); 
    I2C_CLK_HI_Z=1; 
    value <<= 1; 
    loop_count = 255; 
    while (loop_count > 0) 
    { 
      if (I2C_CLK) 
        break; 
      --loop_count; 
      delay_us(1); 
    } 
    delay_us(5); 
    if (I2C_DATA) 
    { 
      value |= 0x01; 
    } 
    else 
    { 
      value &= ~0x01; 
    } 
    delay_us(8); 
    I2C_CLK_HI_Z=0; 
  } 
  delay_us(4); 
  if (read_ack > 0) 
  { 
    I2C_SDA_HI_Z=0; 
  } 
  delay_us(5); 
  I2C_CLK_HI_Z=1; 
  delay_us(10); 
  I2C_CLK_HI_Z=0; 
  delay_us(1); 
  I2C_SDA_HI_Z=0; 

  return (value); 
#endif 
} 

/**************************************************************************** 
* DESCRIPTION: Clears the SSPBUF register 
* RETURN:      none 
* ALGORITHM:   none 
* NOTES:       none 
*****************************************************************************/ 
#inline 
void I2C_Clear_SSPBUF(void) 
{ 
  int8 dummy; 

  dummy = SSPBUF; 
  (void)dummy; 

  return; 
} 

/**************************************************************************** 
* DESCRIPTION: Resets the ACCESS.bus hardware to a known state 
* RETURN:      none 
* ALGORITHM:   none 
* NOTES:       none 
*****************************************************************************/ 
void I2C_Reset_Hardware(void) 
{ 
  // Reset the I2C if we timeout, this should prevent us from ever 
  // locking up the I2C 
  I2C_CLK_HI_Z=1; 
  I2C_SDA_HI_Z=1; 
  SSPADD = I2C_Address; 
  SSPCON = (I2C_SLAVE_MASK); 
  SSPCON = (I2C_SLAVE_MASK | I2C_CLK_ENABLE | I2C_MODE_SETUP); 
  // Clear the buffer 
  I2C_Clear_SSPBUF(); 
  SSPCONbits.SSPOV = 0; 
  I2C_State = I2C_IDLE; 
  clear_interrupt(INT_SSP); 
  enable_interrupts(INT_SSP); 

  return; 
} 

/**************************************************************************** 
* DESCRIPTION: Sets our ACESS.bus address 
* RETURN:      none 
* ALGORITHM:   none 
* NOTES:       none 
*****************************************************************************/ 
void I2C_Set_Address( 
  int8 address) // our new address 
{ 
  SSPADD = I2C_Address = address; 
} 

/**************************************************************************** 
* DESCRIPTION: Initialize our module level variables 
* RETURN:      none 
* ALGORITHM:   none 
* NOTES:       none 
*****************************************************************************/ 
void I2C_Initialize(void) 
{ 
  int8 temp_intcon; 

  // Save the current interrupt state 
  temp_intcon = INTCON; 
  // Make sure that the interrupts are disabled 
  disable_interrupts(GLOBAL); 

  I2C_Set_Address(MY_I2C_ADDR); 
  I2C_Rx_Msg.hdr.status = NO_MSG; 
  I2C_Tx_Msg.hdr.status = NO_MSG; 
  I2C_Wait_To_Send = 0; 
  I2C_State = I2C_IDLE; 
  // Restore the previous interrupt state 
  INTCON = temp_intcon; 
} 

/**************************************************************************** 
* DESCRIPTION: Allows our I2C timers to increment/decrement 
* RETURN:      none 
* ALGORITHM:   none 
* NOTES:       called every mS 
*****************************************************************************/ 
void I2C_Timers(void) 
{ 
  if (I2C_Bus_Timeout) 
    --I2C_Bus_Timeout; 
  // haven't received a byte in a long time 
  else if (I2C_State > I2C_RxING) 
  { 
    // reset the bus if the buffer is full 
    if (SSPSTATbits.BF) 
      I2C_Reset_Hardware(); 
    I2C_State = I2C_IDLE; 
  } 
} 

/**************************************************************************** 
* DESCRIPTION: Processes the messages received on the I2C 
* RETURN:      none 
* ALGORITHM:   none 
* NOTES:       none 
*****************************************************************************/ 
void I2C_Process_Message( 
  union I2C_MSG* buf, 
  int8 len) 
{ 
  // not ready to process yet 
  if (buf->hdr.status != MSG_READY) 
    return; 

  // Disable access bus interrupt since we are processing a message 
  disable_interrupts(INT_SSP); 

  // Go ahead and release the buffer.  We won't receive anything yet 
  // since we disabled the interrupt above 
  buf->hdr.status = NO_MSG; 


  // ------------------------- Add your commands here ----------------------- 
  switch (buf->hdr.opcode) 
  { 
    // Test command for blinking an LED 
    case CMD_BLINK_LED: 
      { 
        if (buf->buf[5] == 0) 
          PORTAbits.RA4 = 0; 
        else 
          PORTAbits.RA4 = 1; 
      } 
      break; 
    default: 
      break; 
  } 
  // Turn back on the access bus interrupt 
  clear_interrupt(INT_SSP); 
  enable_interrupts(INT_SSP); 

  return; 
} 

/**************************************************************************** 
* DESCRIPTION: Handles the bus reception. 
* RETURN:      none 
* ALGORITHM:   none 
* NOTES:       none 
*****************************************************************************/ 
#inline 
void I2C_Rx_Byte(void) 
{ 
  static int8 rx_count = 0;             // num of bytes received 
  static signed int8 rx_msg_len = 0;    // msg length 
  static int8 checksum = 0;             // message checksum (XOR) 
  static int8 data = 0;                 // data read from SSPBUF 

  // Check for a stop bit 
  if (SSPSTATbits.P) 
  { 
    I2C_State = I2C_IDLE; 
    // inter-message wait time in ms 
    I2C_Wait_To_Send = 5; 
    I2C_Bus_Timeout = 0; 
  } 
  else 
  { 
    // were we addressed in read mode 
    if (SSPSTATbits.R_W) 
    { 
      if (I2C_Rx_Msg.hdr.status == CHECKSUM_OK) 
      { 
        I2C_Rx_Msg.hdr.status = MSG_READY; 
        // Load acknowledgement value 
        SSPBUF = MSGACK; 
      } 
      else 
      { 
        SSPBUF = MSGNACK; 
      } 
      // Setup to allow data to be read from us 
      SSPCONbits.CKP=1; 
      I2C_State = I2C_SLAVE_TxING; 
      I2C_Bus_Timeout = 25; 
    } 
    // See if we were addressed 
    // The following line was modified because of errata with 
    // the PIC18CXX2 clearing the BF bit when the BSR is pointed to 0x0F 
    // and an instruction contains 0xC9 in its 8 least significant bits 
//    else if (SSPSTATbits.BF) 
    else if ((SSPSTATbits.BF) || (I2C_State == I2C_RxING)) 
    { 
      // continue recieving bytes 
      // process the first byte 
      if (!SSPSTATbits.D_A) 
      { 
        // Read the data 
        data = SSPBUF; 
        I2C_State = I2C_RxING; 
        I2C_Rx_Msg.hdr.status = NO_MSG; 
        I2C_Rx_Msg.hdr.dest = data; 
        rx_msg_len = 2; 
        rx_count = 1; 
        checksum = data; 
      } 
      else 
      { 
        // Read the data 
        data = SSPBUF; 
        ++rx_count;                             // keep track of how many bytes 
        if (rx_count == 3)                      // test for length of msg byte 
          rx_msg_len = data;                    // set length counter 
        else 
          --rx_msg_len; 
        if (rx_msg_len == -1)                   // check for end of msg 
        { 
          I2C_State = I2C_WAITING; 
          if ((checksum ^ data) == 0) 
            I2C_Rx_Msg.hdr.status = CHECKSUM_OK; 
          else 
            I2C_Rx_Msg.hdr.status = NO_MSG; 
        } 
        checksum ^= data; 
        if (rx_count < sizeof(I2C_Rx_Msg.buf)) 
          I2C_Rx_Msg.buf[rx_count] = data; 
      } 
      I2C_Bus_Timeout = 25; 
    } 
  } 
} 

/**************************************************************************** 
* DESCRIPTION: Transmits messages out the I2C port 
* RETURN:      FALSE on failure    TRUE on success 
* ALGORITHM:   none 
* NOTES:       none 
*****************************************************************************/ 
int8 I2C_Send_Msg( 
  int8* buf,       // buffer containing the msg to send 
  int8 len)        // length of the buffer 
{ 
  int8 msg_count       = 0;           // number of times tx attempted 
  int8 checksum        = 0;           // XOR of each byte in the msg 
  int8 length          = 0;           // length of msg being sent 
  int8 i               = 0;           // index to counter 
  int8 readdata        = 0;           // data read from destination 
  int8 device_present  = FALSE;       // determines if the rxing device is there 
  I2C_RESULTS result   = BYTE_ACKED;  // return result from our I2C functions 

  // The length of the message is the 4th byte 
  length = buf[3]; 

  // Make sure the buffer is big enough.  The true length is 4 more bytes 
  // than the msg length 
  if (len < (length + 4)) 
  { 
    buf[0] = NO_MSG; 
    return(FALSE); 
  } 

  // Wait until this timer expires, this could be due to receiving bytes 
  // or a delay between messages 
  while (TRUE) 
  { 
    restart_wdt(); 
    // Check to see if we are receiving a message.  If so, then exit 
    if (I2C_State != I2C_IDLE) 
      return(FALSE); 

    // See if we are still waiting 
    if (!I2C_Wait_To_Send) 
      break; 
  } 

  // limit the number of times to try and send the message 
  for (msg_count = 0; msg_count < MAX_TRIES; msg_count++) 
  { 
    if (result == LOST_ARBITRATION) 
      return(FALSE); 

    // The last action was not a stop condition so either the hardware 
    // was reset or there was some sort of error.  We will check to see 
    // if the bus is free. 
    if (!SSPSTATbits.P) 
    { 
      restart_wdt(); 
      if (I2C_State != I2C_IDLE) 
        return(FALSE); 

      // We will monitor the SCL line for 2 ms to see if it goes low. 
      // if it does, then someone is talking so exit. 
      I2C_Wait_To_Send = 2; 
      while (I2C_Wait_To_Send) 
      { 
        if (!I2C_CLK) 
          return(FALSE); 
      } 
    } 
    I2C_State = I2C_TxING; 
    SSPCON = (I2C_MASTER_MASK | I2C_CLK_ENABLE); 
    SSPADD = I2C_BAUD_VALUE; 
    // Lets make sure the Interrupt flag for Bus collision is clear 
    clear_interrupt(INT_BUSCOL); 
    disable_interrupts(INT_SSP); 

    buf[0] = NO_MSG; 
    checksum = 0; 

    // The true message length is 4 bytes longer 
    for (i = 1; i < (length + 4); i++) 
    { 
      if (i==1) 
      { 
        result = I2C_Start(); 
        if (result != BYTE_ACKED) 
          break; 
      } 
      checksum ^= buf[i]; 
      // If the receiving device fails to acknowledge a byte then clear 
      // the var i to signal that there was a problem 
      result = I2C_Write(buf[i]); 
      delay_us(50); 
      if (result != BYTE_ACKED) 
        break; 
      device_present = TRUE; 
      if (i==(length + 3)) 
      { 
        result = I2C_Write(checksum); 
        if (result != BYTE_ACKED) 
          break; 
        // Start of the message received acknowledgement 
        result = I2C_Restart(); 
        if (result != BYTE_ACKED) 
          break; 
        result = I2C_Write(buf[1] | 0x01); 
        if (result != BYTE_ACKED) 
          break; 
        readdata = I2C_Read(0); 
      } 
    } 
    if (result != LOST_ARBITRATION) 
      I2C_Stop(); 
    I2C_Reset_Hardware(); 
    if ((result == BYTE_ACKED) && (readdata == MSGACK)) 
    { 
      buf[0] = NO_MSG; 
      return(TRUE); 
    } 
  } 
  // if the other device never ACKed the message, then its probably not there so 
  // lets consider the message sent. 
  if (!device_present) 
  { 
    buf[0] = NO_MSG; 
    return(TRUE); 
  } 

  return(FALSE); 
} 

/**************************************************************************** 
* DESCRIPTION: Allows our I2C tasks to run 
* RETURN:      none 
* ALGORITHM:   none 
* NOTES:       none 
*****************************************************************************/ 
void I2C_Tasks(void) 
{ 
  // See if we have received any messages 
  I2C_Process_Message(&I2C_Rx_Msg, sizeof(I2C_Rx_Msg.buf)); 

  // Check for an overflow condition 
  if (SSPCONbits.SSPOV) 
  { 
    // Overflow occured so reset the I2C 
    I2C_Reset_Hardware(); 
  } 
  // Transmit any messages 
  if (I2C_Tx_Msg.hdr.status != NO_MSG) 
    (void)I2C_Send_Msg(I2C_Tx_Msg.buf, sizeof(I2C_Tx_Msg.buf)); 
} 



/**************************************************************************** 
* DESCRIPTION: Sends the output level to address dest. 
* RETURN:      none 
* ALGORITHM:   none 
* NOTES:       none 
*****************************************************************************/ 
void Blink_LED( 
  int8 dest, // destination address for message 
  int8 level) // level that is being sent 
{ 



  I2C_Tx_Msg.hdr.status = MSG_READY; 
  I2C_Tx_Msg.hdr.dest = dest; 
  I2C_Tx_Msg.hdr.src = I2C_Address; 
  I2C_Tx_Msg.hdr.len = 2; 
  I2C_Tx_Msg.hdr.opcode = CMD_BLINK_LED; 
  I2C_Tx_Msg.hdr.data[0] = level;             /* Level to set the output */ 
  return; 
} 
