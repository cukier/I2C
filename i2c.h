/*
 * i2c.h
 *
 *  Created on: Oct 24, 2012
 *      Author: cuki
 */

#ifndef I2C_H_
#define I2C_H_

short handshake(int addr) {
	short ack = 1;
	i2c_start();
	ack = i2c_write(addr);
	i2c_stop();
	return ack;
}

int send_i2c(int addr, int mem, int data) {
	if (handshake(addr))
		return 0xFF;
	else {
		i2c_start();
		i2c_write(addr);
		i2c_write(mem);
		i2c_start();
		i2c_write(addr + 1);
		i2c_write(data);
		i2c_stop();
		return 0;
	}
}

int read_i2c(int addr, int data) {
	int retorno = 0;
	if (handshake(addr))
		return 0xFF;
	else {
		i2c_start();
		i2c_write(addr);
		i2c_write(data);
		i2c_start();
		i2c_write(addr + 1);
		retorno = i2c_read(0);
		i2c_stop();
		return retorno;
	}
}

#endif /* I2C_H_ */
