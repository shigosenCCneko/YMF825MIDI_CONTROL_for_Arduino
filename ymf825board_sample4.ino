extern "C" {



#include "Managetone.h"
#include "Midicommand.h"

  //#include <avr/io.h>
  //#include <avr/interrupt.h>
  void optimize_queue(void);
  void setChannelDefault(void);
  char usart_read(void);
}
//#include  <SPI.h>

uint8_t data;
uint8_t sysExcnt = 0;
uint8_t dat1, dat2, dat3, com;
uint8_t sysex_buf[65];

uint8_t midi_cnt = 0;








unsigned char fm_data1, fm_data2, fm_data3, fm_data4;

void setup() {

  SetupHardware();
  setChannelDefault();
  optimize_queue();

  _delay_ms(400);
  //Serial.begin(250000);
  (_SFR_BYTE(TIMSK0) &= ~(_BV(TOIE0)));   //タイマ割り込みの停止

  //  sei();

}


void loop() {

  while (1) {
  
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

