/*
 * Managetone.h
 *
 * Created: 2017/10/08 1:05:18
 *  Author: Keiji
 */ 

#include<avr/io.h>
#include "suart.h"
#include "xprintf.h"
#ifndef MANAGETONE_H_
#define MANAGETONE_H_

#define NOT_GET 255
#define HOLD	128

void init_midich(void);
int get_voice(uint8_t,uint8_t,uint8_t);
int return_voice(uint8_t,uint8_t);
void if_s_write(uint8_t,uint8_t);
void if_s_rwrite(uint8_t,uint8_t);
void if_s_lwrite(uint8_t,uint8_t);
uint16_t calc_exp(uint16_t,uint8_t);
void note_on(uint8_t,uint8_t,uint8_t,uint8_t,uint8_t);
void note_off(uint8_t);
void mute(uint8_t);
void all_note_off(void);

struct VoiceChannel{
	uint8_t voice_ch;
	uint8_t voice_no;
	uint8_t note_no;
	uint8_t midi_ch;
	uint8_t velocity;
	uint8_t hold;
	struct VoiceChannel *next;
	uint8_t release_cnt;

};







struct MidiCH{
	uint8_t voice_no;
	uint8_t hold;
	//uint16_t pitchbend;

  uint8_t s_modulation_pitch;
  uint8_t s_modulation_depth;
  uint8_t s_modulation_sintbl_pitch;
  uint8_t s_modulation_sintbl_ofs;
 
	uint8_t modulation;
	uint8_t partlevel;
	uint8_t expression;

	uint8_t pitch_sens;
	uint8_t reg_12;	//0x0c
	uint8_t reg_16;	//0x10

	uint8_t reg_17;	//0x11
	uint8_t reg_18;	//0x12
	uint8_t reg_19;	//0x13
	
	struct VoiceChannel *voice_list;
	
};

extern struct MidiCH midi_ch[];
extern struct VoiceChannel  ym825_voice_ch[];
extern uint8_t rel_optval[];


#endif /* MANAGETONE_H_ */





