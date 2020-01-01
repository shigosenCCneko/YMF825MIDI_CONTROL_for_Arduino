/*
 * CalcExponentiation.S
 *
 * Created: 2017/10/03 21:25:44
 *  Author: Keiji
 */ 
 /*
        AH, AL
      * BH, BL
    -------------------
    CH,CMH,CML, CL

    pitchbend値　-8196 <= x < 8196

    4bit右シフトして精度10bit

    -512 <= x < 512　センター512にオフセットさせる

    0 <= x < 1024 テーブル参照のため+512
    
    pow(2, (x-512)(step/(1200*512))

    pow(2,  x(step/(1200*512)) * pow(2,step/1200)

    各stepごとに定数として

    pow(2,step/1200)


    pow(2, x(step/(1200*512)) のｘを2進数のべき乗に分解して

    pow(2, 512*(step/(1200*512)) * pow(2, 256*(step/(1200*512))　*   128,64,32,16,8,4,2,1 
         ---                ---             
    Xのビットを見て乗算する

    乗算10回 約400クロック　16Mhzで　0.025ms


 */

#include <avr/io.h>

#define AH r27
#define AL r26

#define BL r22
#define BH r23

#define CH r21
#define CMH r20
#define CML r19
#define CL r18
#define ZERO r17

#define TBLP_L r30
#define TBLP_H r31
#define EXP_L r24
#define EXP_H r25

.section .data

.align 2
.section .text
//              pow(2,-step)  512   256   128   64    32    16    8   4   2   1
expon_tbl:
    .dc.w     0x3C68, 0x43CE, 0x41E0, 0x40EE, 0x4076, 0x403B, 0x401D, 0x400E, 0x4007, 0x4003, 0x4001
    .dc.w   0x3904, 0x47D6, 0x43CE, 0x41E0, 0x40EE, 0x4076, 0x403B, 0x401D, 0x400E, 0x4007, 0x4003
    .dc.w   0x35D1, 0x4C1B, 0x45CA, 0x42D5, 0x4166, 0x40B2, 0x4058, 0x402C, 0x4016, 0x400B, 0x4005
    .dc.w   0x32CB, 0x50A2, 0x47D6, 0x43CE, 0x41E0, 0x40EE, 0x4076, 0x403B, 0x401D, 0x400E, 0x4007
    .dc.w   0x2FF2, 0x556E, 0x49F1, 0x44CA, 0x425A, 0x412A, 0x4094, 0x404A, 0x4025, 0x4012, 0x4009
    .dc.w   0x2D41, 0x5A82, 0x4C1B, 0x45CA, 0x42D5, 0x4166, 0x40B2, 0x4058, 0x402C, 0x4016, 0x400B
    .dc.w   0x2AB7, 0x5FE4, 0x4E56, 0x46CE, 0x4351, 0x41A3, 0x40D0, 0x4067, 0x4033, 0x4019, 0x400C
    .dc.w   0x2851, 0x6597, 0x50A2, 0x47D6, 0x43CE, 0x41E0, 0x40EE, 0x4076, 0x403B, 0x401D, 0x400E
    .dc.w   0x260D, 0x6BA2, 0x52FF, 0x48E1, 0x444C, 0x421D, 0x410C, 0x4085, 0x4042, 0x4021, 0x4010
    .dc.w   0x23EB, 0x7208, 0x556E, 0x49F1, 0x44CA, 0x425A, 0x412A, 0x4094, 0x404A, 0x4025, 0x4012
    .dc.w   0x21E7, 0x78D0, 0x57EE, 0x4B04, 0x454A, 0x4297, 0x4148, 0x40A3, 0x4051, 0x4028, 0x4014
    .dc.w   0x2000, 0x8000, 0x5A82, 0x4C1B, 0x45CA, 0x42D5, 0x4166, 0x40B2, 0x4058, 0x402C, 0x4016
    .dc.w   0x1E34, 0x879C, 0x5D29, 0x4D37, 0x464C, 0x4313, 0x4185, 0x40C1, 0x4060, 0x4030, 0x4018
    .dc.w   0x1C82, 0x8FAC, 0x5FE4, 0x4E56, 0x46CE, 0x4351, 0x41A3, 0x40D0, 0x4067, 0x4033, 0x4019
    .dc.w   0x1AE8, 0x9837, 0x62B3, 0x4F7A, 0x4752, 0x438F, 0x41C1, 0x40DF, 0x406F, 0x4037, 0x401B
    .dc.w   0x1965, 0xA145, 0x6597, 0x50A2, 0x47D6, 0x43CE, 0x41E0, 0x40EE, 0x4076, 0x403B, 0x401D
    .dc.w   0x17F9, 0xAADC, 0x6892, 0x51CE, 0x485B, 0x440D, 0x41FE, 0x40FD, 0x407E, 0x403E, 0x401F
    .dc.w   0x16A0, 0xB504, 0x6BA2, 0x52FF, 0x48E1, 0x444C, 0x421D, 0x410C, 0x4085, 0x4042, 0x4021
    .dc.w   0x155B, 0xBFC8, 0x6EC9, 0x5434, 0x4969, 0x448B, 0x423B, 0x411B, 0x408D, 0x4046, 0x4023
    .dc.w   0x1428, 0xCB2F, 0x7208, 0x556E, 0x49F1, 0x44CA, 0x425A, 0x412A, 0x4094, 0x404A, 0x4025
    .dc.w   0x1306, 0xD744, 0x7560, 0x56AC, 0x4A7A, 0x450A, 0x4278, 0x4139, 0x409C, 0x404D, 0x4026
    .dc.w   0x11F5, 0xE411, 0x78D0, 0x57EE, 0x4B04, 0x454A, 0x4297, 0x4148, 0x40A3, 0x4051, 0x4028
    .dc.w   0x10F3, 0xF1A1, 0x7C5B, 0x5936, 0x4B8F, 0x458A, 0x42B6, 0x4157, 0x40AA, 0x4055, 0x402A
    .dc.w   0x1000, 0xFFFF, 0x8000, 0x5A82, 0x4C1B, 0x45CA, 0x42D5, 0x4166, 0x40B2, 0x402C, 0x4058





.global calc_exp

/*
 *    calc_exp ピッチベンド値の計算
 *    calc_exp( uint16_t num, uint8_t range)
 *    num:  pitchbend 14bitを(計算は上位10bit )
 *    range:  0-24
 *
 *    num r25, r24    range (r23), r22
 */


.func calc_exp

calc_exp:

  push  r1
  push  ZERO

  clr   ZERO
  tst   r22
  brne  conv
  ldi   EXP_L,0
  ldi   EXP_H,0x40
  jmp   return

conv:
  dec     r22
  ldi   R23,22    ; &(expon_tbl[num][0]) テーブルのアドレス計算 
  mul   r22,R23
  movw  TBLP_L,r0
  subi  TBLP_L,lo8(-(expon_tbl)) ;set Z pinter
  sbci  TBLP_H,hi8(-(expon_tbl))  

//pow(2;-step)load-------------------------------
  
  lpm AL,Z+
  lpm AH,Z+
    
//mul^512------------------------

  lpm BL,Z+ ;3
  lpm BH,Z+ ;6

  SBRS  EXP_H,5 ;8
  rjmp  next512       

  mul AL,BL ;10
  movw CL,r0  ;12
  mul AH,BH ;14
  movw CMH,r0 ;16
  mul AL,BH ;18
  add CML,r0  ;19
  adc CMH,r1  ;20
  adc CH,ZERO ;21
  mul AH,BL ;23
  add CML,r0  ;24
  adc CMH,r1  ;25
  adc CH,ZERO ;26

  lsl CML   ;27
  rol CMH   ;28
  rol CH    ;29
  lsl CML   ;30
  rol CMH   ;31
  rol CH    ;32
  movw AL,CMH ;34

next512:




//mul^256-------------------------------------

  lpm BL,Z+   
  lpm BH,Z+   

  SBRS EXP_H,4  
  rjmp  next256   

  mul AL,BL 
  movw CL,r0  
  mul AH,BH 
  movw CMH,r0 
  mul AL,BH 
  add CML,r0  
  adc CMH,r1  
  adc CH,ZERO 
  mul AH,BL 
  add CML,r0  
  adc CMH,r1  
  adc CH,ZERO 
  lsl CML   
  rol CMH   
  rol CH    
  lsl CML   
  rol CMH   
  rol CH    
  movw AL,CMH
        
next256:


//mul^128----------------------------

  
  lpm BL,Z+ 
  lpm BH,Z+ 

  SBRS  EXP_H,3
  rjmp  next128 
  
  mul AL,BL 
  movw CL,r0  
  mul AH,BH 
  movw CMH,r0 
  mul AL,BH 
  add CML,r0
  adc CMH,r1  
  adc CH,ZERO 
  mul AH,BL 
  add CML,r0  
  adc CMH,r1  
  adc CH,ZERO
  lsl CML   
  rol CMH 
  rol CH    
  lsl CML   
  rol CMH   
  rol CH    
  movw AL,CMH 

next128:


//mul^64---------------------------

  lpm BL,Z+   
  lpm BH,Z+ 
    
  SBRS  EXP_H,2
  rjmp  next64  

  mul AL,BL 
  movw CL,r0  
  mul AH,BH 
  movw CMH,r0 
  mul AL,BH 
  add CML,r0  
  adc CMH,r1  
  adc CH,ZERO 
  mul AH,BL 
  add CML,r0  
  adc CMH,r1  
  adc CH,ZERO 
  lsl CML   
  rol CMH   
  rol CH    
  lsl CML   
  rol CMH   
  rol CH    
  movw AL,CMH 

next64:


//mul^32------------------------------

  lpm BL,Z+   
  lpm BH,Z+

  SBRS  EXP_H,1
  rjmp  next32  

  mul AL,BL 
  movw CL,r0  
  mul AH,BH 
  movw CMH,r0 
  mul AL,BH 
  add CML,r0  
  adc CMH,r1  
  adc CH,ZERO 
  mul AH,BL 
  add CML,r0  
  adc CMH,r1  
  adc CH,ZERO 
  lsl CML   
  rol CMH   
  rol CH    
  lsl CML   
  rol CMH   
  rol CH    
  movw AL,CMH 

next32:


//mul^16-----------------------------

  lpm BL,Z+ 
  lpm BH,Z+   

  SBRS  EXP_H,0
  rjmp  next16  

  mul AL,BL 
  movw CL,r0  
  mul AH,BH 
  movw CMH,r0 
  mul AL,BH 
  add CML,r0  
  adc CMH,r1  
  adc CH,ZERO 
  mul AH,BL 
  add CML,r0  
  adc CMH,r1  
  adc CH,ZERO 
  lsl CML   
  rol CMH   
  rol CH    
  lsl CML   
  rol CMH   
  rol CH    
  movw AL,CMH 

next16:



//mul^8-----------------------------

  lpm BL,Z+ 
  lpm BH,Z+   

  SBRS  EXP_L,7
  rjmp  next8

  mul AL,BL 
  movw CL,r0  
  mul AH,BH 
  movw CMH,r0 
  mul AL,BH 
  add CML,r0  
  adc CMH,r1  
  adc CH,ZERO 
  mul AH,BL 
  add CML,r0  
  adc CMH,r1  
  adc CH,ZERO 
  lsl CML   
  rol CMH   
  rol CH    
  lsl CML   
  rol CMH   
  rol CH    
  movw AL,CMH 

next8:



//mul^4-------------------------

  lpm BL,Z+ 
  lpm BH,Z+   

  SBRS  EXP_L,6
  rjmp  next4
    
  mul AL,BL 
  movw CL,r0  
  mul AH,BH 
  movw CMH,r0 
  mul AL,BH 
  add CML,r0
  adc CMH,r1  
  adc CH,ZERO 
  mul AH,BL 
  add CML,r0  
  adc CMH,r1  
  adc CH,ZERO 
  lsl CML   
  rol CMH   
  rol CH    
  lsl CML   
  rol CMH   
  rol CH    
  movw AL,CMH 
next4:  

//mul^2-----------------------------

  lpm BL,Z+ 
  lpm BH,Z+   

  SBRS  EXP_L,5
  rjmp  next2

  mul AL,BL 
  movw CL,r0  
  mul AH,BH 
  movw CMH,r0 
  mul AL,BH 
  add CML,r0  
  adc CMH,r1  
  adc CH,ZERO 
  mul AH,BL 
  add CML,r0  
  adc CMH,r1  
  adc CH,ZERO 
  lsl CML   
  rol CMH   
  rol CH    
  lsl CML   
  rol CMH   
  rol CH    
  movw AL,CMH 

next2:



//mul^1-----------------------------

  lpm BL,Z+
  lpm BH,Z+
    
  SBRS  EXP_L,4
  rjmp  next1 
  
    
  mul AL,BL 
  movw CL,r0  
  mul AH,BH 
  movw CMH,r0 
  mul AL,BH 
  add CML,r0  
  adc CMH,r1  
  adc CH,ZERO 
  mul AH,BL 
  add CML,r0  
  adc CMH,r1  
  adc CH,ZERO 
  lsl CML   
  rol CMH   
  rol CH    
  lsl CML   
  rol CMH   
  rol CH    
  movw AL,CMH 

next1:


  movw EXP_L,AL

return:
  pop ZERO
  pop r1
  ret

.endfunc



