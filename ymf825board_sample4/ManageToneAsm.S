#include <avr/io.h>
.global fnum_hi_tbl
.global fnum_lo_tbl

.section .text
fnum_hi_tbl:
.dc.b  0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x18, 0x18
.dc.b  0x18, 0x18, 0x18, 0x20, 0x20, 0x20, 0x20, 0x28, 0x11, 0x11, 0x19, 0x19, 0x19, 0x19, 0x19, 0x21
.dc.b  0x21, 0x21, 0x21, 0x29, 0x12, 0x12, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0x22, 0x22, 0x22, 0x22, 0x2a
.dc.b  0x13, 0x13, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x23, 0x23, 0x23, 0x23, 0x2b, 0x14, 0x14, 0x1c, 0x1c
.dc.b  0x1c, 0x1c, 0x1c, 0x24, 0x24, 0x24, 0x24, 0x2c, 0x15, 0x15, 0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x25
.dc.b  0x25, 0x25, 0x25, 0x2d, 0x16, 0x16, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x26, 0x26, 0x26, 0x26, 0x2e
.dc.b  0x17, 0x17, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x27, 0x27, 0x27, 0x27, 0x2f, 0x10, 0x10, 0x18, 0x18
.dc.b  0x18, 0x18, 0x18, 0x20, 0x20, 0x20, 0x20, 0x28, 0x11, 0x11, 0x19, 0x19, 0x19, 0x19, 0x10, 0x1e


fnum_lo_tbl:
 .dc.b 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x7a, 0x11, 0x29
 .dc.b 0x42, 0x5d, 0x79, 0x17, 0x37, 0x59, 0x7d, 0x22, 0x65, 0x7a, 0x11, 0x29, 0x42, 0x5d, 0x79, 0x17
 .dc.b 0x37, 0x59, 0x7d, 0x22, 0x65, 0x7a, 0x11, 0x29, 0x42, 0x5d, 0x79, 0x17, 0x37, 0x59, 0x7d, 0x22
 .dc.b 0x65, 0x7a, 0x11, 0x29, 0x42, 0x5d, 0x79, 0x17, 0x37, 0x59, 0x7d, 0x22, 0x65, 0x7a, 0x11, 0x29
 .dc.b 0x42, 0x5d, 0x79, 0x17, 0x37, 0x59, 0x7d, 0x22, 0x65, 0x7a, 0x11, 0x29, 0x42, 0x5d, 0x79, 0x17
 .dc.b 0x37, 0x59, 0x7d, 0x22, 0x65, 0x7a, 0x11, 0x29, 0x42, 0x5d, 0x79, 0x17, 0x37, 0x59, 0x7d, 0x22
 .dc.b 0x65, 0x7a, 0x11, 0x29, 0x42, 0x5d, 0x79, 0x17, 0x37, 0x59, 0x7d, 0x22, 0x65, 0x7a, 0x11, 0x29
 .dc.b 0x42, 0x5d, 0x79, 0x17, 0x37, 0x59, 0x7d, 0x22, 0x65, 0x7a, 0x11, 0x29, 0x42, 0x5d, 0x65, 0x5d


/* 古典調律 */

//fnum_hi_tbl:
// .dc.b 0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x18,0x18
// .dc.b 0x18,0x18,0x18,0x20,0x20,0x20,0x28,0x28,0x11,0x11,0x19,0x19,0x19,0x19,0x19,0x21
// .dc.b 0x21,0x21,0x29,0x29,0x12,0x12,0x1a,0x1a,0x1a,0x1a,0x1a,0x22,0x22,0x22,0x2a,0x2a
// .dc.b 0x13,0x13,0x1b,0x1b,0x1b,0x1b,0x1b,0x23,0x23,0x23,0x2b,0x2b,0x14,0x14,0x1c,0x1c
// .dc.b 0x1c,0x1c,0x1c,0x24,0x24,0x24,0x2c,0x2c,0x15,0x15,0x1d,0x1d,0x1d,0x1d,0x1d,0x25
// .dc.b 0x25,0x25,0x2d,0x2d,0x16,0x16,0x1e,0x1e,0x1e,0x1e,0x1e,0x26,0x26,0x26,0x2e,0x2e
// .dc.b 0x17,0x17,0x1f,0x1f,0x1f,0x1f,0x1f,0x27,0x27,0x27,0x2f,0x2f,0x10,0x10,0x18,0x18
// .dc.b 0x18,0x18,0x18,0x20,0x20,0x20,0x28,0x28,0x11,0x11,0x19,0x19,0x19,0x19,0x10,0x26
//
//
//
///* 古典調律 */
//
//fnum_lo_tbl:
// .dc.b 0x68,0x68,0x68,0x68,0x68,0x68,0x68,0x68,0x68,0x68,0x68,0x68,0x68,0x7c,0x16,0x2b
// .dc.b 0x43,0x61,0x7b,0x1d,0x3a,0x59,0x01,0x24,0x68,0x7c,0x16,0x2b,0x43,0x61,0x7b,0x1d
// .dc.b 0x3a,0x59,0x01,0x24,0x68,0x7c,0x16,0x2b,0x43,0x61,0x7b,0x1d,0x3a,0x59,0x01,0x24
// .dc.b 0x68,0x7c,0x16,0x2b,0x43,0x61,0x7b,0x1d,0x3a,0x59,0x01,0x24,0x68,0x7c,0x16,0x2b
// .dc.b 0x43,0x61,0x7b,0x1d,0x3a,0x59,0x01,0x24,0x68,0x7c,0x16,0x2b,0x43,0x61,0x7b,0x1d
// .dc.b 0x3a,0x59,0x01,0x24,0x68,0x7c,0x16,0x2b,0x43,0x61,0x7b,0x1d,0x3a,0x59,0x01,0x24
// .dc.b 0x68,0x7c,0x16,0x2b,0x43,0x61,0x7b,0x1d,0x3a,0x59,0x01,0x24,0x68,0x7c,0x16,0x2b
// .dc.b 0x43,0x61,0x7b,0x1d,0x3a,0x59,0x01,0x24,0x68,0x7c,0x16,0x2b,0x43,0x61,0x68,0x3a





.extern midi_ch 
.extern fnum_hi_tbl 
.extern fnum_lo_tbl 
 .equ voice_no, 16  
 .equ velo,    18  
 .equ note_no, 20  
 .equ voicech, 22  
 .equ midich,  24  
 .equ XL,26 
 .equ XH,27 
 .equ ZL,30 
 .equ ZH,31 
 .equ WORK,25 
 .equ SPIWORK,21  


//
//  void note_on(uint8_t midich, uint8_t voicech, uint8_t note_no, uint8_t velo, uint8_t voice_no) {
//                        25,24           23,22             21,20          19,18   
.global note_on  
.func
note_on:



 cbi 0x5,2            //cs low
 ldi SPIWORK,0x0b      //  if_s_write(0x0b, voicech);
 out _SFR_IO_ADDR(SPDR),SPIWORK    
                          
                            //  channel_noteno[voicech] = note_no;      //mono
                      
                            mov XL,voicech                      ;1
                            ldi XH,0                            ;2
                            subi XL,lo8(-(channel_noteno))      ;3
                            sbci XH,hi8(-(channel_noteno))      ;4
                            st X,note_no                        ;6    
                          
                            //  velo = velo & 0x7c;
                            andi velo,0x7c                      ;7
                            
                            //   modulation_cnt[voicech] = midi_ch[midich].s_modulation_delay;
                            mov XL,voicech                      ;8
                            ldi XH,0                            ;9
                            subi XL,lo8(-(modulation_cnt))      ;10
                            sbci XH,hi8(-(modulation_cnt))      ;11
                          
                            ldi WORK,19                         ;12// size of struct midi_ch
                            mul WORK,midich                     ;14//Z <- midi_ch[midich]
                            movw ZL,r0                          ;15
                            subi ZL,lo8(-(midi_ch))             ;16
                            sbci ZH,hi8(-(midi_ch))             ;17
                            nop

 out _SFR_IO_ADDR(SPDR),voicech      

                            ldd  WORK,Z+18                      ;2
                            st   X,WORK                         ;4
                              
                            //   modulation_pitch[voicech] = midi_ch[midich].s_modulation_pitch;
                            mov XL,voicech                      ;5
                            ldi XH,0                            ;6
                            subi XL,lo8(-(modulation_pitch))    ;7
                            sbci XH,hi8(-(modulation_pitch))    ;8
                              
                            ldd  WORK,Z+2                       ;10//software_modulation_pitch
                            st  X,WORK                          ;11
                            
                            //  modulation_depth[voicech] = midi_ch[midich].s_modulation_depth;  
                            mov XL,voicech                      ;12
                            ldi XH,0                            ;13
                            subi XL,lo8(-(modulation_depth))    ;14
                            sbci XH,hi8(-(modulation_depth))    ;15
                            ldd  WORK,Z+3                       ;17//software_modulation_depth
                            nop
sbi 0x5,2             
ldi SPIWORK,0x0C     //  if_s_write(0x0C,velo);  // #12
cbi 0x5,2            //cs low
out _SFR_IO_ADDR(SPDR),SPIWORK   
        
                            st  X,WORK                          ;2
                            //  sin_pitch[voicech] = midi_ch[midich].s_modulation_sintbl_pitch;4
                            mov XL,voicech                      ;3
                            ldi XH,0                            ;4
                            subi XL,lo8(-(sin_pitch))           ;5
                            sbci XH,hi8(-(sin_pitch))           ;6
                            ldd  WORK,Z+4                       ;8//s_modulation_sintbl_pitch;
                            st  X,WORK                          ;10
                            //  sin_tbl_offs[voicech] = midi_ch[midich].s_modulation_sintbl_ofs;5
                            mov XL,voicech                      ;11
                            ldi XH,0                            ;12
                            subi XL,lo8(-(sin_tbl_offs))        ;13
                            sbci XH,hi8(-(sin_tbl_offs))        ;14
                            ldd  WORK,Z+5                       ;16//s_modulation_sintbl_offs;
                            st  X,WORK                          ;18
                       
                            //  sin_tbl_address[voicech] = (uint8_t *)midi_ch[midich].s_modulation_sintbl_addr;6,7

out _SFR_IO_ADDR(SPDR),velo     
      
                            mov XL,voicech                      ;1
                            add XL,XL                           ;2
                            ldi XH,0                            ;3
                            subi XL,lo8(-(sin_tbl_address))     ;4
                            sbci XH,hi8(-(sin_tbl_address))     ;5
                            ldd  WORK,Z+6                       ;7//s_modulation_sintbl_offs;
                            st  X+,WORK                         ;9
                            ldd  WORK,Z+7                       ;11//s_modulation_sintbl_offs;
                            st  X,WORK                          ;13
                       
                            //   modulation_tblpointer[(int)voicech] = (voicech & 0x3); // or 0x1f
                            mov XL,voicech                      ;14
                            ldi XH,0                            ;15
                            subi XL,lo8(-(modulation_tblpointer))  ;16
                            sbci XH,hi8(-(modulation_tblpointer))  ;17
                            nop
                      
sbi 0x5,2             
ldi SPIWORK,0x10    // if_s_write(0x10, midi_ch[midich].reg_16);      
cbi 0x5,2            //cs low
out _SFR_IO_ADDR(SPDR),SPIWORK  

                            mov WORK,voicech                    ;1
                            andi WORK,0x03                      ;2
                            st   X,WORK                         ;4
                            
                            //  sin_pointer[voicech] = 0; 
                            mov XL,voicech                      ;5
                            ldi XH,0                            ;6
                            subi XL,lo8(-(sin_pointer))         ;7
                            sbci XH,hi8(-(sin_pointer))         ;8
                            ldi WORK,0                          ;9
                            st   X,WORK                         ;11
                          
                            //ym825_voice_ch[voicech].release_cnt = rel_optval[voice_no];
                            mov XL,voice_no                     ;12
                            ldi XH,0                            ;13
                            subi XL,lo8(-(rel_optval))          ;14
                            sbci XH,hi8(-(rel_optval))          ;15
                            ldi WORK,9                          ;16// size of struct midi_ch
                            mul WORK,voicech                    ;18//Z <- midi_ch[midich]
                      
ldd SPIWORK,Z+12    
out _SFR_IO_ADDR(SPDR),SPIWORK  
      
                            movw ZL,r0                          ;1
                            subi ZL,lo8(-(ym825_voice_ch))      ;2   
                            sbci ZH,hi8(-(ym825_voice_ch))      ;3   
                            ld  WORK,X                          ;5
                            std  Z+8,WORK                       ;7
                            call wait_11clock                   ;17
sbi 0x5,2              
ldi SPIWORK,0x11    //  if_s_write(0x11, midi_ch[midich].reg_17);    
cbi 0x5,2            //cs low
out _SFR_IO_ADDR(SPDR),SPIWORK    
        call wait_18clock
ldd SPIWORK,Z+13    
out _SFR_IO_ADDR(SPDR),SPIWORK
        call wait_18clock
sbi 0x5,2                   
ldi SPIWORK,0x12    //  if_s_write(0x12, midi_ch[midich].reg_18);   
cbi 0x5,2            //cs low
out _SFR_IO_ADDR(SPDR),SPIWORK
       call wait_18clock
ldd SPIWORK,Z+14    
out _SFR_IO_ADDR(SPDR),SPIWORK        
       call wait_18clock 
sbi 0x5,2                
ldi SPIWORK,0x13     //  if_s_write(0x13, midi_ch[midich].reg_19);     
cbi 0x5,2            //cs low
out _SFR_IO_ADDR(SPDR),SPIWORK        
       call wait_18clock  
        
ldd SPIWORK,Z+15    
out _SFR_IO_ADDR(SPDR),SPIWORK        
       call wait_18clock
       
sbi 0x5,2                 
ldi SPIWORK,0x0D    //  if_s_write(0x0D, pgm_read_byte( &(fnum_hi_tbl[note_no])));   
cbi 0x5,2            //cs low
out _SFR_IO_ADDR(SPDR),SPIWORK    
mov ZL,note_no       
ldi ZH,0             
subi ZL,lo8(-(fnum_hi_tbl))   
sbci ZH,hi8(-(fnum_hi_tbl))   
lpm SPIWORK,Z 
call wait_11clock       

out _SFR_IO_ADDR(SPDR),SPIWORK     
          call wait_18clock
sbi 0x5,2                 
 
ldi SPIWORK,0x0E    //  if_s_write(0x0E, pgm_read_byte( &(fnum_lo_tbl[note_no]))); 

cbi 0x5,2            //cs low
out _SFR_IO_ADDR(SPDR),SPIWORK    
mov ZL,note_no       
ldi ZH,0             
subi ZL,lo8(-(fnum_lo_tbl))   
sbci ZH,hi8(-(fnum_lo_tbl))   
lpm SPIWORK,Z  
call wait_11clock      

out _SFR_IO_ADDR(SPDR),SPIWORK     
          call wait_18clock
sbi 0x5,2  
  
ldi SPIWORK,0x0F    //  if_s_write(0x0f, 0x40 | voice_no);

cbi 0x5,2            //cs low
out _SFR_IO_ADDR(SPDR),SPIWORK    
mov SPIWORK,voice_no 
ori SPIWORK,0x40
call wait_15clock     
 
out _SFR_IO_ADDR(SPDR),SPIWORK     
          call wait_18clock
          
sbi 0x5,2  
    clr r1
ret 
.endfunc
