
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "Midicommand.h"


#define DATA_LEN_825	30

/* offsets for SBI params */
#define AM_VIB		0
#define KSL_LEVEL	2
#define ATTACK_DECAY	4
#define SUSTAIN_RELEASE	6
#define WAVE_SELECT	8

/* offset for SBI instrument */
#define CONNECTION	10
#define OFFSET_4OP	11
#define MAX_MUL	8


#define FOSC 16000000UL
//#define BAUD FOSC
//#define UBRR_DAT (FOSC/(16*BAUD)-1)
void usart_spi_init(void);
void usart_spi_send(unsigned int data);


extern uint8_t tone_reg[480];


//#define MAX_3MUL 3

#include <avr/pgmspace.h>
#include "Managetone.h"
#include <avr/io.h>
#include <util/delay.h>
#include "wave.h"
#include "divtable.h"

#include <avr/interrupt.h>
//#include "sintbl.h"
#include "controlLine.h"






uint8_t pan_level[32] = {27, 27, 28, 28, 29, 29, 30, 30, 30, 31, 31, 30, 29, 29, 29, 28,
                         26, 25, 24, 23, 21, 18, 17, 14, 12, 11, 9, 7, 6, 4, 2, 0
                        };



//uint8_t pan_level[32] = {28,28,29,29,30,30,31,31,31,30,30,29,28,28,28,27,
//26,25,24,23,21,18,17,14,12,11,9,7,6,4,2,0};




//char MAX_3MUL = 8;
int eeprom_no = 0;				//Selected channel
int eeprom_p_midi = 0;			//eeprom read pointer;
char channelVal;				// Channel value



//char  modulation_depth[16];	//modulation
//char  modulation_pitch[16];	//modulation
//char  modulation_cnt[16];    //modulation
//char  modulation_tblpointer[16]; //sin

uint8_t rpn_msb[16];
uint8_t rpn_lsb[16];

//char  sin_pointer[16];
//char  sin_pitch[16];
//char  sin_tbl_offs[16];


uint8_t play_mode = 1;			// state of channel 1 [mono,mul,hum]
uint8_t hid_res_mode = 0;
uint16_t flash_squ = 0;



char bank = 0;


char send_cnt = 0;

void optimize_queue(void);
void mem_reset(void);
void setChannelDefault();


void if_s_write(uint8_t addr, uint8_t data);


void note_on(uint8_t, uint8_t , uint8_t , uint8_t , uint8_t);

void pitch_wheel_change(char , char  , char );
void startup_sound(void);



extern uint16_t calc_exp(uint16_t, uint8_t);


void note_off_func(int, char);
void set_timer_intrupt(void);
void reset_ymf825(void);

void check_midimessage();

enum {Atck, Decy, Sus, Rel, Mul, Tlv, Ksl, Wave, Am, Vib, Egt, Ksr, FeedBk, Connect,
      D1R, D2R, D1L, DT1, DT2
     };

char Data[4];



void other(void) {


  int led_cnt;
  led_cnt++;
  if (led_cnt == 400) {
    led_cnt = 0;
    optimize_queue();

  }
}



void midi_command(uint8_t command, uint8_t midi_data1, uint8_t midi_data2, uint8_t midi_data3)
{

  int com;

  uint8_t i, j, l;

  uint8_t k, m;
  uint8_t a, c;

  int f;
  int ch;
  int adr;



  switch (command) {

    case MIDI_NOTE_ON:

      PORTD ^= 0x04;
      ch = midi_data1 & 0x0f;
      //  midi_data2 = midi_data2;
      //  midi_data3 = midi_data3;
      if (midi_data3 == 0) {
        note_off_func(ch, midi_data2);

        //break;
      } else {


        if (play_mode == 1) {

          f = get_voice(ch, midi_data2, midi_data3);
          note_on(ch, f, midi_data2, midi_data3, midi_ch[ch].voice_no);


        } else if (play_mode == 3) {

          if (ch < 8) {
            f = get_voice(ch, midi_data2, midi_data3);

            note_on(ch, f, midi_data2, midi_data3, midi_ch[ch].voice_no);
            f = get_voice(ch + 8, midi_data2, midi_data3);
            note_on(ch + 8, f, midi_data2, midi_data3, midi_ch[ch + 8].voice_no);
          }
        } else {
          note_on(ch, ch, midi_data2, midi_data3, midi_ch[ch].voice_no);
        }
      }

      break;


    case MIDI_NOTE_OFF:

      ch = midi_data1 & 0x0f;

      note_off_func(ch, midi_data2);
      break;


    case MIDI_PITCH_BEND:

      ch = midi_data1 & 0x0f;


      if (play_mode == 1) {
        change_pitchbend(ch, midi_data3, midi_data2);

      } else if (play_mode == 3) {
        if (ch < 8) {
          change_pitchbend(ch, midi_data3, midi_data2);
          change_pitchbend(ch + 8, midi_data3, midi_data2);
        }
      } else {
        pitch_wheel_change(ch, midi_data3 , midi_data2);

      }
      break;


    case 	MIDI_CONTROL_CHANGE:


      ch = midi_data1 & 0x0f;
      switch (midi_data2) {

        case 11:			//

          m = (midi_data3 >> 2);
          //channelExpression[ch] = m;
          midi_ch[ch].expression = m;
          //m = pgm_read_byte(&(divtbl[(int)m][channelPartLevel[(int)ch]]));
          m = pgm_read_byte(&(divtbl[(int)m][(int)midi_ch[(int)ch].partlevel]));
          k = m << 2;
          //k = m;
          if ( play_mode == 1 ) {
            change_expression(ch, k);
          } else if (play_mode == 3) {
            if (ch < 8) {
              change_expression(ch, k);
              change_expression(ch + 8, k);
            }


          } else {
            if_s_write(0x0B, ch);
            if_s_write(0x10, k);

          }
          break;

        case 1:  //

          midi_data3 = midi_data3 >> 4;
          //modulation_depth[ch] = j;

          //modulation_cnt[ch] = modulation_pitch[ch];
          if ( play_mode == 1  ) {
            change_modulation(ch, midi_data3);


          } else if (play_mode == 3) {
            if (ch < 8) {
              change_modulation(ch, midi_data3);
              change_modulation(ch + 8, midi_data3);
            }
          } else {
            if_s_write(0x0B, ch);
            if_s_write(0x11, midi_data3);
          }
          break;

        case 7:		//

          m = midi_data3 >> 2;
          //channelPartLevel[ch] = m;
          //m = m >> 1;


          midi_ch[ch].partlevel = m;


          m = pgm_read_byte(&(divtbl[(int)m][(int) midi_ch[(int)ch].expression]));
          k = m << 2;
          //k = m;
          if ( play_mode == 1  ) {

            change_part_level(ch, k);

          } else if (play_mode == 3) {
            if (ch < 8) {
              change_part_level(ch, k);
              change_part_level(ch + 8, k);
            }
          } else {
            if_s_write(0x0B, ch);
            if_s_write(0x10, k);

          }
          break;

        case	101:	// RPN MSB

          rpn_msb[ch] = midi_data3;
          break;

        case	100:	//RPN LSB

          rpn_lsb[ch] = midi_data3;

        case	6:		//Pitch Bend Sense

          if ((rpn_msb[ch] == 0) && (rpn_lsb[ch] == 0)) {
            midi_ch[ch].pitch_sens = midi_data3;
            if (play_mode == 3) {
              if (ch < 8) {
                midi_ch[ch + 8].pitch_sens = midi_data3;
              }
            }
          }
          break;

        case 121:	//

          break;
          // setChannelDefault();
          break;

        case	64:			//hold

          if (j < 64) {
            // hold off
            hold_off(ch);
          } else {
            //hold on
            hold_on(ch);
          }
          break;

        case	0:				//

          //j = j & 0x01;
          //j = j ^ 0x01;
          bank = midi_data3 & 0x03;
          break;

        case	10:				//

          m = midi_data3 >> 2;
          midi_ch[ch].panpot_L = pan_level[m];
          midi_ch[ch].panpot_R = pan_level[31 - m];
          midi_ch[ch].panpot_L = 31;
          midi_ch[ch].panpot_R = 31;

          //

          //m =  j >> 3;
          //midi_ch[ch].panpot_L = 16+m;
          //midi_ch[ch].panpot_R = 31-m;
          //



          //m = midi_ch[ch].partlevel;
          //m = m << 2;
          //m = pgm_read_byte(&(divtbl[(int)m][(int) midi_ch[(int)ch].expression]));
          //change_part_level(ch,m);

          break;
        /*
        					case 59:	//if(i == 59){		//software modulation depth
        						j = ((j & 0xfc)>>1);
        						modulation_depth[ch] = j;
        						if(play_mode == 3){
        							modulation_depth[ch+8] = j;
        						}
        						break;

        					case 60://if(i == 60){
        						for(l = 0;l<channelVal;l++){
        							sin_pitch[l] = j;
        						}
        						break;

        					case 61://if(i == 61){
        						for(l = 0;l<channelVal;l++){
        							sin_tbl_offs[(int)l] = (j<<5) & 0xe0;
        						}
        						break;

        					case 31://if(i == 31){ //modulation sin table pitch
        						sin_pitch[ch] = j ;
        						if( play_mode == 3   ){
        							sin_pitch[ch+8] = j;
        						}
        						break;

        					case 32: //if(i == 32){ //modulation sin table pitch
        						sin_tbl_offs[ch] = (j<<5) & 0xe0;
        						if( play_mode == 3   ){
        							sin_tbl_offs[ch+8] = (j<<5) & 0xe0;
        						}
        						break;

        					case 76: //if(i == 76){  // �ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽW�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ[�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽV�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｿ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｿ�ｽｽ�ｿｽ�ｽｽ�ｽｽ�ｿｽ�ｽｽ�ｽｽpitch
        						modulation_pitch[ch] = j ;
        						modulation_cnt[ch] = modulation_pitch[ch];
        						if( play_mode == 3  ){

        							modulation_pitch[ch+8] = j;
        							modulation_cnt[ch+8] = modulation_pitch[ch+1];
        						}
        						break;
        */

        default:
          break;
      }		//controll change swich end



      break;


    case	MIDI_PROGRAM_CHANGE:

      ch = midi_data1 & 0x0f;

      //f = midi_data2 << 5;  //  f = j * 32;
      l = midi_data2 & 0x0f;

      if (bank == 1) {
        midi_ch[ch].voice_no = ch;
        /* Yamaha midi*/

        f = midi_data2 * 30;
        adr = ch * 30;

        for (i = 0; i < 30; i++) {
          tone_reg[adr + i] = pgm_read_byte(&(wave825[(int)i + f]));
        }

        write_burst();

      } else if (bank == 2) {
        midi_data2  = midi_data2 & 0x1f;
        f = midi_data2 << 5;		//*32
        adr = ch * 30;
        for (i = 0; i < 30; i++) {
          eeprom_busy_wait();
          tone_reg[adr + i] = eeprom_read_byte((uint8_t *)(f + i));
        }
        write_burst();

      } else {
        //channelVoiceNo[ch] = l;
        midi_ch[ch].voice_no = l;

      }
      break;
defalut:
      break;
  }
}

void midi_sysEx(uint8_t * sysex_mes, uint8_t dat_len) {

  uint8_t a, c;
  uint8_t j;
  int f;

  // uint8_t i, j, l;
  uint8_t l;
  //uint8_t k, m;


  // int f;
  // int ch;
  int adr;




  //  if ((sysex_mes[0] == 0xf0) && (sysex_mes[1] == 0x00)) {
  //   if (sysex_mes[1] != 0x43 ) {
  if (dat_len < 8) {
   
    Data[0] = sysex_mes[1];
    Data[1] = sysex_mes[2];
    Data[2] = sysex_mes[3];
    Data[3] = sysex_mes[4];
    l = sysex_mes[5];

    if (l & 0x04) {
      Data[1] += 0x80;
    }
    if (l & 0x02) {
      Data[2] += 0x80;
    }
    if (l & 0x01) {
      Data[3] += 0x80;
    }
    switch (Data[0]) {
      case 0:
        reset_ymf825();
        _delay_ms(10);
        setChannelDefault();
        init_midich();
        optimize_queue();
        bank = 1;
        break;

      case 1:

        if_s_write(Data[2], Data[3]);
        break;


      case 2:   // set pori mode midi channel 1 and 4

        if (Data[1] == 0) {
          play_mode = 0;

          setChannelDefault();
          mem_reset();

        } else if (Data[1] == 1) {
          play_mode = 1;
          for (a = 0; a < 16; a++) {
            midi_ch[a].voice_no = a;

          }
          setChannelDefault();
          mem_reset();

        } else if (Data[1] == 3) {
          play_mode = 3;
          //MAX_3MUL = 8;
          setChannelDefault();
          mem_reset();
        }
        break;



      case 9:
        write_burst();

        break;
      case 10:
        tone_reg[Data[1] * 30 + Data[2] ] = Data[3];
        write_burst();
        PORTD ^= 0x08;

        break;
      case 11:

        tone_reg[Data[1] * 30 + Data[2]] = Data[3];
        break;

      default:
        break;

    }


  } else if ( (sysex_mes[1] == 0x43) && (sysex_mes[2] == 0x7f) &&
              (sysex_mes[3] == 0x02) && (sysex_mes[4] == 0x00) && (sysex_mes[5] == 0x00)) {

    adr = (sysex_mes[6] & 0x0f) * 30; //tone_reg top adr of tone no

    a = sysex_mes[7];	//voice common
    tone_reg[adr + 0] = (a & 0x60) >> 5;
    tone_reg[adr + 1] = (a & 0x18) << 3;
    tone_reg[adr + 1] |= (a & 0x07);
    c = 8;
    for (j = 0; j < 4; j++) {
      f = j * 7 + adr;
      a = sysex_mes[c++];				//key control
      tone_reg[f + 8] = (a & 0x70) >> 4;	//8 first
      tone_reg[f + 2] = (a & 0x08);		//2 first
      tone_reg[f + 2] |= (a & 0x04) >> 2;
      tone_reg[f + 5] = (a & 0x03);		//5 first

      a = sysex_mes[c++];				//Attck Rate
      tone_reg[f + 4] = (a & 0x0f) << 4;	//4 first

      a = sysex_mes[c++];				//Decay Rate
      tone_reg[f + 3] = (a & 0x0f);		//3 first

      a = sysex_mes[c++];				//Sustain Rate
      tone_reg[f + 2] |= (a & 0x0f) << 4;

      a =  sysex_mes[c++];			//Release Rate
      tone_reg[f + 3] |= (a & 0x0f) << 4;

      a =  sysex_mes[c++];			//Sustain Level
      tone_reg[f + 4] |= (a & 0x0f);

      a =  sysex_mes[c++];			//Total Level
      tone_reg[f + 5] |= (a & 0x3f) << 2;

      a =  sysex_mes[c++];			//Modulation
      tone_reg[f + 6] = a;				//6 first

      a =  sysex_mes[c++];			//Pitch
      tone_reg[f + 7] = (a & 0x0f) << 4;	//7 first
      tone_reg[f + 7] |= (a & 0x70) >> 4;

      a =  sysex_mes[c++];			//Wave Shape
      tone_reg[f + 8] |= (a & 0x1f) << 3;



    }
    write_burst();

  }



}



//		check_midimessage(); //







/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
  int i, ch;



  /* Hardware Initialization */
  CLKPR = 0x80;
  CLKPR = 0x00;


  _delay_ms(50);
  //usart_spi_init();
  _delay_ms(50);
  reset_ymf825();
  _delay_ms(10);

  startup_sound();

  setChannelDefault();
  //set_timer_intrupt();
}
void keyon(unsigned char fnumh, unsigned char fnuml) {
  if_s_write( 0x0B, 0x00 );//voice num
  if_s_write( 0x0C, 0x54 );//vovol
  if_s_write( 0x0D, fnumh );//fnum
  if_s_write( 0x0E, fnuml );//fnum
  if_s_write( 0x0F, 0x40 );//keyon = 1

}

void keyoff(void) {
  if_s_write( 0x0F, 0x00 );//keyon = 0
}




void setChannelDefault() {
  char reg;
  int i, j, val;
  val = 16;

  for (i = 0; i < 16; i++) {
    //rpn_msb[i] = 0;
    //rpn_lsb[i] = 0;

    rpn_msb[i] = 127;
    rpn_lsb[i] = 127;

    //modulation_depth[i] = 20;
    //modulation_pitch[i] = 21;	//modulation

    //modulation_cnt[i] = 0;    //modulation


    //modulation_tblpointer[i] = 0;
    //sin_pointer[i] = 0;
    //sin_pitch[i] = 3;
    //sin_tbl_offs[i] = (7<< 5);
  }

  init_midich();
  channelVal = val;

}






void mem_reset(void) {
  init_midich();
}


void set_timer_intrupt(void) {
  OCR1A = 0x18;
  //OCR1A = 0x06;
  TCCR1A  = 0b00000000;
  TIMSK1 = (1 << OCIE1A); //compare match A interrupt

  TCCR1B  = (1 << WGM12);
  //TCCR1B |= (1 << CS12)|(1 << CS10);  // CTC mode top = OCR1A

  TCCR1B |= (1 << CS12);  // CTC mode top = OCR1A


  sei();
}

//#define USE_C_INTRUPT

#ifdef USE_C_INTRUPT
ISR(TIMER1_COMPA_vect) {

  uint8_t i, c, d, e;


  for (i = 0; i < channelVal; i++) {

    //c = modulation_cnt[i];

    if (c != 0) {
      c--;
      if (c == 0) {
        //PORTC ^= 0x80;
        //modulation_cnt[i] = modulation_pitch[i];
        //c = modulation_tblpointer[i];
        c++;
        c &= 0x01f;
        modulation_tblpointer[i] = c;
        d = modulation_depth[i];
        e = pgm_read_byte(&(sin_tbl[(int)d][(int)c]));

        e = e + midi_ch[ym825_voice_ch[i].midi_ch].reg_18; //PitchBend hi
        if_s_write(0x0b, i);
        if_s_write(0x12, e);
      } else {
        modulation_cnt[i] = c;
      }

    }

  }

}
#endif


void reset_ymf825() {

  int i;


  rs_hi();
  _delay_ms(100);
  rs_lo();
  _delay_ms(10);

  rs_hi();

  wr_hi();
  wr_hi();
  _delay_ms(100);

  if_s_write(0x1D, 0);	//5V

  if_s_write( 0x02, 0x0e );
  _delay_ms(1);
  if_s_write( 0x00, 0x01 );//CLKEN
  if_s_write( 0x01, 0x00 ); //AKRST
  if_s_write( 0x1A, 0xA3 );
  _delay_ms(1);
  if_s_write( 0x1A, 0x00 );
  _delay_ms(30);
  if_s_write( 0x02, 0x00 );
  //add
  //if_s_write( 0x19, 0xcc );//MASTER VOL
  if_s_write( 0x19, 0xac );//MASTER VOL
  if_s_write( 0x1B, 0x3F );//interpolation
  if_s_write(0x1b, 0x00);


  if_s_write( 0x14, 0x00 );//interpolation
  //if_s_write( 0x03, 0x03 );//Analog Gain
  if_s_write( 0x03, 0x01 );//Analog Gain
  if_s_write( 0x08, 0xF6 );
  //if_s_write( 0x08, 0xFf );
  flush_spi_buff();
  _delay_ms(21);
  if_s_write( 0x08, 0x00 );

  //if_s_write( 0x09, 0xF8 );
  if_s_write( 0x09, 0xd0 );
  if_s_write( 0x0b, 0x00 );

  if_s_write( 0x17, 0x40 );//MS_S
  if_s_write( 0x18, 0x00 );


  if_s_write(0x20, 0x0f);
  flush_spi_buff();
  _delay_us(10);

  if_s_write(0x21, 0x0f);
  flush_spi_buff();
  _delay_us(10);

  if_s_write(0x22, 0x0f);
  flush_spi_buff();
  _delay_us(10);



  for (i = 0; i < 16; i++) {

    if_s_write(0x0b, i);			//

    if_s_write(0x14, 0);
    tone_reg[(30 * i)] = 0x01;	//BO
    tone_reg[(30 * i) + 1] = 0x83;	//LFO,ALG

    tone_reg[(30 * i) + 2] = 0x00; //0:sr,lfo,ksr
    tone_reg[(30 * i) + 3] = 0x7F;	//1:RR,DR
    tone_reg[(30 * i) + 4] = 0xF4;	//2:ar,sl
    tone_reg[(30 * i) + 5] = 0xBB;	//3:TL,KSL
    tone_reg[(30 * i) + 6] = 0x00;	//4:DAM,EAM,DVB,EVB
    tone_reg[(30 * i) + 7] = 0x10;	//5:MUL,DT
    tone_reg[(30 * i) + 8] = 0x40;	//6:WS,FB

    tone_reg[(30 * i) + 9] = 0x00;
    tone_reg[(30 * i) + 10] = 0xAF;
    tone_reg[(30 * i) + 11] = 0xA0;
    tone_reg[(30 * i) + 12] = 0x0E;
    tone_reg[(30 * i) + 13] = 0x03;
    tone_reg[(30 * i) + 14] = 0x10;
    tone_reg[(30 * i) + 15] = 0x40;

    tone_reg[(30 * i) + 16] = 0x00;
    tone_reg[(30 * i) + 17] = 0x2F;
    tone_reg[(30 * i) + 18] = 0xF3;
    tone_reg[(30 * i) + 19] = 0x9B;
    tone_reg[(30 * i) + 20] = 0x00;
    tone_reg[(30 * i) + 21] = 0x20;
    tone_reg[(30 * i) + 22] = 0x41;

    tone_reg[(30 * i) + 23] = 0x00; //sr,xof,ks4r
    //tone_reg[(30*i)+24] = 0xAF;
    tone_reg[(30 * i) + 24] = 0x7F;
    tone_reg[(30 * i) + 25] = 0xA0;
    tone_reg[(30 * i) + 26] = 0x0E;
    tone_reg[(30 * i) + 27] = 0x01;
    tone_reg[(30 * i) + 28] = 0x10;
    tone_reg[(30 * i) + 29] = 0x40;

  }

  write_burst();




}

void startup_sound(void) {
  // _delay_ms(50);
  keyon(0x1c, 0x11);
  _delay_ms(30);
  keyoff();
  _delay_ms(10);

  keyon(0x1c, 0x42);
  _delay_ms(30);
  keyoff();
  _delay_ms(10);

  keyon(0x1c, 0x5d);
  _delay_ms(30);
  keyoff();
  _delay_ms(10);

  keyon(0x24, 0x17);
  _delay_ms(30);
  keyoff();
  // _delay_ms(10);


}

void usart_spi_init() {

  char c;

  //UBRR1 = 0;
  //UCSR1C = (1<<UMSEL11)|(1<<UMSEL10)|(0<< UCSZ10)|(0<<UCPOL1); //UCSZ10 = UCPHA1
  //UCSR1C = 0xc0;
  //UCSR1B = (1<<RXEN1)|(1<<TXEN1);
  //UCSR1B = (1<<RXEN1) | (1<<TXEN1) |  (1<<UDRIE1);
  //UBRR1 = UBRR_DAT;
  //UBRR1 = 40;
  SPCR = (1 << SPE) | (1 << MSTR) | (0 << SPR0) | (0 << SPR1) | (1 << SPIE);
  //		SPCR = (1<<SPE)|(1<<MSTR)|(0<<SPR0)|(0<<SPR1);

  //SPSR = (1<< SPI2X);	//8MH
  SPSR = 0;  //4Mh
  c = SPDR;
}
void usart_spi_send(unsigned int data) {
  while ( !(UCSR0A & (1 << UDRE0)));
  UDR0 = data;
  //while( !(UCSR1A & (1<<RXC1)));
  data = UDR0;
}


//void check_midimessage() {
/*
  MIDI_EventPacket_t MIDIEvent;
  uint8_t MIDImidi_data1;

  uint8_t dat1,dat2;
  if(send_cnt == 0)
	return;


  send_cnt = 30;
  MIDImidi_data1 = MIDI_midi_data1_SYSEX_START_3BYTE;
  dat1 = eeprom_read_byte( (uint8_t*)(eeprom_p_midi ++));
  MIDIEvent = (MIDI_EventPacket_t)
  {
	.Event       = MIDI_EVENT(1, MIDImidi_data1),

	.Data1       = 0xF0,
	.Data2       = (dat1 >> 4),
	.Data3       = dat1 & 0x0f,
  };
  send_cnt--;
  MIDI_Device_SendEventPacket(&Keyboard_MIDI_Interface, &MIDIEvent);

  do{

	dat1 = eeprom_read_byte( (uint8_t*)(eeprom_p_midi ++));
	dat2= eeprom_read_byte( (uint8_t*)(eeprom_p_midi ++));

	MIDImidi_data1 = MIDI_midi_data1_SYSEX_3BYTE;

	MIDIEvent = (MIDI_EventPacket_t)
	{
		.Event       = MIDI_EVENT(1, MIDImidi_data1),

		.Data1       = (dat1 >> 4),
		.Data2       = dat1 & 0x0f,
		.Data3       = (dat2 >> 4),
	};
	MIDI_Device_SendEventPacket(&Keyboard_MIDI_Interface, &MIDIEvent);

	dat1 = eeprom_read_byte( (uint8_t*)(eeprom_p_midi ++));
	MIDIEvent = (MIDI_EventPacket_t)
	{
		.Event       = MIDI_EVENT(1, MIDImidi_data1),

		.Data1       = dat2 & 0x0f,
		.Data2       = (dat1 >>4),
		.Data3       = dat1 & 0x0f,
	};
	MIDI_Device_SendEventPacket(&Keyboard_MIDI_Interface, &MIDIEvent);

	send_cnt = send_cnt -3;
  }while(send_cnt > 2);
  send_cnt = 0;

  dat1 = eeprom_read_byte( (uint8_t*)(eeprom_p_midi ++));
  dat2= eeprom_read_byte( (uint8_t*)(eeprom_p_midi ++));

  MIDIEvent = (MIDI_EventPacket_t)
  {
	.Event       = MIDI_EVENT(1, MIDImidi_data1),

	.Data1       = (dat1 >> 4),
	.Data2       = dat1 & 0x0f,
	.Data3       = (dat2 >> 4),
  };
  MIDI_Device_SendEventPacket(&Keyboard_MIDI_Interface, &MIDIEvent);

  MIDImidi_data1 = MIDI_midi_data1_SYSEX_END_3BYTE;
  MIDIEvent = (MIDI_EventPacket_t)
  {
	.Event       = MIDI_EVENT(1, MIDImidi_data1),

	.Data1       = dat2 & 0x0f,
	.Data2       = 0,
	.Data3       = 0xf7,
  };
  MIDI_Device_SendEventPacket(&Keyboard_MIDI_Interface, &MIDIEvent);
  MIDI_Device_Flush(&Keyboard_MIDI_Interface);

  //PORTC ^= 0x80;
*/
//}







