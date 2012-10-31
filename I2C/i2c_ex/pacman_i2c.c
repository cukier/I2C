#include "sfr_r81b.h"
#include "24C02.h"

// ---------------------- Low Level Functions ---------------

void waitFactor(unsigned int factor) {
unsigned int fc=0;
factor*=waitUnit;
for(fc=0; fc<factor; fc++) asm("NOP");
}
unsigned char getSDAState(void) { _24C02_sda_d = 0; asm("NOP"); return(_24C02_sda); }
unsigned char getSCLState(void) { _24C02_scl_d = 0; asm("NOP"); return(_24C02_scl); }
void setSDAHigh(void) { _24C02_sda_d = 1; _24C02_sda = 1; }
void setSCLHigh(void) { _24C02_scl_d = 1; _24C02_scl = 1; }
void setSDALow(void) { _24C02_sda_d = 1; _24C02_sda = 0; }
void setSCLLow(void) { _24C02_scl_d = 1; _24C02_scl = 0; }
void clockToHigh(void) { setSCLHigh(); waitFactor(1); }
void clockToLow(void) { setSCLLow(); waitFactor(1); }
void clockPulse(void) { clockToHigh(); clockToLow(); }
void sendBit(unsigned char bit) {
if(bit) {
setSDAHigh();
} else {
setSDALow();
}
waitFactor(1);
setSCLHigh(); waitFactor(1);
setSCLLow();  waitFactor(1);
}
unsigned char resetBus (void) {
unsigned char sdaStatus;
clockToHigh();
sdaStatus=getSDAState();
clockToLow();
return(sdaStatus==0?BUSY:ONLINE);
}
void startCondition(void) {
setSDAHigh(); waitFactor(1);
setSCLHigh(); waitFactor(1);
setSDALow(); waitFactor(1);
setSCLLow(); waitFactor(1);
}
void stopCondition(void) {
setSDALow(); waitFactor(1);
setSCLHigh(); waitFactor(1);
setSDAHigh(); waitFactor(1);
setSCLHigh(); waitFactor(1);
}
void writeByte(unsigned char byte) {
unsigned char rotateLeft;
unsigned char tmp;
unsigned char toReturn;
rotateLeft = 0;
while(rotateLeft<8) {
tmp=byte;
tmp=(tmp << rotateLeft) >> 7;
rotateLeft++;
sendBit(tmp);
}
}
unsigned char readByte(void) {
unsigned char byte=0;
unsigned char pos=0;
_24C02_sda_d=0;
while(pos<8) {
clockToHigh();
byte|=getSDAState();
if(pos<7) byte<<=1;
pos++;
clockToLow();
}
return(byte);
}
unsigned char readAck (void) {
unsigned char sdaStatus;
clockToHigh();
sdaStatus=getSDAState();
clockToLow();
return(sdaStatus==0?OK:NOK);
}

// ---------------------- High Level Functions ---------------

void Write_24C02(unsigned char deviceAddress, unsigned char menAddress, unsigned char byte) {
unsigned char myAck=0;
reset_bus:
while(resetBus()==BUSY);
device_address:
startCondition();
writeByte(deviceAddress);
myAck=readAck();
if(myAck==NOK) goto reset_bus;
men_address:
writeByte(menAddress);
myAck=readAck();
if(myAck==NOK) goto reset_bus;
send_byte:
writeByte(byte);
myAck=readAck();
if(myAck==NOK) goto reset_bus;
stopCondition();
}

unsigned char Read_24C02(unsigned char deviceAddress, unsigned char menAddress) {
unsigned char myAck=0;
unsigned char byte=0;
reset_bus:
while(resetBus()==BUSY);
device_address:
startCondition();
writeByte(deviceAddress);
myAck=readAck();
if(myAck==NOK) goto reset_bus;
men_address:
writeByte(menAddress);
myAck=readAck();
if(myAck==NOK) goto reset_bus;
restart_condition:
startCondition();
device_address_read:
writeByte(deviceAddress | 0x01);
myAck=readAck();
if(myAck==NOK) goto reset_bus;
read_byte:
byte = readByte();
sendBit(1);
stopCondition();
return(byte);
}