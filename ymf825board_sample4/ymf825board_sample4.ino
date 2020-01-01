extern "C" {



#include "Managetone.h"
#include "Midicommand.h"

  //#include <avr/io.h>
  //#include <avr/interrupt.h>
  void optimize_queue(void);
  void setChannelDefault(void);
  char usart_read(void);
  uint8_t master_vol = 0xac;
}
//#include  <SPI.h>

uint8_t data;
uint8_t sysExcnt = 0;
uint8_t dat1, dat2, dat3, com;



void setup() {

  SetupHardware();
  setChannelDefault();
  optimize_queue();
_delay_ms(10);

pinMode( 16,INPUT_PULLUP );
pinMode( 17,INPUT_PULLUP );
  _delay_ms(400);

  TIMSK0 = 0;   //タイマ割り込みの停止

}


void loop() {
extern uint8_t sysex_buf[65];
  
  while (1) {
    if(!(PINC & 0x04)){
      if(master_vol < 0xfc){
        ++master_vol;
       cli();
       if_s_write( 0x19, master_vol );//MASTER VO
       sei();
      }
    }
    if(!(PINC & 0x08)){
      if(master_vol > 4){
       cli();
       --master_vol;
       if_s_write( 0x19, master_vol );//MASTER VO
       sei();
      }
    }
    dat1 = usart_read();

    if (dat1 == 0xf0) {
      sysExcnt = 1;

      do {
        data = usart_read();
        sysex_buf[sysExcnt++] = data;
      } while (data != 0xf7);  //システムエクルシーブメッセージ終了
      midi_sysEx(sysex_buf, sysExcnt);
      sysExcnt = 0;

    } else {

      if (dat1 & 0x80) {
        com = dat1 & 0xf0;
        dat2 = usart_read();

        if ( (com == 0xc0) || (com == 0xd0)) {
          midi_command(com, dat1, dat2, dat3);

        } else {

          dat3 = usart_read();
          midi_command(com, dat1, dat2, dat3);
        }
      }
    }
  }
}

