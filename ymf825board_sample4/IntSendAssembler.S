#include <avr/io.h>
#include <avr/interrupt.h>
#include "controlLine.h"

#define ZERO  r1
.global USART_RX_vect
.global SPI_STC_vect

.global if_s_write  
.global if_s_rwrite
.global if_s_lwrite
.global flush_spi_buff
.global usart_read

.global spi_sendData
.global send_buf_byte
.global usart_cnt
.section .data
.extern send_buf_byte
.extern usart_cnt
spi_send_write: .dc.b 0
spi_send_read:  .dc.b 0
send_buf_byte:  .dc.b 0


usart_write_p: .dc.b 0
usart_read_p:  .dc.b 0
usart_cnt:     .dc.b 0   

.section .bss
.balign 256
  ;バッファを256byteの境界域に確保
spi_sendData: .ds.b 256
.balign 256
usart_recvData: .ds.b 256

.section .text
.func SPI_STC_vect
SPI_STC_vect:
  cli

  push r0
  in r0, _SFR_IO_ADDR(SREG) 
  push r0

  push r24
  push r30
  push r31

  lds r24,send_buf_byte
  subi r24,1
  sts send_buf_byte,r24
  tst r24
  breq NO_DATA

  sbrs r24,0
  rjmp FIRST_BYTE

  lds r30,spi_send_read
  ldi r31,hi8(spi_sendData)
  ld r24,Z
  out _SFR_IO_ADDR(SPDR),r24
  inc r30
  sts spi_send_read,r30

  pop r31
  pop r30
  pop r24
  pop r0
  out _SFR_IO_ADDR(SREG),r0
  pop r0
  sei
  reti

NO_DATA:  
  sbi 0x5,2
  pop r31
  pop r30
  pop r24
  pop r0
  out _SFR_IO_ADDR(SREG),r0
  pop r0
  sei
  reti

FIRST_BYTE: 
  sbi 0x5,2       ;SSをHI

  lds r30,spi_send_read
  cbi 0x5,2         ;1 SPI clock 以上空けてSSをLOにする
  ldi  r31,hi8(spi_sendData)

  ld r24,Z
  out _SFR_IO_ADDR(SPDR),r24

  inc r30
  sts spi_send_read,r30
  pop r31
  pop r30
  pop r24
  pop r0
  out _SFR_IO_ADDR(SREG),r0
  pop r0
  sei
  reti

.endfunc



//
//  if_s_write(uint8_t adr,uint8_t dat)
//
.global if_s_write  
.func
if_s_write:

  cli
  
  lds r25,send_buf_byte
  lds r30,spi_send_write
  
  cpse r25,ZERO
  rjmp ON_SEND    //送信中

  ldi r31,hi8(spi_sendData)
  st Z,r22
  inc r30
  sts spi_send_write,r30
  ldi r25,2
  sts send_buf_byte,r25
  cbi 0x5,2
  out _SFR_IO_ADDR(SPDR),r24
//cbi _SFR_IO_ADDR(PORTC),7

  sei
  ret


ON_SEND:        //送信中、バファに積むだけ
  ldi r31,hi8(spi_sendData)
  st Z,r24
  inc r30
  st Z,r22
  inc r30
  sts spi_send_write,r30

  subi r25,(-2)
  sts  send_buf_byte,r25

/*ldi r24,4
sub r24,r25
ori r24,0x40
out _SFR_IO_ADDR(PORTC),r24*/

  sei
  ret     


.endfunc

/*
 *  void flush_spi_buff(void)
 *
 *  送信バッファが空になるまでループ
 */

.func
flush_spi_buff:

loop1:
  lds r24,send_buf_byte
  tst r24
brne loop1
  ret
.endfunc



.func usart_read
usart_read:
cli
  lds r25,usart_cnt
sei
  cpi r25,0
  brne serial_read
  
  
  
  rjmp usart_read

serial_read:
cli
  
  lds r25,usart_cnt
   dec r25
  sts usart_cnt,r25
  lds r30,usart_read_p
  ldi r31,hi8(usart_recvData)
  ld  r24,Z+
  sts usart_read_p,r30
  sei
  ret
.endfunc

.func USART_RX_vect
USART_RX_vect:
 cli
  push r0
  in r0, _SFR_IO_ADDR(SREG) 
  push r0

  push r24
  push r30
  push r31
  
  lds r30,usart_write_p
  ldi r31,hi8(usart_recvData)  
  
  lds r24,UDR0
  st Z+,r24
  
  sts usart_write_p,r30
  lds r24,usart_cnt
  inc r24
  sts usart_cnt,r24
  
  pop r31
  pop r30
  pop r24
  pop r0
  out _SFR_IO_ADDR(SREG),r0
  pop r0
  sei
  reti
.endfunc

/*
*/


