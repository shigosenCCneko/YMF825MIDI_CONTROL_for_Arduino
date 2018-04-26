
/*
   IntSend.c

   Created: 2017/06/28 23:19:56
    Author: Keiji
*/
// USART.H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "controlLine.h"
#include "Managetone.h"
#include "IntSend.h"



void write_burst(void);
void flush_spi_buff(void);
void lwrite_burst(void);


//volatile uint8_t spi_sendData[256];    // spi
volatile uint8_t spi_send_write = 0;      //
volatile uint8_t spi_send_read = 0;       //
//volatile uint8_t send_buf_byte = 0;



uint8_t tone_reg[480];
extern uint8_t career_no[8][4];
extern uint8_t carrier_val[8];
extern uint8_t spi_sendData[256];
//extern uint8_t spi_send_write;
//extern uint8_t spi_send_read;
extern uint8_t send_buf_byte;





void send_atmega(char c) {

  SPDR = c;
  while ( !(SPSR & (1 << SPIF)));
  c =  SPDR;

}

void write_burst() {
  int voice_top_addr;
 char k, l, m, alg;
 int i;

  PORTD ^= 0x04;
  
  asm(
    
    "ldi  r24,0x08      \n\t"
    "ldi  r22,0x16      \n\t"
    "call if_s_write    \n\t"

    "ldi  r24,0x08      \n\t"
    "ldi  r22,0x00      \n\t"
    "call if_s_write    \n\t"

    "call flush_spi_buff   \n\t"
    "cbi 5,2         \n\t"
    
    
 ";cli             \n\t"
"   ldi r24,0x50  \n\t"   //    SPCR = (1 << SPE) | (1 << MSTR) | (0 << SPR0) | (0 << SPR1);
"   out 0x2c,r24  \n\t"   //　　usart受信割り込みは有効にしてSPIの送信割り込み停止


    "in r24,0x2e      \n\t"
    "ldi r24,0x07  \n\t"
    "out 0x2E,r24     \n\t"
    "wait1:             \n\t"
    "in  r24,0x2d     \n\t"
    "sbrs r24,7     \n\t"
    "rjmp wait1     \n\t"
    "in r24,0x2e      \n\t"

    "ldi r24,0x90  \n\t"
    "out 0x2E,r24     \n\t"
    "wait2:             \n\t"
    "in  r24,0x2d     \n\t"
    "sbrs r24,7     \n\t"
    "rjmp wait2     \n\t"
    "in r24,0x2e      \n\t"

    "  ldi r31,hi8(tone_reg)     \n\t"
    " ldi r30,lo8(tone_reg)     \n\t"
    
 
    "  ldi r25,240         \n\t"
 

    
"loop1:   \n\t"
 
    "  ld  r24,Z+   \n\t"     
    "  out 0x2E,r24   \n\t"
  
    "wait_L1:          \n\t"

    "in  r24,0x2d     \n\t"
    "sbrs r24,7     \n\t"
    "rjmp wait_L1     \n\t"

";in r24,0x2e      \n\t"
    "subi r25,1  \n\t"
    "brne loop1 \n\t"
    

    "  ldi r25,240          \n\t"
 

    
    "loop2:   \n\t"

    
    "  ld  r24,Z+   \n\t"
    "  out 0x2E,r24   \n\t"
  
    "wait_L2:           \n\t"

    "in  r24,0x2d     \n\t"
    "sbrs r24,7     \n\t"
    "rjmp wait_L2     \n\t"

 ";in r24,0x2e      \n\t"

    "subi r25,1  \n\t"
    "brne loop2 \n\t"

    "ldi r24,0x80  \n\t"
    "out 0x2E,r24     \n\t"
    "wait3:             \n\t"
    "in  r24,0x2d     \n\t"
    "sbrs r24,7     \n\t"
    "rjmp wait3     \n\t"
    "in r24,0x2e      \n\t"

    "ldi r24,0x03  \n\t"
    "out 0x2E,r24     \n\t"
    "wait4:             \n\t"
    "in  r24,0x2d     \n\t"
    "sbrs r24,7     \n\t"
    "rjmp wait4     \n\t"
    "in r24,0x2e      \n\t"

    "ldi r24,0x81  \n\t"
    "out 0x2E,r24     \n\t"
    "wait5:             \n\t"
    "in  r24,0x2d     \n\t"
    "sbrs r24,7     \n\t"
    "rjmp wait5     \n\t"
    "in r24,0x2e      \n\t"

    "ldi r24,0x80  \n\t"
    "out 0x2E,r24     \n\t"
    "wait6:             \n\t"
    "in  r24,0x2d     \n\t"
    "sbrs r24,7     \n\t"
    "rjmp wait6     \n\t"
    "in r24,0x2e      \n\t"

"   ldi r24,0xd0  \n\t"   //  SPCR = (1 << SPE) | (1 << MSTR) | (0 << SPR0) | (0 << SPR1) | (1 << SPIE);
"   out 0x2c,r24  \n\t"

    
    "sbi 5,2        \n\t"
    ";sei            \n\t"
  );
  voice_top_addr = 0;
  for (i = 0; i < 16; i++) {
    alg = tone_reg[voice_top_addr + 1] & 0x07;	//get algorithm no
    l = carrier_val[alg];

    k = 255;
    while (l != 0) {
      l--;
      m = career_no[alg][l];
      m = tone_reg[voice_top_addr + m * 7 + 3]  & 0xf0; // get release late * 16
      if (m < k)
        k = m;

    }
    rel_optval[i] = ((240 - k) >> 6);
    voice_top_addr += 30;
  }


}

void lwrite_burst() {
  //int voice_top_addr;
  //char k,l,m,alg;
  int i;
  char *tone;
  tone = tone_reg;


  if_s_lwrite(0x08, 0x16);

  if_s_lwrite(0x08, 0x00);
  flush_spi_buff();
  //while(send_buf_byte>0);

  wrl_lo();
  cli();
  send_atmega(0x07);
  send_atmega(0x90);

  for (i = 0; i < 480; i++) {

    send_atmega(*tone);
    tone++;

  }
  send_atmega(0x80);
  send_atmega(0x03);
  send_atmega(0x81);
  send_atmega(0x80);
  wrl_hi();
  sei();
}


