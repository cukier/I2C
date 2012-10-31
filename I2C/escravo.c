/*
 * escravo.c
 *
 *  Created on: Oct 24, 2012
 *      Author: cuki
 */

#include <18F252.h>

#fuses	HS,NOOSCSEN,NOPUT,NOBROWNOUT,NOWDT,NOSTVREN,NOLVP
#fuses	NODEBUG,NOPROTECT,NOCPB,NOCPD,NOWRT,NOWRTC,NOWRTB
#fuses	NOWRTD,NOEBTR,NOEBTRB

#use delay(clock=8MHz)
#use rs232(xmit=pin_c6, rcv=pin_c7, baud=9600, parity=E, stop=1)
#use i2c(slave, scl=pin_c3, sda=pin_c4, fast=2000000, force_hw, address=0xB0)

static int primeiraLeitura, segundaLeitura, terceiraLeitura, print;

#int_ssp
void ssp_interrupt() {

	register int lido = 0;
	register int estado = i2c_isr_state();

	if (estado < 0x80) {
		lido = i2c_read();
		if (estado == 1) { //primeira leitura
			primeiraLeitura = lido;
			print = 1;
		} else if (estado == 2) { //segunda leitura
			segundaLeitura = lido;
			print = 2;
		} else if (estado == 3) { //terceira leitura
			terceiraLeitura = lido;
			print = 3;
		}
	} else if (estado == 0x80) {
	}
}

int main(void) {
	enable_interrupts(INT_SSP);
	enable_interrupts(GLOBAL);
	while (true) {
		switch (print) {
		case 3:
			printf("\t\t\t%d", terceiraLeitura);
		case 2:
			printf("\t%d", segundaLeitura);
		case 1:
			printf("%d", primeiraLeitura);
		default:
			print = 0;
			break;
		}
	}
	return 0;
}
