/*
 * controlLine.h
 *
 * Created: 2017/07/06 22:14:14
 *  Author: Keiji
 */ 





/*
#define wr_hi() (PORTB |= 0x04)
#define wr_lo() (PORTB &= ~(0x04))
#define wrl_hi() (PORTB |= 0x04)
#define wrl_lo() (PORTB &= ~(0x04))
*/
#define wr_lo()  asm("cbi 5,2 ")
#define wr_hi()  asm("sbi 5,2 ")
#define wrl_lo() asm("cbi 5,2 ")
#define wrl_hi()  asm("sbi  5,2 ")
//#define ymf825wr_hi sbi _SFR_IO_ADDR(PORTD),2
//#define ymf825wr_lo cbi _SFR_IO_ADDR(PORTD),2	


#define rs_hi() asm("cbi 5,1")
#define rs_lo() asm("sbi 5,1")


#define ymf825_cs0_hi()	(PORTD |= 0x04)
#define ymf825_cs1_hi()	(PORTD |= 0x08)
#define ymf825_cs01_hi()	(PORTD |= 0x0c)

#define ymf825_cs0_lo()	(PORTD &=~(0x04))
#define ymf825_cs1_lo()	(PORTD &=~(0x08))
#define ymf825_cs01_lo()	(PORTD &=~(0x0c))





















