/*
 * http://www.ccsinfo.com/forum/viewtopic.php?t=21456
 * 11/04/2013 08:06
 */

#include <16F876A.H>

// 10-bit A/D conversion
#device ADC=10
#fuses HS,NOWDT,NOPROTECT,NOLVP

#use Delay(Clock=20000000)
#use rs232(baud=9600,xmit=PIN_C6,rcv=PIN_C7,brgh1ok)

unsigned char read_i2c(void);
void i2c_interrupt_handler(void);
void i2c_initialize(void);
void i2c_error(void);
void write_i2c(unsigned char transmit_byte);

#INT_SSP
void ssp_interupt() {
	i2c_interrupt_handler();
}

/* 16f87X bytes */
/* Change it per chip */
#byte PIC_SSPBUF=0x13
#byte PIC_SSPADD=0x93
#byte PIC_SSPSTAT=0x94
#byte PIC_SSPCON1=0x14
#byte PIC_SSPCON2=0x91

/* Bit defines */
#define PIC_SSPSTAT_BIT_SMP     0x80
#define PIC_SSPSTAT_BIT_CKE     0x40
#define PIC_SSPSTAT_BIT_DA      0x20
#define PIC_SSPSTAT_BIT_P       0x10
#define PIC_SSPSTAT_BIT_S       0x08
#define PIC_SSPSTAT_BIT_RW      0x04
#define PIC_SSPSTAT_BIT_UA      0x02
#define PIC_SSPSTAT_BIT_BF      0x01

#define PIC_SSPCON1_BIT_WCOL    0x80
#define PIC_SSPCON1_BIT_SSPOV   0x40
#define PIC_SSPCON1_BIT_SSPEN   0x20
#define PIC_SSPCON1_BIT_CKP     0x10
#define PIC_SSPCON1_BIT_SSPM3   0x08
#define PIC_SSPCON1_BIT_SSPM2   0x04
#define PIC_SSPCON1_BIT_SSPM1   0x02
#define PIC_SSPCON1_BIT_SSPM0   0x01

#define PIC_SSPCON2_BIT_GCEN    0x80
#define PIC_SSPCON2_BIT_ACKSTAT 0x40
#define PIC_SSPCON2_BIT_ACKDT   0x20
#define PIC_SSPCON2_BIT_ACKEN   0x10
#define PIC_SSPCON2_BIT_RCEN    0x08
#define PIC_SSPCON2_BIT_PEN     0x04
#define PIC_SSPCON2_BIT_RSEN    0x02
#define PIC_SSPCON2_BIT_SEN     0x01

#define RX_BUF_LEN  32
#define NODE_ADDR   0x02    /* I2C address of the slave node */

unsigned char slave_buffer[RX_BUF_LEN];
int buffer_index;
int comms_error;
int debug_state;

void i2c_initialize(void) {
	/* Set up SSP module for 7-bit */
	PIC_SSPCON1 = 0x36; /* 0011 0101 */
	PIC_SSPADD = NODE_ADDR; /* Set the slave's address */
	PIC_SSPSTAT = 0x00; /* Clear the SSPSTAT register. */
	enable_interrupts(INT_SSP); /* Enable MSSP interrupts. */
}

void i2c_interrupt_handler(void) {

	unsigned char i2c_mask = 0x2D; /* 0010 1101 */
	unsigned char temp_sspstat;
	unsigned char this_byte;
	unsigned char tx_byte;
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

void write_i2c(unsigned char transmit_byte) {
	unsigned char write_collision = 1;

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
unsigned char read_i2c(void) {
	return PIC_SSPBUF;
}

void main(void) {
	debug_state = 0;
	i2c_initialize();
	enable_interrupts(GLOBAL);
	printf("i2c slave 09 Jan 2005\n\r\n\r");

	while (1) {
		if (debug_state) {
			// printf ("debug state = %d\r\n", debug_state);
			debug_state = 0;
		}

	}
}
