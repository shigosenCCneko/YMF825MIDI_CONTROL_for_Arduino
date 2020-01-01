#include <avr/io.h>
#include <avr/interrupt.h>
#include "controlLine.h"

#define ZERO  r1
.global USART_RX_vect
.global SPI_STC_vect

.global if_s_write  
.global usart_read
.global usart_cnt
.section .data


.extern wait_16clock

.global modulation_cnt
.global modulation_tblpointer
.global modulation_depth
.global modulation_pitch
.global sin_pitch    
.global sin_pointer    
.global sin_tbl_offs  
.global sin_tbl_address
.global sysex_buf
.global voice_queue
.global voice_queue_top
.global voice_queue_tail
.section .data
.section .bss




//  データテーブル定義

usart_recvData: .ds.b 256    //usart 受信バッファ

voice_queue:    .ds.b 16      
/* software modulation */
modulation_cnt:    .ds.b 16
modulation_tblpointer: .ds.b  16
modulation_depth: .ds.b 16
modulation_pitch: .ds.b 16

sin_pitch:      .ds.b 16
sin_pointer:    .ds.b 16
sin_tbl_offs:   .ds.b 16
sin_tbl_address:  .ds.b 32



usart_write_p: .dc.b 1
usart_read_p:  .dc.b 1
usart_cnt:     .dc.b 1   
voice_queue_top:  .dc.b 1
voice_queue_tail: .dc.b 1

sysex_buf:        .ds.b 65


.section .text
.func SPI_STC_vect
SPI_STC_vect:
reti

.endfunc

//
//  if_s_write(uint8_t adr,uint8_t dat)
//

.global if_s_write  
.func
if_s_write:
  cbi 0x5,2   //cs low
  out _SFR_IO_ADDR(SPDR),r24
  call wait_17clock
  out _SFR_IO_ADDR(SPDR),r22
  call wait_17clock
  sbi 0x5,2
  ret
.endfunc


//
//  if_chs_write(uint8_t ch, uint8_t adr,uint8_t dat)
//
.global if_chs_write  
.func
if_chs_write:
  ldi r23,0x0b
  cbi 0x5,2   //cs low
  out _SFR_IO_ADDR(SPDR),r23
  call wait_17clock
  out _SFR_IO_ADDR(SPDR),r24
  call wait_17clock
  sbi 0x5,2
  nop
  cbi 0x5,2
  out _SFR_IO_ADDR(SPDR),r22
  call wait_17clock
  out _SFR_IO_ADDR(SPDR),r20
  call wait_17clock
  sbi 0x5,2
  ret
.endfunc



.func usart_read
usart_read:
cli
  lds r25,usart_cnt
sei
  cpi r25,0
  brne serial_read
  lds r25,TCNT1L
  lds r24,TCNT1H
  cpi r25,0x20
  brcs usart_read
  clr r25
  sts TCNT1H,r25
  sts TCNT1L,r25
call software_modulation  
  rjmp usart_read

serial_read:


  lds r30,usart_read_p
    inc r30
    //andi r30,0x7f
    sts usart_read_p,r30 
cli
  
  lds r25,usart_cnt
   dec r25
  sts usart_cnt,r25
  //ldi r31,hi8(usart_recvData)
  clr r31
  subi r30,lo8(-(usart_recvData))
  sbci r31,hi8(-(usart_recvData))
  ld  r24,Z
  sei
  ret
.endfunc

.func USART_RX_vect
USART_RX_vect:
 cli
  push r0
  in r0, _SFR_IO_ADDR(SREG) 
  push r0


  push r30
  push r31
  
  lds r30,usart_write_p
  inc r30
 // andi r30,0x7f
  sts usart_write_p,r30
//  ldi r31,hi8(usart_recvData)
  clr r31
  subi r30,lo8(-(usart_recvData))
  sbci r31,hi8(-(usart_recvData))  
  
  lds r0,UDR0
  st Z,r0
  
  lds r0,usart_cnt
  inc r0
  sts usart_cnt,r0
  
  pop r31
  pop r30

  pop r0
  out _SFR_IO_ADDR(SREG),r0
  pop r0
  sei
  reti
.endfunc





