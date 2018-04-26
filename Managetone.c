/*
   Managetone.c

   Created: 2017/10/07 23:28:28
    Author: Keiji
*/


/*

  MidiCH


  MidiCH[0]->voice[1]->voice[5]->voice[2]->NULL
  MidiCH[1]->NULL
  MidiCH[2]->voice_[4]->NULL
  MidiCH[3]->NULL
  .
  .
  .

*/

#include <avr/pgmspace.h>
#include "Managetone.h"
#include <avr/io.h>
#include <util/delay.h>
#define MAX_VOICE_NUM	16
#define FALSE 0
#define TRUE  1
#define NULL 0

struct VoiceChannel ym825_voice_ch[16];

struct MidiCH midi_ch[16];
extern uint8_t play_mode;
//extern char tone_reg[480];

//extern char  modulation_depth[16];	//modulation 
////extern char  modulation_pitch[16];	//modulation
//extern char  modulation_cnt[16];    //modulation
//extern char  modulation_tblpointer[16]; //sin

extern const char divtbl[32][32];
char  sin_pointer[16];
char  sin_pitch[16];
char  sin_tbl_offs[16];
uint8_t voice_queue[MAX_VOICE_NUM];
uint8_t voice_queue_top;
uint8_t voice_queue_tail;


void keyon(uint8_t,uint8_t);
void keyoff(void);




int		active_voice_num;
char channel_noteno[16];

uint8_t career_no[8][4] = { {1, 0, 0, 0},
  {0, 1, 0, 0},

  {0, 1, 2, 3},
  {3, 0, 0, 0},
  {3, 0, 0, 0},
  {1, 3, 0, 0},
  {0, 3, 0, 0},
  {0, 2, 3, 0}
};

uint8_t carrier_val[8] = {1, 2, 4, 2, 1, 2, 2, 3};
uint8_t rel_optval[16];



PROGMEM const uint8_t fnum_hi_tbl[128] = {
  0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x18, 0x18,
  0x18, 0x18, 0x18, 0x20, 0x20, 0x20, 0x20, 0x28, 0x11, 0x11, 0x19, 0x19, 0x19, 0x19, 0x19, 0x21,
  0x21, 0x21, 0x21, 0x29, 0x12, 0x12, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0x22, 0x22, 0x22, 0x22, 0x2a,
  0x13, 0x13, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x23, 0x23, 0x23, 0x23, 0x2b, 0x14, 0x14, 0x1c, 0x1c,
  0x1c, 0x1c, 0x1c, 0x24, 0x24, 0x24, 0x24, 0x2c, 0x15, 0x15, 0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x25,
  0x25, 0x25, 0x25, 0x2d, 0x16, 0x16, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x26, 0x26, 0x26, 0x26, 0x2e,
  0x17, 0x17, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x27, 0x27, 0x27, 0x27, 0x2f, 0x10, 0x10, 0x18, 0x18,
  0x18, 0x18, 0x18, 0x20, 0x20, 0x20, 0x20, 0x28, 0x11, 0x11, 0x19, 0x19, 0x19, 0x19, 0x10, 0x1e,

};
 PROGMEM const uint8_t fnum_lo_tbl[128]  = {
  0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65, 0x7a, 0x11, 0x29,
  0x42, 0x5d, 0x79, 0x17, 0x37, 0x59, 0x7d, 0x22, 0x65, 0x7a, 0x11, 0x29, 0x42, 0x5d, 0x79, 0x17,
  0x37, 0x59, 0x7d, 0x22, 0x65, 0x7a, 0x11, 0x29, 0x42, 0x5d, 0x79, 0x17, 0x37, 0x59, 0x7d, 0x22,
  0x65, 0x7a, 0x11, 0x29, 0x42, 0x5d, 0x79, 0x17, 0x37, 0x59, 0x7d, 0x22, 0x65, 0x7a, 0x11, 0x29,
  0x42, 0x5d, 0x79, 0x17, 0x37, 0x59, 0x7d, 0x22, 0x65, 0x7a, 0x11, 0x29, 0x42, 0x5d, 0x79, 0x17,
  0x37, 0x59, 0x7d, 0x22, 0x65, 0x7a, 0x11, 0x29, 0x42, 0x5d, 0x79, 0x17, 0x37, 0x59, 0x7d, 0x22,
  0x65, 0x7a, 0x11, 0x29, 0x42, 0x5d, 0x79, 0x17, 0x37, 0x59, 0x7d, 0x22, 0x65, 0x7a, 0x11, 0x29,
  0x42, 0x5d, 0x79, 0x17, 0x37, 0x59, 0x7d, 0x22, 0x65, 0x7a, 0x11, 0x29, 0x42, 0x5d, 0x65, 0x5d,
};



void init_midich(void) {
  int i;

  voice_queue_top = 0;
  voice_queue_tail = 0;
  active_voice_num = 0;



  for (i = 0; i < MAX_VOICE_NUM; i++) {
    ym825_voice_ch[i].voice_ch = i;		//voice channel no
    ym825_voice_ch[i].voice_no = i;		//voice no
    ym825_voice_ch[i].midi_ch = 127;	//midi channel no
    ym825_voice_ch[i].note_no = 255;	//midi 
    ym825_voice_ch[i].velocity = 32;
    ym825_voice_ch[i].next = NULL;
    ym825_voice_ch[i].release_cnt = 0;
    voice_queue[i] = i;				//

  }
  for (i = 0; i < 16; i++) {
    midi_ch[i].voice_no = i;		// MIDI_CH == voice_no
    midi_ch[i].hold	 = FALSE;
    midi_ch[i].pitchbend = 0x4000;
    midi_ch[i].modulation = 0;
    midi_ch[i].partlevel = 24;
    midi_ch[i].expression = 24;
    midi_ch[i].panpot_L = 25;
    midi_ch[i].panpot_R = 25;
    midi_ch[i].pitch_sens = 2;
    midi_ch[i].voice_list = NULL;	//
    /*-------------------------------note oｽ */
    midi_ch[i].reg_16 = 0x60;	//ch_vol,expression
    //midi_ch[i].reg_16R = 60;
    //midi_ch[i].reg_16L = 60;
    midi_ch[i].reg_17 = 0x00;	//vib
    midi_ch[i].reg_18 = 0x08;	//pitchbend_hi
    midi_ch[i].reg_19 = 0x00;	//pitchbend_lo
  }
}




int get_voice(uint8_t ch, uint8_t tone_no, uint8_t velo) {
  uint8_t voice_ch;
  struct VoiceChannel *p;



  //voice_ch = NOT_GET;
  p = midi_ch[ch].voice_list;
  while (p != NULL) {
    if (p->note_no == tone_no) {
      //if(p->hold == TRUE){

      voice_ch =  p->voice_ch;
      mute(voice_ch);
      //break;
      return voice_ch;
      //}
    }

    p = p->next;
  }
  //if(voice_ch != NOT_GET){
  //return voice_ch;
  //}


  if (active_voice_num == MAX_VOICE_NUM) {
    voice_ch = voice_queue_top;
    note_off(voice_ch);
    return voice_ch;
    //return NOT_GET;
  }
  voice_ch = voice_queue[voice_queue_top++];
  if (voice_queue_top == MAX_VOICE_NUM) {
    voice_queue_top = 0;
  }
  active_voice_num++;



  ym825_voice_ch[voice_ch].voice_no = midi_ch[ch].voice_no;
  ym825_voice_ch[voice_ch].note_no = tone_no;
  ym825_voice_ch[voice_ch].midi_ch = ch;
  ym825_voice_ch[voice_ch].velocity = velo;
  ym825_voice_ch[voice_ch].next = midi_ch[ch].voice_list;
  ym825_voice_ch[voice_ch].hold = FALSE;
  midi_ch[ch].voice_list = &(ym825_voice_ch[voice_ch]);

  return (int)voice_ch;
}


/*
	
*/
int  return_voice(uint8_t ch, uint8_t tone_no) {
  struct VoiceChannel **p;
  int voice_ch;
  //int i;


  p = &(midi_ch[ch].voice_list);
  //voice_ch = NOT_GET;

  while ( *p != NULL) {

    if ( (*p)->note_no == tone_no) {
      if ( midi_ch[ch].hold == TRUE) {
        (*p)->hold = TRUE;		//hol
       
    //break;
        return NOT_GET;
      }
      voice_ch = (*p)->voice_ch;
      *p = ((*p)->next);

      voice_queue[voice_queue_tail++] = voice_ch;
      if (voice_queue_tail == MAX_VOICE_NUM) {
        voice_queue_tail = 0;
      }
      active_voice_num--;
      //break;
      return voice_ch;
    }
    p = &( (*p)->next);
  }
  //return voice_ch;
  return NOT_GET;
}



void hold_on(uint8_t ch) {

  midi_ch[ch].hold = TRUE;


}


/*
 
*/


void hold_off(uint8_t ch) {
  struct VoiceChannel **p;
  int voice_ch;

  midi_ch[ch].hold = FALSE;

  p = &(midi_ch[ch].voice_list);

  while ( *p != NULL) {
    if ( (*p)->hold == TRUE) {
      voice_ch = (*p)->voice_ch;
      note_off(voice_ch);

      *p = ((*p)->next);

      voice_queue[voice_queue_tail++] = voice_ch;
      if (voice_queue_tail == MAX_VOICE_NUM) {
        voice_queue_tail = 0;
      }
      active_voice_num--;


    } else {
      p = &( (*p)->next);
    }
  }

}



void mute(uint8_t ch) {
  if_s_write(0x0b, ch);
  //if_s_write(0x0F, ym825_voice_ch[ch].voice_no+0x60);
  if_s_write(0x0F, ym825_voice_ch[ch].voice_no + 0x20);
}




void note_on(uint8_t midich, uint8_t voicech, uint8_t note_no, uint8_t velo, uint8_t voice_no) {


  uint8_t alg, k, l, m;
  uint16_t voice_top_addr;
  channel_noteno[voicech] = note_no;			//mono
  //modulation_tblpointer[(int)voicech] = 0; // or 0x1f
  //modulation_cnt[voicech] = modulation_pitch[voicech];
  //sin_pointer[voicech] = 0;


  velo = velo & 0x7C;  //  (velo >>2 ) << 2);
 // velo = velo >> 2;

  
 
  ym825_voice_ch[voicech].release_cnt = rel_optval[voice_no];




if_s_write(0x0b, voicech);
 


if_s_write(0x0C,velo);  // #12

/*
  if_s_rwrite( 0x0c, (pgm_read_byte( &(divtbl[velo][ midi_ch[midich].panpot_R ]))) << 2);
  if_s_lwrite( 0x0c, (pgm_read_byte( &(divtbl[velo][ midi_ch[midich].panpot_L ]))) << 2);
*/
/*
if_s_write(0x0D, fnum_hi_tbl[note_no]);
if_s_write(0x0E, fnum_lo_tbl[note_no]);
*/
 

  if_s_write(0x0D, pgm_read_byte( &(fnum_hi_tbl[note_no])));
  if_s_write(0x0E, pgm_read_byte( &(fnum_lo_tbl[note_no])));


  if_s_write(0x10, midi_ch[midich].reg_16);
  if_s_write(0x11, midi_ch[midich].reg_17);
  if_s_write(0x12, midi_ch[midich].reg_18);
  if_s_write(0x13, midi_ch[midich].reg_19);


  if_s_write(0x0f, 0x40 | voice_no);

 
}

void optimize_queue() {
  uint8_t min, v, i, j, k, p, pre;

  if (active_voice_num < MAX_VOICE_NUM - 1) {

    p = voice_queue_tail;
    if (p == 0) {
      p = MAX_VOICE_NUM;
    }
    p--;
    min = ym825_voice_ch[voice_queue[p]].release_cnt;

    while (p != voice_queue_top) {
      pre = p;
      if (p == 0) {
        p = MAX_VOICE_NUM;
      }
      p--;

      k = ym825_voice_ch[voice_queue[p]].release_cnt;
      if (k != 0)
        k--;
      ym825_voice_ch[voice_queue[p]].release_cnt = k;
      if (k < min) {
        min = k;
      } else {
        // swap
        k = voice_queue[p];
        voice_queue[p] = voice_queue[pre];
        voice_queue[pre] = k;
        /* no operation */
        //k = voice_queue[p];
        //voice_queue[p] = voice_queue[pre];
        //voice_queue[p] = k;

      }

    }
  }

}
void note_off(uint8_t voice_ch) {
  //setChannel(ch);
  if_s_write(0x0b, voice_ch);
  if_s_write(0x0F, ym825_voice_ch[voice_ch].voice_no);




}



void note_off_func(int ch, char i) {
  int f, adr;

  if (play_mode == 1) {
    f = return_voice(ch, i);
    if ( f != NOT_GET) {
      note_off(f);
    }

  } else if (play_mode == 3) {
    if (ch < 8) {
      f = return_voice(ch, i);
      if (f != NOT_GET) {
        note_off(f);
      }
      f = return_voice(ch + 8, i);
      if (f != NOT_GET) {
        note_off(f);
      }
    }
  } else {
    if (channel_noteno[ch] == i) {
      note_off(ch);
    }
  }

}



void change_modulation(uint8_t ch, uint8_t mod) {
  struct VoiceChannel *p;
  uint8_t voice_ch;

  midi_ch[ch].reg_17 = mod;
  p = midi_ch[ch].voice_list;
  while (p != NULL) {
    if_s_write(0x0b, p->voice_ch);
    if_s_write(0x11, mod);
    p = p->next;
  }
}

void change_pitchbend(uint8_t ch, uint8_t i, uint8_t j) {
  int f;
  int hi, lo;
  struct VoiceChannel *p;

  f = (i << 7) | j;
  f = calc_exp(f, midi_ch[ch].pitch_sens);

  hi = (f >> 11) & 0x001f;
  lo = (f >> 4) & 0x007f;
  midi_ch[ch].reg_18 = hi;
  midi_ch[ch].reg_19 = lo;

  p = midi_ch[ch].voice_list;
  while (p != NULL) {
    if_s_write(0x0b, p->voice_ch);
    if_s_write(0x12, hi);
    if_s_write(0x13, lo);
    p = p->next;
  }

}


void change_part_level(uint8_t ch, uint8_t val) {
  struct VoiceChannel *p;

  midi_ch[ch].reg_16 = val;


  p = midi_ch[ch].voice_list;
  while (p != NULL) {
    if_s_write(0x0b, p->voice_ch);
    if_s_write(0x10, val);
    p = p->next;
  }
}

void change_expression(uint8_t ch, uint8_t val) {
  struct VoiceChannel *p;


  midi_ch[ch].reg_16 = val;

  p = midi_ch[ch].voice_list;
  while (p != NULL) {

    if_s_write(0x0b, p->voice_ch);
    if_s_write(0x10, val);
    p = p->next;
  }
}




void pitch_wheel_change(char ch, char i , char j) {
  int f, d;


  f = (i << 7) | j;

  f = calc_exp(f, midi_ch[ch].pitch_sens);



  d = (f >> 11) & 0x001f;




  if_s_write(0x0B, ch);
  if_s_write(0x12, d);

  d = (f >> 4) & 0x007f;
  if_s_write(0x13, d);


}



