/*
 * http://www.ccsinfo.com/forum/viewtopic.php?t=21456
 * 11/04/2013 08:06
 */

#include <18F252.H>
#include "defines.h"
// 10-bit A/D conversion
#device ADC=10
#fuses HS,NOWDT,NOPROTECT,NOLVP

#use Delay(Clock=20000000)
#use rs232(baud=9600,xmit=PIN_C6,rcv=PIN_C7,brgh1ok)
#use i2c(master, sda=EEPROM_SDA, scl=EEPROM_SCL, FORCE_HW)

void initI2C() {
	output_float(EEPROM_SCL);
	output_float(EEPROM_SDA);
}

void writeI2C(INT16 word) {
	i2c_start();
	//delay_ms(1);
	i2c_write(SLAVE_ADDRESS); /* Device Address */
	//delay_ms(1);
	i2c_write(word & 0x00ff);
	//delay_ms(1);
	i2c_write((word & 0xff00) >> 8);
	//delay_ms(1);
	i2c_stop();
}

INT16 readI2C() {
	BYTE b1 = 0, b2 = 0;
	i2c_start();   // restart condition
	i2c_write(SLAVE_ADDRESS + 1);
	b1 = i2c_read(1);
	//delay_ms(1);
	b2 = i2c_read(0);
	//delay_ms(1);
	i2c_stop();
	return make16(b2, b1);
}

INT16 gethexword() {
	BYTE lo, hi;

	hi = gethex();
	lo = gethex();

	return make16(hi, lo);
}

void main() {

	int cmd;
	INT16 value;

	initI2C();

	printf("i2c master 09 Jan 2005\n\r\n\r");

	do {
		do {
			printf("\r\nRead or Write: ");
			cmd = getc();
			cmd = toupper(cmd);
			putc(cmd);
		} while ((cmd != 'R') && (cmd != 'W'));

		if (cmd == 'R') {
			value = 0;
			value = readI2C();
			printf("\r\nValue: %lX\r\n", value);
		} else if (cmd == 'W') {
			printf("\r\nNew 16-bit value: ");
			value = gethexword();
			printf("\n\r");
			writeI2C(value);
		}

	} while (TRUE);
}
