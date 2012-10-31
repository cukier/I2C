#ifndef SFR16F876_H 
#define SFR16F876_H 

#nolist 

unsigned char ADCON0; 
#locate ADCON0=0x1F 

struct { 
    unsigned char ADON:1; 
    unsigned char UNUSED0:1; 
    unsigned char GO_DONE:1; 
    unsigned char CHS0:1; 
    unsigned char CHS1:1; 
    unsigned char CHS2:1; 
    unsigned char ADCS0:1; 
    unsigned char ADCS1:1; 
} ADCON0bits; 
#locate ADCON0bits=0x1F 

unsigned char ADCON1; 
#locate ADCON1=0x9F 

struct { 
    unsigned char PCFG0:1; 
    unsigned char PCFG1:1; 
    unsigned char PCFG2:1; 
    unsigned char PCFG3:1; 
    unsigned char UNUSED:3; 
    unsigned char ADFM:1; 
} ADCON1bits ; 
#locate ADCON1bits=0x9F 

unsigned char ADRESH; 
#locate ADRESH=0x1E 

unsigned char ADRESL; 
#locate ADRESL=0x9E 

unsigned char CCP1CON; 
#locate CCP1CON=0x17 

struct { 
    unsigned char CCP1M0:1; 
    unsigned char CCP1M1:1; 
    unsigned char CCP1M2:1; 
    unsigned char CCP1M3:1; 
    unsigned char CCP1Y:1; 
    unsigned char CCP1X:1; 
} CCP1CONbits ; 
#locate CCP1CONbits=0x17 

unsigned char CCP2CON; 
#locate CCP2CON=0x1D 

struct { 
    unsigned char CCP2M0:1; 
    unsigned char CCP2M1:1; 
    unsigned char CCP2M2:1; 
    unsigned char CCP2M3:1; 
    unsigned char CCP2Y:1; 
    unsigned char CCP2X:1; 
} CCP2CONbits ; 
#locate CCP2CONbits=0x1D 

unsigned int16 CCPR1; 
#locate CCPR1=0x15 

unsigned char CCPR1H; 
#locate CCPR1H=0x16 

unsigned char CCPR1L; 
#locate CCPR1L=0x15 

unsigned int16 CCPR2; 
#locate CCPR2=0x1B 

unsigned char CCPR2H; 
#locate CCPR2H=0x1C 

unsigned char CCPR2L; 
#locate CCPR2L=0x1B 

unsigned char EEADR; 
#locate EEADR=0x10D 

unsigned char EECON1; 
#locate EECON1=0x18C 
struct { 
    unsigned char RD:1; 
    unsigned char WR:1; 
    unsigned char WREN:1; 
    unsigned char WRERR:1; 
    unsigned char UNUSED:3; 
    unsigned char EEPGD:1; 
} EECON1bits ; 
#locate EECON1bits=0x18C 

unsigned char EECON2; 
#locate EECON2=0x18D 

unsigned char EEDATA; 
#locate EEDATA=0x10C 

unsigned char EEDATH; 
#locate EEDATA=0x10E 

unsigned char EEADRH; 
#locate EEDATA=0x10F 

unsigned char FSR; 
#locate FSR=0x04 

unsigned char INDF; 
#locate INDF=0x00 

unsigned char INTCON; 
#locate INTCON=0x0B 

struct 
{ 
  unsigned char RBIF:1; 
  unsigned char INTF:1; 
  unsigned char T0IF:1; 
  unsigned char RBIE:1; 
  unsigned char INTE:1; 
  unsigned char T0IE:1; 
  unsigned char PEIE:1; 
  unsigned char GIE:1; 
} INTCONbits; 
#locate INTCONbits=0x0B 

unsigned char OPTION; 
#locate OPTION=0x81 

struct 
{ 
  unsigned char PS0:1; 
  unsigned char PS1:1; 
  unsigned char PS2:1; 
  unsigned char PSA:1; 
  unsigned char T0SE:1; 
  unsigned char T0CS:1; 
  unsigned char INTEDG:1; 
  unsigned char NOT_RBPU:1; 
} OPTIONbits; 
#locate OPTIONbits=0x81 

unsigned char PCL; 
#locate RCON=0x02 

unsigned char PCLATH; 
#locate RCON=0x0A 

unsigned char PCON; 
#locate RCON=0x8E 

struct 
{ 
  unsigned char NOT_BOR:1; 
  unsigned char NOT_POR:1; 
} PCONbits ; 
#locate PCONbits=0x8E 

unsigned char PIE1; 
#locate PIE1=0x8C 

struct { 
    unsigned char TMR1IE:1; 
    unsigned char TMR2IE:1; 
    unsigned char CCP1IE:1; 
    unsigned char SSPIE:1; 
    unsigned char TXIE:1; 
    unsigned char RCIE:1; 
    unsigned char ADIE:1; 
    unsigned char PSPIE:1; 
} PIE1bits ; 
#locate PIE1bits=0x8C 

unsigned char PIE2; 
#locate PIE2=0x8D 

struct { 
    unsigned char CCP2IE:1; 
    unsigned char UNUSED0:2; 
    unsigned char BCLIE:1; 
    unsigned char EEIE:1; 
} PIE2bits ; 
#locate PIE2bits=0x8D 

unsigned char PIR1; 
#locate PIR1=0x0C 

struct { 
    unsigned char TMR1IF:1; 
    unsigned char TMR2IF:1; 
    unsigned char CCP1IF:1; 
    unsigned char SSPIF:1; 
    unsigned char TXIF:1; 
    unsigned char RCIF:1; 
    unsigned char ADIF:1; 
    unsigned char PSPIF:1; 
} PIR1bits ; 
#locate PIR1bits=0x0C 

unsigned char PIR2; 
#locate PIR2=0x0D 

struct { 
    unsigned char CCP2IF:1; 
    unsigned char UNUSED0:2; 
    unsigned char BCLIF:1; 
    unsigned char EEIF:1; 
} PIR2bits ; 
#locate PIR2bits=0x0D 

unsigned char PORTA; 
#locate PORTA=0x05 

struct 
{ 
  unsigned char RA0:1; 
  unsigned char RA1:1; 
  unsigned char RA2:1; 
  unsigned char RA3:1; 
  unsigned char RA4:1; 
  unsigned char RA5:1; 
  unsigned char RA6:1; 
} PORTAbits ; 
#locate PORTAbits=0x05 

unsigned char PORTB; 
#locate PORTB=0x06 

struct 
{ 
  unsigned char RB0:1; 
  unsigned char RB1:1; 
  unsigned char RB2:1; 
  unsigned char RB3:1; 
  unsigned char RB4:1; 
  unsigned char RB5:1; 
  unsigned char RB6:1; 
  unsigned char RB7:1; 
} PORTBbits ; 
#locate PORTBbits=0x06 

unsigned char PORTC; 
#locate PORTC=0x07 

struct 
{ 
  unsigned char RC0:1; 
  unsigned char RC1:1; 
  unsigned char RC2:1; 
  unsigned char RC3:1; 
  unsigned char RC4:1; 
  unsigned char RC5:1; 
  unsigned char RC6:1; 
  unsigned char RC7:1; 
} PORTCbits ; 
#locate PORTCbits=0x07 

unsigned char PORTD; 
#locate PORTD=0x08 

struct 
{ 
  unsigned char RD0:1; 
  unsigned char RD1:1; 
  unsigned char RD2:1; 
  unsigned char RD3:1; 
  unsigned char RD4:1; 
  unsigned char RD5:1; 
  unsigned char RD6:1; 
  unsigned char RD7:1; 
} PORTDbits ; 
#locate PORTDbits=0x08 

unsigned char PORTE; 
#locate PORTE=0x09 

struct 
{ 
  unsigned char RE0:1; 
  unsigned char RE1:1; 
  unsigned char RE2:1; 
} PORTEbits ; 
#locate PORTEbits=0x09 

unsigned char PR2; 
#locate PR2=0x92 

unsigned char RCREG; 
#locate RCREG=0x1A 

unsigned char RCSTA; 
#locate RCSTA=0x18 

struct { 
    unsigned char RX9D:1; 
    unsigned char OERR:1; 
    unsigned char FERR:1; 
    unsigned char ADDEN:1; 
    unsigned char CREN:1; 
    unsigned char SREN:1; 
    unsigned char RX9:1; 
    unsigned char SPEN:1; 
} RCSTAbits ; 
#locate RCSTAbits=0x18 

unsigned char SPBRG; 
#locate SPBRG=0x99 

unsigned char SSPADD; 
#locate SSPADD=0x93 

unsigned char SSPBUF; 
#locate SSPBUF=0x13 

unsigned char SSPCON; 
#locate SSPCON=0x14 

struct { 
    unsigned char SSPM0:1; 
    unsigned char SSPM1:1; 
    unsigned char SSPM2:1; 
    unsigned char SSPM3:1; 
    unsigned char CKP:1; 
    unsigned char SSPEN:1; 
    unsigned char SSPOV:1; 
    unsigned char WCOL:1; 
} SSPCONbits ; 
#locate SSPCONbits=0x14 

unsigned char SSPCON2; 
#locate SSPCON2=0x91 

struct { 
    unsigned char SEN:1; 
    unsigned char RSEN:1; 
    unsigned char PEN:1; 
    unsigned char RCEN:1; 
    unsigned char ACKEN:1; 
    unsigned char ACKDT:1; 
    unsigned char ACKSTAT:1; 
    unsigned char GCEN:1; 
} SSPCON2bits ; 
#locate SSPCON2bits=0x91 

unsigned char SSPSTAT; 
#locate SSPSTAT=0x94 

struct { 
    unsigned char BF:1; 
    unsigned char UA:1; 
    unsigned char R_W:1; 
    unsigned char S:1; 
    unsigned char P:1; 
    unsigned char D_A:1; 
    unsigned char CKE:1; 
    unsigned char SMP:1; 
} SSPSTATbits ; 
#locate SSPSTATbits=0x94 

unsigned char STATUS; 
#locate STATUS=0x03 

struct { 
    unsigned char C:1; 
    unsigned char DC:1; 
    unsigned char Z:1; 
    unsigned char NOT_PD:1; 
    unsigned char NOT_TO:1; 
    unsigned char RP0:1; 
    unsigned char RP1:1; 
    unsigned char IRP:1; 
} STATUSbits ; 
#locate STATUSbits=0x03 

unsigned char T1CON; 
#locate T1CON=0x10 

struct 
{ 
  unsigned char TMR1ON:1; 
  unsigned char TMR1CS:1; 
  unsigned char NOT_T1SYNC:1; 
  unsigned char T1OSCEN:1; 
  unsigned char T1CKPS0:1; 
  unsigned char T1CKPS1:1; 
} T1CONbits ; 
#locate T1CONbits=0x10 

unsigned char T2CON; 
#locate T2CON=0x12 

struct { 
    unsigned char T2CKPS0:1; 
    unsigned char T2CKPS1:1; 
    unsigned char TMR2ON:1; 
    unsigned char TOUTPS0:1; 
    unsigned char TOUTPS1:1; 
    unsigned char TOUTPS2:1; 
    unsigned char TOUTPS3:1; 
} T2CONbits ; 
#locate T2CONbits=0x12 

unsigned char TMR0; 
#locate TMR0=0x01 

unsigned int16 TMR1; 
#locate TMR1=0x0E 

unsigned char TMR1H; 
#locate TMR1H=0x0F 

unsigned char TMR1L; 
#locate TMR1L=0x0E 

unsigned char TMR2; 
#locate TMR2=0x11 

unsigned char TRISA; 
#locate TRISA=0x85 

struct { 
    unsigned char TRISA0:1; 
    unsigned char TRISA1:1; 
    unsigned char TRISA2:1; 
    unsigned char TRISA3:1; 
    unsigned char TRISA4:1; 
    unsigned char TRISA5:1; 
} TRISAbits ; 
#locate TRISAbits=0x85 

unsigned char TRISB; 
#locate TRISB=0x86 

struct { 
    unsigned char TRISB0:1; 
    unsigned char TRISB1:1; 
    unsigned char TRISB2:1; 
    unsigned char TRISB3:1; 
    unsigned char TRISB4:1; 
    unsigned char TRISB5:1; 
    unsigned char TRISB6:1; 
    unsigned char TRISB7:1; 
} TRISBbits ; 
#locate TRISBbits=0x86 

unsigned char TRISC; 
#locate TRISC=0x87 

struct { 
    unsigned char TRISC0:1; 
    unsigned char TRISC1:1; 
    unsigned char TRISC2:1; 
    unsigned char TRISC3:1; 
    unsigned char TRISC4:1; 
    unsigned char TRISC5:1; 
    unsigned char TRISC6:1; 
    unsigned char TRISC7:1; 
} TRISCbits ; 
#locate TRISCbits=0x87 

unsigned char TRISD; 
#locate TRISD=0x88 

struct { 
    unsigned char TRISD0:1; 
    unsigned char TRISD1:1; 
    unsigned char TRISD2:1; 
    unsigned char TRISD3:1; 
    unsigned char TRISD4:1; 
    unsigned char TRISD5:1; 
    unsigned char TRISD6:1; 
    unsigned char TRISD7:1; 
} TRISDbits ; 
#locate TRISDbits=0x88 

unsigned char TRISE; 
#locate TRISE=0x89 

struct { 
    unsigned char TRISE0:1; 
    unsigned char TRISE1:1; 
    unsigned char TRISE2:1; 
    unsigned char UNUSED0:1; 
    unsigned char PSPMODE:1; 
    unsigned char IBOV:1; 
    unsigned char OBF:1; 
    unsigned char IBF:1; 
} TRISEbits ; 
#locate TRISEbits=0x89 

unsigned char TXREG; 
#locate TXREG=0x19 

unsigned char TXSTA; 
#locate TXSTA=0x98 

struct { 
    unsigned char TX9D:1; 
    unsigned char TRMT:1; 
    unsigned char BRGH:1; 
    unsigned char UNUSED:1; 
    unsigned char SYNC:1; 
    unsigned char TXEN:1; 
    unsigned char TX9:1; 
    unsigned char CSRC:1; 
} TXSTAbits ; 
#locate TXSTAbits=0x98 


#list 
#endif  /* SFR16F876_H */ 
