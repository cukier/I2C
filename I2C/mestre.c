/*
 * mestre.c
 *
 *  Created on: 22/10/2012
 *      Author: cuki
 */

#include <18F252.h>

#fuses	HS,NOOSCSEN,NOPUT,NOBROWNOUT,NOWDT,NOSTVREN,NOLVP
#fuses	NODEBUG,NOPROTECT,NOCPB,NOCPD,NOWRT,NOWRTC,NOWRTB
#fuses	NOWRTD,NOEBTR,NOEBTRB

#use delay(clock=8MHz)
#use rs232(xmit=pin_c6, rcv=pin_c7, baud=9600, parity=E, stop=1)
#use i2c(master, scl=pin_c3, sda=pin_c4, fast=2000000, force_hw)

#define addr_device 0xB0
#define memory_addr 0x01
#define data_addr	0x55
#define debounce	500

static short ext1, ext2;

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

int main(void) {

	int cont = 0;
	int aux = 0;

	enable_interrupts(INT_EXT1_H2L);
	enable_interrupts(INT_EXT2_H2L);
	enable_interrupts(GLOBAL);

	while (true) {
		printf("\f Ext1 check\n Ext2 sent:");
		if (ext1) {
			short ack = 0;
			delay_ms(debounce);
			ext1 = 0;
			printf("\fIniciando 0x%X", addr_device);
			do {
				ack = handshake(addr_device);
				if (ack) {
					printf("\n Nao repondendo");
					cont--;
				} else
					printf("\nOk");
			} while (ack || ext1);
		}
		if (ext2) {
			delay_ms(debounce);
			ext2 = 0;
			printf("\fEscrevendo 0x%X\nem 0x%X :", data_addr, memory_addr);
			aux = send_i2c(addr_device, memory_addr, data_addr);
			delay_ms(500);
			printf("  %s", aux == 0xFF ? "Error" : "Ok");
		}
	}
	return 0;
}

/*		switch (cont++) {
 case 0:

 break;
 case 1:
 aux = 0;
 printf("\fEnviando 0x%X\nem 0x%X :", data_addr, memory_addr);
 aux = send_i2c(addr_device, memory_addr, data_addr);
 printf("\n%s", aux == 0xFF ? "Error" : "Ok");
 break;
 case 2:
 aux = 0;
 printf("\fLendo em 0x%X", memory_addr);
 aux = read_i2c(addr_device, memory_addr);
 printf("\nleitura: 0x%X", aux);
 break;
 default:
 cont = 0;
 break;
 }*/
