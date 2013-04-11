/*
 * escravo.c
 *
 *  Created on: Oct 24, 2012
 *      Author: cuki
 */

#include <18F252.h>

#fuses	H4,NOOSCSEN,NOPUT,NOBROWNOUT,NOWDT,NOSTVREN,NOLVP
#fuses	NODEBUG,NOPROTECT,NOCPB,NOCPD,NOWRT,NOWRTC,NOWRTB
#fuses	NOWRTD,NOEBTR,NOEBTRB

#use delay(clock=32MHz, crystal=8MHz)
#use rs232(xmit=pin_c6, rcv=pin_c7, baud=9600, parity=N, stop=1)
#use i2c(slave, scl=pin_c3, sda=pin_c4, fast=1000000, force_hw, address=0xB0)

static int primeiraLeitura, segundaLeitura, terceiraLeitura, print, cont,
		estado, lido;

#int_ssp
void ssp_interrupt() {

	estado = i2c_isr_state();
	lido = 0;

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
		printf("\fPronto...");
		cont = 0;
		switch (print) {
		case 3:
			printf("\n\rTerceira Leitrua: 0x%X", terceiraLeitura);
		case 2:
			printf("\n\rSegunda Leitura:  0x%X", segundaLeitura);
		case 1:
			printf("\n\rPrimeira Leitura: 0x%X", primeiraLeitura);
		default:
			print = 0;
			break;
		}
		delay_ms(1500);
	}
	return 0;
}
