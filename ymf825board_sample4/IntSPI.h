 *
 *  cs0  module 0 select
 *  cs1  module 1 select
 *  cs01 module 0 and 1 select
 *
 *
 *
 */ 


#define DEV_MASK 0x7f //PORT 0-6 avairable
#define DEV_SELECTPORT  PORTD
#define DEV_RESETPORT PORTC
#define  DEV_RESETBIT 6

#define wr_hi_all() (DEV_SELECTPORT |= DEV_MASK)
#define wr_lo_all() (DEV_SELECTPORT &= ~(DEV_MASK))

#define rs_hi() (DEV_RESETPORT |= ( 1<<DEV_RESETBIT)) //reset YMF825
#define rs_lo() (DEV_RESETPORT &= ~(1 << DEV_RESETBIT))



