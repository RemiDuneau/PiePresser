/* ruota.c

    Copyright Peter Dannegger (danni@specs.de)
    http://www.mikrocontroller.net/articles/Entprellung 
    http://www.mikrocontroller.net/articles/Drehgeber

    Slightly adapted by Klaus-Peter Zauner for FortunaOS, March 2015

*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "rios.h"
#include "ruota.h"


#define COMPASS_SWITCHES (_BV(SWW)|_BV(SWS)|_BV(SWE)|_BV(SWN))
#define ALL_SWITCHES (_BV(SWC) | COMPASS_SWITCHES | _BV(OS_CD))


int8_t os_enc_delta(void);
volatile int8_t delta;


volatile uint8_t switch_state;   /* debounced and inverted key state:
                                 bit = 1: key pressed */
volatile uint8_t switch_press;   /* key press detect */
 


int scan_switches(int state);


void os_init_ruota(void) {

    /* Configure I/O Ports */
	DDRE &= ~_BV(SWC);   /* Central button */
	PORTE |= _BV(SWC);
	
	DDRC &= ~COMPASS_SWITCHES;  /* configure compass buttons for input */
	PORTC |= COMPASS_SWITCHES;  /* and turn on pull up resistors */

	/* Schedule button scan at 10 ms */
	os_add_task( scan_switches, 10, 0);

}


/* Read the two step encoder
   -> call frequently enough to avoid overflow of delta
*/
int8_t os_enc_delta() {
    int8_t val;

    cli();
    val = delta;
    delta &= 1;
    sei();

    return val >> 1;
}



int scan_switches(int state) {
  static uint8_t ct0, ct1, rpt;
  uint8_t i;
 
  cli();
  /* 
     Overlay port E for central button of switch wheel and Port B
     for SD card detection switch:
  */ 
  i = switch_state ^ ~( (PINC|_BV(SWC))& (PINE|~_BV(SWC)));  
										   /* switch has changed */
  ct0 = ~( ct0 & i );                      /* reset or count ct0 */
  ct1 = ct0 ^ (ct1 & i);                   /* reset or count ct1 */
  i &= ct0 & ct1;                          /* count until roll over ? */
  switch_state ^= i;                       /* then toggle debounced state */
  switch_press |= switch_state & i;        /* 0->1: key press detect */
  sei();
  
  return state;
}




/*
   Check if a key has been pressed
   Each pressed key is reported only once.
*/
uint8_t get_switch_press( uint8_t switch_mask ) {
  cli();                         /* read and clear atomic! */
  switch_mask &= switch_press;         /* read key(s) */
  switch_press ^= switch_mask;         /* clear key(s) */
  sei();
  return switch_mask;
}


 
/*
   Check if a key is pressed right now
*/
uint8_t get_switch_state( uint8_t switch_mask ) {
	switch_mask &= switch_state;
	return switch_mask;
}

 
/*
   Read key state and key press atomic!
*/
uint8_t get_switch_short( uint8_t switch_mask ) {
  cli();                                         
  return get_switch_press( ~switch_state & switch_mask );
}
