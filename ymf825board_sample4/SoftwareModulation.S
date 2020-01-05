
/*
 * SoftwareModuration.S
 *
 * Created: 2018/12/08 19:09:52
 *  Author: Keiji
 */ 

#include <avr/io.h>


.extern divtbl2

.extern modulation_cnt
.extern modulation_tblpointer
.extern modulation_depth
.extern modulation_pitch
.extern sin_pitch:     
.extern sin_pointer:    
.extern sin_tbl_offs:   
.extern sin_tbl_address: 

#define CNT_IL r18
#define CNT_IH r19
#define WORK_S r20
#define WORK_C r26  //r21
#define WORK_E r27  //r22
#define ZERO   r1
 
.global modulation_cnt
.global modulation_tblpointer
.global modulation_depth


.global modulation_pitch  ; //modulation の周期
.global sin_pointer;
.global sin_pitch
.global sin_tbl_offs
.global sin_tbl_address


.section  .bss
//.align 2
//modulation_cnt:    .ds.b 16
//modulation_tblpointer: .ds.b  16
//modulation_depth: .ds.b 16
//modulation_pitch: .ds.b 16
//sin_pitch:      .ds.b 16
//sin_pointer:    .ds.b 16
//sin_tbl_offs:   .ds.b 16
//sin_tbl_address:  .ds.b 32

 .section .text

.global wait_17clock  ;サブルーチンコールとリターンで17clock
.func wait_17clock
 wait_17clock:
     nop 
     nop
     nop
     nop
     nop
     nop
     nop
     nop        
     nop
     ret  
.endfunc
     
.global software_modulation
.func  software_modulation
software_modulation:

  push r0
  push r1
  push r28    ;Y
  push r29

  ldi CNT_IL,0x10;
  ldi CNT_IH,0
  dec CNT_IL

LOOP_1:

  movw ZL,CNT_IL
  subi ZL,lo8(-(modulation_cnt))
  sbci ZH,hi8(-(modulation_cnt))
  ld WORK_C, Z ; cnt

  cpi WORK_C,0
  brne DO_MODULATE
  
  rjmp NEXT_CHANNEL

DO_MODULATE:  

  subi WORK_C,1
  breq CALC_MODULATE
  st Z,WORK_C
  rjmp NEXT_CHANNEL
  
CALC_MODULATE:



  movw YL,CNT_IL
  subi YL,lo8(-(modulation_pitch))
  sbci YH,hi8(-(modulation_pitch))
  ld   WORK_C,Y
  st   Z,WORK_C   //cnt <- pitch

  ldd   WORK_S,Y+16 //WORK_S <- sin_pitch[i]
  ldd   WORK_C,Y+32 //WORK_C <- sin_pointer[i]
  add  WORK_C,WORK_S
  andi WORK_C,0x3f;
  std   Y+32,WORK_C

  ldd   WORK_S,Y+48 //WORK_S <- sin_tbl_offs[i]
//----------------------------------------------------------------------------------


  ldi  WORK_E,(32)  ;テーブルサイズ:乗数用
  
  movw YL,CNT_IL        //指定テーブルアドレス読み出し
  lsl  YL
  subi YL,lo8(-(sin_tbl_address))
  sbci YH,hi8(-(sin_tbl_address))
  ld   ZL,Y+
  ld   ZH,Y

  clr ZERO
  add ZL,WORK_C
  adc ZH,ZERO
  lpm  WORK_C,Z   //offset無し値,読み込み

  lsr WORK_S  // offset >> 1
  brcc  pass_shift1
  neg WORK_C

pass_shift1:
  add  WORK_C,WORK_E  //テーブル位置マイナスオフセット補正
  mul  WORK_C, WORK_E
  movw ZL,r0
  subi ZL,lo8(-(divtbl2))
  sbci ZH,hi8(-(divtbl2))
  clr ZERO
  add ZL,WORK_S
  adc ZH,ZERO
  lpm  WORK_S, Z


//---------------------------------------------------------------------------------
  movw YL,CNT_IL    //c = modulation_tblpointer[i]
  subi YL, lo8(-(modulation_tblpointer))
  sbci YH, hi8(-(modulation_tblpointer))

  ld   WORK_C, Y
  inc  WORK_C
  andi WORK_C, 0x3f
  st   Y, WORK_C
  add  WORK_C, WORK_S
  andi WORK_C, 0x3f //modulation_tblpointer

  ldd   WORK_S, Y+16  //modulation_depth
//--------------------------------------------------


  movw YL, CNT_IL
  lsl  YL
  subi YL, lo8(-(sin_tbl_address))
  sbci YH, hi8(-(sin_tbl_address))
  ld   ZL ,Y+
  ld   ZH ,Y


  
  clr ZERO
  add ZL,WORK_C
  adc ZH,ZERO
  lpm WORK_C,Z   //offset無値読み出し

  asr WORK_S  //modulation_depth >> 1
  brcc pass_shift2
  neg WORK_C
pass_shift2:

  add WORK_C,WORK_E //テーブル位置マイナスオフセット補正

  mul WORK_C,WORK_E
  movw ZL,r0






  
  subi ZL,lo8(-(divtbl2))
  sbci ZH,hi8(-(divtbl2))
  clr ZERO
  add ZL,WORK_S
  adc ZH,ZERO
  lpm WORK_E,Z

  
  ldi r24,0x0b
  cbi 0x5,2   //cs low
  out _SFR_IO_ADDR(SPDR),r24
//  call wait_17clock
//-----------------------------------------------------------



  //midi_ch[ym825_voice_ch[i].midi_ch].reg_18
  
  
  ldi  YL,9 //sizeof voice_ch
  mul  CNT_IL,YL                                            ;-1
  movw YL,r0                                                ;-3

  subi YL,lo8(-(ym825_voice_ch))                            ;-4
  sbci YH,hi8(-(ym825_voice_ch))                            ;-5
  ldd  ZL,Y+3                                               ;-6
  ldi  ZH,19   //***********size of Midi_ch *************** ;-8

  mul  ZH,ZL                                  ;-9
  
  movw YL,r0                                  ;-11
  subi YL,lo8(-(midi_ch))                     ;-12
  sbci YH,hi8(-(midi_ch))                     ;-13

// tremolo /

  ldd WORK_C,Y+12 //reg16                       ;-14

  
  add WORK_C,WORK_E //equal 2bit right shift    ;-16
  andi WORK_C,0xfc                              ;-17


//-------------------------------------------------------
  out _SFR_IO_ADDR(SPDR),CNT_IL
  call wait_17clock
  sbi 0x5,2 

  ldi r24,0x10
  cbi 0x5,2   //cs low
  out _SFR_IO_ADDR(SPDR),r24
  call wait_17clock
  out _SFR_IO_ADDR(SPDR),WORK_C
  call wait_17clock
  sbi 0x5,2



//  ldi   r24,0x0b
//  mov   r22,CNT_IL
//  call if_s_write
//  ldi  r24,0x10
// mov  r22,WORK_C
//  call if_s_write


///tremolo end /

// vivrateとtremoroが逆になっていたのを補正
//  neg WORK_E

/// vibrate /
  ldi r24,0x12
  cbi 0x5,2   //cs low
  out _SFR_IO_ADDR(SPDR),r24


      ldd  WORK_C,Y+14 //reg18  ;2    ウェイト中に計算
      ldd  WORK_S,Y+15 //reg19  ;4
  //    asr WORK_E                ;5
      asr WORK_E                ;6

      add  WORK_S,WORK_E        ;5
    
    
      cpi WORK_S,0x80           ;6
     brmi WRITE_DATA            ;8
    
      sbrc WORK_E,7   ;9
      dec  WORK_C     ;10
      sbrs WORK_E,7   ;11
      inc  WORK_C     ;12
      rjmp  WRITE_DATA_PASS ;13
      
WRITE_DATA:     //pass when 10
      nop ;9
      nop ;11
      nop ;11
      nop ;12
      nop ;13
 WRITE_DATA_PASS:      
      nop ;14

      nop ;15
      nop ;16
      nop ;17

  out _SFR_IO_ADDR(SPDR),WORK_C
  call wait_17clock
  sbi 0x5,2
  
  ldi r24,0x13
  cbi 0x5,2   //cs low
  out _SFR_IO_ADDR(SPDR),r24
  call wait_17clock
  out _SFR_IO_ADDR(SPDR),WORK_S
  call wait_17clock
  sbi 0x5,2       
  
//  ldi  r24,0x12
//  mov  r22,WORK_C
//  call if_s_write
//  ldi  r24,0x13
//  mov  r22,WORK_S
//  call if_s_write

NEXT_CHANNEL:

  subi CNT_IL,1
  brcs END_LOOP
  rjmp LOOP_1

END_LOOP: 


  pop r29
  pop r28


  pop r1
  pop r0
  clr r1
  ret

.endfunc






