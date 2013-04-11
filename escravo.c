/*
 * http://www.ccsinfo.com/forum/viewtopic.php?t=21456
 * 11/04/2013 08:06
 */

#include <18F252.h>
#include "defines.h"

#fuses H4,NOWDT,NOPROTECT,NOLVP

#use delay(Clock=40MHz, chrystal=10MHz)
#use rs232(baud=9600,xmit=PIN_C6,rcv=PIN_C7)
#use i2c(slave, sda=EEPROM_SDA, scl=EEPROM_SCL, FORCE_HW)

int read_i2c(void);
void i2c_interrupt_handler(void);
void i2c_initialize(void);
void i2c_error(void);
void write_i2c(int transmit_byte);

#INT_SSP
void ssp_interupt() {
	i2c_interrupt_handler();
}

int slave_buffer[RX_BUF_LEN];
int buffer_index;
int comms_error;
int debug_state;


void i2c_interrupt_handler(void) {

	int i2c_mask = 0x2D; /* 0010 1101 */
	int temp_sspstat;
	int this_byte;
	int tx_byte;
	int x;

	/* Mask out the unnecessary bits */
	temp_sspstat = PIC_SSPSTAT & i2c_mask;

	switch (temp_sspstat) {
	/* Write operation, last byte was an address, buffer is full */
	case 0x09: /* 0000 1001 */
		/* Clear the receive buffer */
		for (x = 0; x < RX_BUF_LEN; x++) {
			slave_buffer[x] = 0x00;
		}
		buffer_index = 0; /* Clear the buffer index */
		this_byte = read_i2c(); /* Do a dummy read of PIC_SSPBUF */

		debug_state = 1;
		break;

		/* Write operation, last byte was data, buffer is full */
	case 0x29: /* 0010 1001 */
		/* Point to the buffer */
		this_byte = read_i2c(); /* Get the byte from the SSP */
		slave_buffer[buffer_index] = this_byte; /* Put it into the buffer */
		buffer_index++; /* Increment the buffer pointer */
		/* Get the current buffer index */
		/* Subtract the buffer length */
		/* Has the index exceeded the buffer length? */
		if (buffer_index >= RX_BUF_LEN) {
			buffer_index = 0; /* Yes, clear the buffer index. */
		}
		debug_state = 2;
		break;

		/* Read operation; last byte was an address, buffer is empty */
	case 0x0C: /* 0000 1100 */
		buffer_index = 0; /* Clear the buffer index */
		/* Point to the buffer */
		tx_byte = slave_buffer[buffer_index]; /* Get byte from the buffer */
		write_i2c(tx_byte); /* Write the byte to PIC_SSPBUF */
		buffer_index++; /* increment the buffer index */
		debug_state = 3;
		break;

		/* Read operation; last byte was data, buffer is empty */
	case 0x2C: /* 0010 1100 */
		/* Get the current buffer index */
		/* Subtract the buffer length */
		/* Has the index exceeded the buffer length? */
		if (buffer_index >= RX_BUF_LEN) {
			buffer_index = 0; /* Yes, clear the buffer index */
		}
		/* Point to the buffer */
		/* Get the byte */
		tx_byte = slave_buffer[buffer_index];
		write_i2c(tx_byte); /* Write to PIC_SSPBUF */
		buffer_index++; /* increment the buffer index */
		debug_state = 4;
		break;

		/* A NACK was received when transmitting data back from the master. */
		/* Slave logic is reset in this case. R_W=0, D_A=1, and BF=0. */
		/* If we don't stop in this state, then something is wrong!! */
	case 0x28: /* 0010 1000 */
		debug_state = 5;
		break;

		/* Something went wrong!! */
	default:
		i2c_error();
		break;
	}
}

void i2c_error(void) {
	comms_error = 1;
	printf("I2C ERROR!\r\n");
}

void write_i2c(int transmit_byte) {
	int write_collision = 1;

	while (PIC_SSPSTAT & PIC_SSPSTAT_BIT_BF) /* Is BF bit set in PIC_SSPSTAT? */
	{
		/* If yes, then keep waiting */
	}

	while (write_collision) {
		/* If not, then do the i2c_write. */
		PIC_SSPCON1 &= ~PIC_SSPCON1_BIT_WCOL; /* Clear the WCOL flag */
		PIC_SSPBUF = transmit_byte;

		/* Was there a write collision? */
		if (PIC_SSPCON1 & PIC_SSPCON1_BIT_WCOL) {
			/* Yes there was a write collision. */
			write_collision = 1;
		} else {
			/* NO, there was no write collision. */
			/* The transmission was successful */
			write_collision = 0;
		}
	}
	PIC_SSPCON1 |= PIC_SSPCON1_BIT_CKP; /* Release the clock. */
}

/* This function returns the byte in SSPBUF */
int read_i2c(void) {
	return PIC_SSPBUF;
}

void main(void) {
	debug_state = 0;
	enable_interrupts(INT_SSP); /* Enable MSSP interrupts. */
	enable_interrupts(GLOBAL);
	printf("i2c slave 09 Jan 2005\n\r\n\r");

	while (1) {
		if (debug_state) {
			// printf ("debug state = %d\r\n", debug_state);
			debug_state = 0;
		}

	}
}
