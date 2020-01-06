
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "Midicommand.h"
#include "sintbl.h"


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


uint8_t tone_reg[480];

extern const PROGMEM char divtbl[32][32] ;
//#define MAX_3MUL 3

#include <avr/pgmspace.h>
#include "Managetone.h"
#include <avr/io.h>
#include <util/delay.h>
#include "wave.h"
#include "divtable.h"

#include <avr/interrupt.h>
#include "sintbl.h"
#include "controlLine.h"







int eeprom_no = 0;				//Selected channel
char *readpoint_midi = 0;			//eeprom read pointer;
char channelVal;				// Channel value

extern char  modulation_depth[16];	//modulation
extern char  modulation_pitch[16];	//modulation
extern char  modulation_cnt[16];    //modulation
extern char  modulation_tblpointer[16]; //sin

uint8_t rpn_msb[16];
uint8_t rpn_lsb[16];

extern char  sin_pointer[16];
extern char  sin_pitch[16];
extern char  sin_tbl_offs[16];
extern uint8_t *sin_tbl_address[16];

uint8_t play_mode = 1;			// state of channel 1 [mono,mul,hum]
uint8_t hid_res_mode = 0;
uint16_t flash_squ = 0;

char bank = 0;
//char send_cnt = 0;

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
char Data[4];
  int com;

  uint8_t i, j, l;

  uint8_t k, m;
  uint8_t a, c;

  int f;
  int ch;
  int adr;



  switch (command) {

    case MIDI_NOTE_ON:


      ch = midi_data1 & 0x0f;
      if (midi_data3 == 0) {
        note_off_func(ch, midi_data2);
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
            if_chs_write(ch,0x10, k);

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
            if_chs_write(ch,0x11, midi_data3);
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
            if_chs_write(ch,0x10, k);

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
          all_note_off();
          init_midich();
          setChannelDefault();

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





          break;




          case 59:  //if(i == 59){    //software modulation depth

            midi_ch[ch].s_modulation_depth = midi_data3;

            break;        
          
          case 60://if(i == 60){
          
            for(l = 0;l<channelVal;l++){

              midi_ch[l].s_modulation_sintbl_pitch =midi_data3;
            }
            break;
            
          case 61://if(i == 61){
            
            for(l = 0;l<channelVal;l++){

              midi_ch[l].s_modulation_sintbl_ofs = midi_data3;
            }
            break;
          case 58: //all software moduration
            for(l = 0;l<channelVal;l++){
              midi_ch[l].s_modulation_depth = midi_data3;
            }
            break;
              
          case 31://if(i == 31){ //modulation sin table pitch

            midi_ch[ch].s_modulation_sintbl_pitch = midi_data3;
 
            break;
              
          case 32: //if(i == 32){ //modulation sin table pitch

            midi_ch[ch].s_modulation_sintbl_ofs = midi_data3;
 
            break;
            
          case 76: //if(i == 76){  // モジュレーションpitch

            //modulation_pitch[ch] = midi_data3 ;
            //modulation_cnt[ch] = modulation_pitch[ch];
            midi_ch[ch].s_modulation_pitch = midi_data3;

            break;
          
          case 62: // モジュレーション波形テーブルセレクト
            adr = (int)sin_tbl;
            if(midi_data3 == 1)
              adr = (int)tri_tbl;
            if(midi_data3 == 2)
              adr = (int)saw_tbl;

              midi_ch[ch].s_modulation_sintbl_addr = (char *)adr;
              break;

          case 63: //ソフトモジュレーションディレイ
            //midi_data3 <<= 2;
            if(midi_data3 == 0)
              midi_data3 = 1;

            midi_ch[ch].s_modulation_delay = midi_data3;
            break;  
        default:
          break;

          case 20:  //OP1 TL change
            change_totallevel(ch,0,midi_data3);
            
            break;
          
          case 21:  //OP2 TL change
            change_totallevel(ch,1,midi_data3);            
            
            break;
            
          case 22:  //OP3 TL Change
            change_totallevel(ch,2,midi_data3);            
            
            break;
                  
          case 52:  //OP1 DT Change
            change_detune(ch,0,midi_data3);
            break;
          
          case 53:  //OP2 DT Change
            change_detune(ch,1,midi_data3);
            break;
          case 54:  //OP3 DT Change
            change_detune(ch,2,midi_data3);
            break; 
          case 55:  //OP4 DT Change
            change_detune(ch,3,midi_data3);
            break;
          
          
          case 112: //OP1 Release Change
          change_release(ch,0,midi_data3);
          break;
          
          case 113: //OP2 Release Change
          change_release(ch,1,midi_data3);
          break;
          case 114: //OP3 Release Change
          change_release(ch,2,midi_data3);
          break;
          case 115: //OP4 Release Change
          change_release(ch,3,midi_data3);
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
  uint8_t Data[4];
  uint8_t a, c;
  uint8_t j;
  int f;
  int i;
  uint8_t l;
  int ch;
  int adr;


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
        cli();
        if_s_write(Data[2], Data[3]);
        sei();
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

      case 5: // set tone data  EEPROM  ;
        ch = (int)Data[1];
        f = (int)Data[2];
        c = Data[3];
        ch = ch << 5;
        f = f + ch;

        eeprom_busy_wait();
        eeprom_write_byte((uint8_t *)f, c);
        break;
        
      case 6: // read tone data from EEPROM
        readpoint_midi = (char *)(Data[1] << 5);
        for (f = 0; f < 30; f++) {
          c = eeprom_read_byte((uint8_t *)(readpoint_midi++));
          while (!(UCSR0A & (1 << UDRE0)))
            ;
          UDR0 = c;
          _delay_us(15);     //Atmega16U2の転送が間に合わないのでディレイを入れる

        }  
        break;

        case 7: //read tone data from toneMemory
        readpoint_midi = (char *)&(tone_reg[(int)Data[1]*30]);
        
        set_usartsendpoint(readpoint_midi);

        
//         for (f = 0; f < 30; f++) {
//          c = (*readpoint_midi++);
//          while (!(UCSR0A & (1 << UDRE0)))
//            ;
//          UDR0 = c;
//          _delay_us(15);     //Atmega16U2の転送が間に合わないのでディレイを入れる
//        } 
              
        break;


        case 8:
            i = Data[1];
              sysex_mes[0] = midi_ch[i].s_modulation_pitch;
              sysex_mes[1] = midi_ch[i].s_modulation_depth;
              sysex_mes[2] = midi_ch[i].s_modulation_sintbl_pitch;
              sysex_mes[3] = midi_ch[i].s_modulation_sintbl_ofs;
              sysex_mes[5] = midi_ch[i].s_modulation_delay;
              adr = (int)midi_ch[i].s_modulation_sintbl_addr;

            j = 0;
            if(adr ==  (int)tri_tbl)
              j = 1;
            if(adr ==  (int)saw_tbl)
              j =2;
            sysex_mes[4] = j;


            readpoint_midi = (char *)sysex_mes;
            set_usartsendpoint(readpoint_midi);

            
//           for (f = 0; f < 30; f++) {
//            c = (*readpoint_midi++);
//           while (!(UCSR0A & (1 << UDRE0)))
//             ;
//          UDR0 = c;
//          _delay_us(15);     //Atmega16U2の転送が間に合わないのでディレイを入れる
//
//           }

  
              break;
      case 9:
        write_burst();
        break;
        
      case 10:
        tone_reg[Data[1] * 30 + Data[2] ] = Data[3];
        write_burst();
        break;
        
      case 11:

        tone_reg[Data[1] * 30 + Data[2]] = Data[3];
        break;

      case 16:  //Note On
        f = get_voice(Data[1],Data[2],Data[3]);
        note_on(Data[1],f,Data[2],Data[3],midi_ch[Data[1]].voice_no);
        break;

      case 17:
        note_off_func(Data[1],Data[2]);
        break;

            case 18: //software modulation change
              midi_ch[(int)Data[1]].s_modulation_depth = Data[2];           
              break;
          
            case 19: //software modulation pitch change
              midi_ch[(int)Data[1]].s_modulation_sintbl_pitch = Data[2];              
              break;
              
            case 20: //software modulation depth change
              midi_ch[(int)Data[1]].s_modulation_sintbl_ofs = Data[2];          
              break;  
                          
            case 21: //software modulation table select
              switch(Data[2]){
                case 0:
                adr = (int)sin_tbl;
                  break;
                case 1:
                adr = (int)tri_tbl;
                  break;
                case 2:
                adr = (int)saw_tbl;
                  break;
                default:
                adr = (int)sin_tbl;
                
              }
              midi_ch[(int)Data[1]].s_modulation_sintbl_addr = (char *)adr; 
              break;
            case 22: //software modulation value
              midi_ch[(int)Data[1]].s_modulation_pitch = Data[2]; 
            
              break;
            case 23: //software modulation Delay set
              i = Data[2];
             // i <<= 2;
              if(i == 0)
                i = 1;
              midi_ch[(int)Data[1]].s_modulation_delay = i;
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


  DDRD |= 0x0c;


  /* usart setup */
  /**/
  UBRR0 = 0;
  UCSR0A |= (1 << U2X0);
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00) ;

  UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
 //UBRR0 = 0x0007;   //250,000
  //UBRR0 = 0x0022;   //57600
//  UBRR0 = 0x0010;   //115200
// UBRR0 = 0x0001;   //1,000,000
  UBRR0 = 0x0000;   //2,000,000

  SPCR = (1 << SPE) | (1 << MSTR) | (0 << SPR0) | (0 << SPR1);
  SPSR = (1 << SPI2X); //8MH
  PINB = 0xFF;
  PORTB = 0x0C;

  DDRB = 0x2C;
 // SPCR |= (1 << SPIE);

  _delay_ms(150);
  //usart_spi_init();
  _delay_ms(150);
  reset_ymf825();
  _delay_ms(100);

  startup_sound();

  setChannelDefault();
  set_timer_intrupt();

}
void keyon(unsigned char fnumh, unsigned char fnuml) {
  cli();
  if_s_write( 0x0B, 0x00 );//voice num
  if_s_write( 0x0C, 0x54 );//vovol
  if_s_write( 0x0D, fnumh );//fnum
  if_s_write( 0x0E, fnuml );//fnum
  if_s_write( 0x0F, 0x40 );//keyon = 1
  sei();
}

void keyoff(void) {
  cli();
  if_s_write( 0x0F, 0x00 );//keyon = 0
  sei();
}


void setChannelDefault() {
  char reg;
  int i, j, val;
  val = 16;

  for (i = 0; i < 16; i++) {

    rpn_msb[i] = 127;
    rpn_lsb[i] = 127;

    modulation_depth[i] = 3;
    modulation_pitch[i] = 40;	//modulation
   modulation_cnt[i] = 0;    //modulation

    modulation_tblpointer[i] = 0;
    sin_pointer[i] = 0;
    sin_pitch[i] = 1;
    sin_tbl_offs[i] = 5;
    sin_tbl_address[i] = (uint8_t *)sin_tbl;  
  }

  init_midich();
  channelVal = val;

}


void mem_reset(void) {
  init_midich();
}

void set_timer_intrupt(void) {
  OCR1A = 0x30;
  //OCR1A = 0x06;
  TCCR1A  = 0b00000000;
  //TIMSK1 = (1 << OCIE1A); //compare match A interrupt

  TCCR1B  = (1 << WGM12);
  TCCR1B |= (1 << CS12);  // CTC mode top = OCR1A


  sei();
}




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
  
  cli();  
  if_s_write(0x1D,0); //5V  
  if_s_write( 0x02, 0x0e );
  sei();
 // flush_spi_buff();
   _delay_ms(1);
   
   cli();
  if_s_write( 0x00, 0x01 );//CLKEN
  if_s_write( 0x01, 0x00 ); //AKRST
  if_s_write( 0x1A, 0xA3 );
  sei();
 // flush_spi_buff();
   _delay_ms(1);
   
   cli();
  if_s_write( 0x1A, 0x00 );
  sei();
//  flush_spi_buff();
   _delay_ms(30);
   
   cli();
  if_s_write( 0x02, 0x00 );
  if_s_write( 0x19, 0xac );//MASTER VOL
  if_s_write( 0x1B, 0x3F );//interpolation
  if_s_write(0x1b,0x00);
 
  if_s_write( 0x14, 0x00 );//interpolation
  if_s_write( 0x03, 0x01 );//Analog Gain 
  if_s_write( 0x08, 0xF6 );
  sei();
 // flush_spi_buff();
   _delay_ms(21);
   
   cli();
  if_s_write( 0x08, 0x00 );
  if_s_write( 0x09, 0xd0 );
  if_s_write( 0x0b, 0x00 );
  if_s_write( 0x17, 0x40 );//MS_S
  if_s_write( 0x18, 0x00 ); 
  if_s_write(0x20,0x0f);
  sei();
//  flush_spi_buff();
  _delay_us(10);
  
  cli();
  if_s_write(0x21,0x0f);
  sei();
 // flush_spi_buff();
  _delay_us(10);
  
  cli();
  if_s_write(0x22,0x0f);
  sei();
  //flush_spi_buff(); 
  _delay_us(10);




sei();
  for (i = 0; i < 16; i++) {

    if_chs_write(i,0x14, 0);
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
// _delay_ms(30);
}

void startup_sound(void) {
  // _delay_ms(50);
  keyon(0x1c, 0x11);
  _delay_ms(20);
  keyoff();
  _delay_ms(5);

  keyon(0x1c, 0x42);
  _delay_ms(20);
  keyoff();
  _delay_ms(5);

  keyon(0x1c, 0x5d);
  _delay_ms(20);
  keyoff();
  _delay_ms(5);

  keyon(0x24, 0x17);
  _delay_ms(20);
  keyoff();
  // _delay_ms(5);


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

  SPSR = (1<< SPI2X);	//8MH
  //SPSR = 0;  //4Mh
  c = SPDR;
}
void usart_spi_send(unsigned int data) {
  while ( !(UCSR0A & (1 << UDRE0)));
  UDR0 = data;
  //while( !(UCSR1A & (1<<RXC1)));
  data = UDR0;
}








