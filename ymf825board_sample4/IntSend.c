
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

#include <avr/pgmspace.h>

#define USART_SEND_ENABLE()  ( UCSR0B |=  _BV(UDRIE0) )
#define USART_SEND_DISABLE() ( UCSR0B &= ~_BV(UDRIE0) )

void write_burst(void);
void flush_spi_buff(void);


uint8_t tone_reg[480];

extern  const   uint8_t career_no[8][4]PROGMEM  ;
extern uint8_t carrier_val[8];

extern uint8_t send_buf_byte;
volatile char * usart_sendpoint;
uint8_t usart_sendcnt = 0;

/* リリースレートの減衰値 queueの最適化用 */
uint8_t rer_time[16] = {255,128,64,32,16,8,4,2,1,0,0,0,0,0,0,0};

void send_atmega(char c) {

  SPDR = c;
  while ( !(SPSR & (1 << SPIF)));
  c =  SPDR;

}

/* アセンブラ内から呼ばれるサブルーチン */
asm(
  "   write_wait_data:     \n\t"   //15   SPI書き込みのタイミングはloopさせると上手く合わないので無効命令で17Clockに合わせた
  "   ld  r24,Z+        \n\t"      //17
  "   out 0x2E,r24      \n\t"
  "   lpm r24,Z         \n\t"      //3
  "   lpm r24,Z         \n\t"      //6
  "   nop               \n\t"      //7
  "   ret               \n\t"      //11

  "wait_16clock:        \n\t"
  "   lpm r24,Z            \n\t"
  "   lpm r24,Z            \n\t"
  "   nop                  \n\t"
  "   nop                  \n\t"
  "   ret                  \n\t"

);

void write_burst() {
  int voice_top_addr;
  char k, l, m, alg;
  int i;

  asm(
    " ldi  r24,0x08           \n\t"
    " ldi  r22,0x16           \n\t"
    " cli                     \n\t"
    " call if_s_write         \n\t"
    " sei                     \n\t"

    " ldi  r24,0x08           \n\t"
    " ldi  r22,0x00           \n\t"
    " cli                     \n\t"    
    " call if_s_write         \n\t"
    " sei                     \n\t"
//    " call flush_spi_buff     \n\t"
    " cbi 5,2                 \n\t"


//    " ldi r24,0x50            \n\t"    //SPI割り込みのみ停止、Serialの受信割り込みは続ける
//    " out 0x2c,r24            \n\t"


    " in r24,0x2e             \n\t"
    " ldi r24,0x07            \n\t"
    " out 0x2E,r24            \n\t"
    " call wait_16clock       \n\t"


    " ldi r24,0x90            \n\t"
    " out 0x2E,r24            \n\t"
    " call wait_16clock       \n\t"


    " ldi r31,hi8(tone_reg)   \n\t"
    " ldi r30,lo8(tone_reg)   \n\t"

    ".rept 480  \n\t"         //480回 write_wait_dataを呼ぶ
    " call write_wait_data   \n\t"
    ".endr                    \n\t"


    " rjmp .                  \n\t"
    " rjmp .                  \n\t"
    " rjmp .                  \n\t"


    " ldi r24,0x80            \n\t"
    " out 0x2E,r24            \n\t"
    " call wait_16clock       \n\t"

    " ldi r24,0x03            \n\t"
    " out 0x2E,r24            \n\t"
    " call wait_16clock       \n\t"


    " ldi r24,0x81            \n\t"
    " out 0x2E,r24            \n\t"

    " in  r24,0x2d            \n\t"
    " sbrs r24,7              \n\t"
    " rjmp .-6                \n\t"


    " ldi r24,0x80            \n\t"
    " out 0x2E,r24            \n\t"
    " in  r24,0x2d            \n\t"
    " sbrs r24,7              \n\t"
    " rjmp .-6                \n\t"
    " in r24,0x2e             \n\t"
    " sbi  5,2                \n\t"

//    " ldi r24,0xd0            \n\t"
//    " out 0x2c,r24            \n\t"
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
/* リリースレートカウンタの残置でqueueの入れ替えをする */
     k = k >>4;
    rel_optval[i] = rer_time[(int)k];
    voice_top_addr += 30;
  }
}
/* 30byte割り込み送信処理*/
ISR( USART_UDRE_vect)
{ 

  UDR0 = *usart_sendpoint;
  usart_sendpoint++;
  _delay_us(3);       // atmega16u8の転送が間に合わないのでディレイを入れる（最小値2)
  //UDR0 = (uint8_t *)(usart_sendpoint++);

  if(--usart_sendcnt == 0)
     USART_SEND_DISABLE();   




}

void set_usartsendpoint(char *p){

   
  usart_sendpoint = p;
  usart_sendcnt = 30;
  USART_SEND_ENABLE();




}




