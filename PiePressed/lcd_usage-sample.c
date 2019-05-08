/* For La Fortuna board.            */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "lcd.h"
#include "rios.h"
#include "ruota.h"


#define BUFFSIZE 256

void init(void);

int check_switches(int);
int update_building_pies(int);

void add_pies(long);
void refresh_pies();

void add_girl_scout();
void add_grandma();
void add_factory();
void add_reactor();

char* int_to_string(int);

long  pies;
long pies_gain;

int num_of_girl_scouts;
int girl_scout_pies;

int num_of_grandmas;
int grandma_pies;

int num_of_factories;
int factory_pies;

int num_of_reactors;
long int reactor_pies;

int building_tick_delay;


void main(void) {
    init();
	building_tick_delay = 400;
    os_add_task( check_switches,  50, 1);
	os_add_task( update_building_pies, building_tick_delay, 0);

    sei();
    for(;;){}

}

void display_title_text() {
	display_string_xy("",0,0);
	display_string("            _         \n");
	display_string("           (_)        \n");
	display_string("      _ __  _  ___      _ __  _ __ ___  ___ ___  ___ _ __       ");
	display_string("     | \'_ \\| |/ _ \\    | \'_ \\| \'__/ _ \\/ __/ __|/ _ \\ \'__| \n");
	display_string("     | |_) | |  __/    | |_) | | |  __/\\__ \\__ \\  __/ |       \n");
	display_string("     | .__/|_|\\___|    | .__/|_|  \\___||___/___/\\___|_|        \n");
	display_string("     | |               | |         \n");
	display_string("     |_|               |_|     \n");
	
	display.foreground = LIGHT_SALMON;
	display_string("\n\n\n\n");
	display_string("                             ( \n");
	display_string("                              ) \n");
	display_string("                         __..---..__ \n");
	display_string("                     ,-=\'  /  |  \\  `=-. \n");
	display_string("                    :--..___________..--; \n");
	display_string("                     \\.,_____________,./ \n");
	
	display.foreground = WHITE;
	display_string("\n           press the middle button to make pies!");
	
	display.foreground = WHITE;
	display_string_xy("< Buy Girl Scout (300g)              > Buy Grandma (1000g) \n^ Buy Factory (8000g)                v Buy Pie Reactor (50000g)",0, 190);
						
}


void init(void) {
    /* 8MHz clock, no prescaling (DS, p. 48) */
    CLKPR = (1 << CLKPCE);
    CLKPR = 0;

    init_lcd();
    os_init_scheduler();
    os_init_ruota();
	
	/*set up values for game*/
	pies = 0;
	pies_gain = 0;
	
	num_of_girl_scouts = 0;
	girl_scout_pies = 1;
	
	num_of_grandmas = 0;
	grandma_pies = 5;
	
	num_of_factories = 0;
	factory_pies = 60;
	
	num_of_reactors = 0;
	reactor_pies = 400;
	
	display_title_text();
}


int check_switches(int state) {


	if (get_switch_press(_BV(SWN))) {
			add_factory();
	}

	if (get_switch_press(_BV(SWE))) {
			add_grandma();
	}

	if (get_switch_press(_BV(SWS))) {
			add_reactor();
	}

	if (get_switch_press(_BV(SWW))) {
			add_girl_scout();
	}

	if (get_switch_short(_BV(SWC))) {
		add_pies(3);
		refresh_pies();
	}
	
	

	return state;
}

int update_building_pies(int state) {	
	pies += pies_gain;
	refresh_pies();
	
	return state;
}

void refresh_pies() {
	
	/* update display */
	display.foreground = WHITE;
	char buffer[256];		
	int x = LCDWIDTH/2 + 17;
	int y = LCDHEIGHT/2 + 7;
	clear_screen_xy(x, 320, y, y+20);
	display_string_xy("Pies: ", x, y);
	display_string(ltoa(pies, buffer, 10));
	
	display_string_xy("Gain: ", x, y+10);
	display.foreground = GREEN;
	display_string("+");
	display_string(ltoa(pies_gain, buffer, 10));
}

char* int_to_string(int n) {
	int ncopy = n;
	char *string = malloc(sizeof(char)*10); //allow up to 10 digits
	int counter = 0;
	while (ncopy > 1) {
		int tempn = ncopy % 10;
		*(string + sizeof(char)) = tempn;
		counter++;
		ncopy  = ncopy / 10;
	}
	return string;
}


void add_pies(long diff) {
	pies += diff;
}

void add_pies_gain(long gain) {
	pies_gain += gain;
}

void add_girl_scout() {
	int price = 300;
	if (price <= pies) {
		add_pies(-price);
		add_pies_gain(girl_scout_pies);
		refresh_pies();
	}
}

void add_grandma() {
	int price = 1000;
	if (price <= pies) {
		add_pies(-price);
		add_pies_gain(grandma_pies);
		refresh_pies();
	}
}

void add_factory() {
	int price = 8000;
	if (price <= pies) {
		add_pies(-price);
		add_pies_gain(factory_pies);
		refresh_pies();
	}
}

void add_reactor() {
	long price = 50000;
	if (price <= pies) {
		add_pies(-price);
		add_pies_gain(reactor_pies);
		refresh_pies();
	}
}