extern "C" {



#include "Managetone.h"
#include "Midicommand.h"

#include <avr/io.h>
#include <avr/interrupt.h>
  void optimize_queue(void);
  void setChannelDefault(void);
  char usart_read(void);
}
//#include  <SPI.h>

void note_on(uint8_t , uint8_t , uint8_t , uint8_t , uint8_t);



uint8_t data;
uint8_t start = 0;
uint8_t sysExcnt = 0;
uint8_t dat1, dat2, dat3, com;
uint8_t sysex_buf[65];
volatile uint8_t usart_cnt;


uint8_t midi_cnt = 0;
const int LED_PIN = 2;
const int LED2_PIN = 3;
const int LED3_PIN = 4;

#define LED1  (PORTD ^= 0x04)
#define LED2  (PORTD ^= 0x08)

unsigned char fm_data1, fm_data2, fm_data3, fm_data4;

int main() {

  int i;
  DDRD = 0xfe;
  //pinMode(LED_PIN, OUTPUT);
  //pinMode(LED2_PIN, OUTPUT);
  //pinMode(LED3_PIN, OUTPUT);

  
  CLKPR = 0x80;
  CLKPR = 0x00;
  /* usart setup */
  UBRR0 = 0;
  UCSR0A |= (1 << U2X0);
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00) ;

  UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
  UBRR0 = 0x00;


  SPCR = (1 << SPE) | (1 << MSTR) | (0 << SPR0) | (0 << SPR1);
  //SPSR = 0;  //4Mh
  SPSR = (1 << SPI2X); //8MH
  PINB = 0xFF;
  PORTB = 0x0C;

  DDRB = 0x2C;
  SPCR |= (1 << SPIE);



  SetupHardware();
  setChannelDefault();
  optimize_queue();

  _delay_ms(400);

  sei();
  while (1) {


    dat1 = usart_read();

    if (dat1 == 0xf0) {
      //sysex_buf[sysExcnt++] = 0xf0;
      sysExcnt = 1;
      do {
        data = usart_read();
        sysex_buf[sysExcnt++] = data;
        if (sysExcnt > 65)      //エラー時のリカバリ
          break;
      } while (data != 0xf7);  //システムエクルシーブメッセージ終了
      if (sysExcnt < 66) {
        midi_sysEx(sysex_buf, sysExcnt);
      }
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
  setup();
  while (1) {
    loop();
  }
}

void setup() {
}

void loop() {
}

