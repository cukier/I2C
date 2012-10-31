/*
 * i2c_interrupts.h
 *
 *  Created on: Oct 24, 2012
 *      Author: cuki
 */

#ifndef I2C_INTERRUPTS_H_
#define I2C_INTERRUPTS_H_

#INT_EXT1
void isr_ext1() {
	clear_interrupt(INT_EXT1);
	ext1 = 1;
}

#INT_EXT2
void isr_ext2() {
	clear_interrupt(INT_EXT2);
	ext2 = 1;
}

#endif /* I2C_INTERRUPTS_H_ */
