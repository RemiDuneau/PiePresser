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
int update_building_money(int);

void add_money(long);
void refresh_money();

void add_girl_scout();
void add_grandma();
void add_factory();
void add_reactor();

char* int_to_string(int);

long  money;
long money_gain;

int num_of_girl_scouts;
int girl_scout_money;

int num_of_grandmas;
int grandma_money;

int num_of_factories;
int factory_money;

int num_of_reactors;
long int reactor_money;

int building_tick_delay;


void main(void) {
    init();
	building_tick_delay = 400;
    os_add_task( check_switches,  50, 1);
	os_add_task( update_building_money, building_tick_delay, 0);

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
	display_string("\n           press the middle button to make money!");
	
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
	money = 0;
	money_gain = 0;
	
	num_of_girl_scouts = 0;
	girl_scout_money = 1;
	
	num_of_grandmas = 0;
	grandma_money = 5;
	
	num_of_factories = 0;
	factory_money = 60;
	
	num_of_reactors = 0;
	reactor_money = 400;
	
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
		add_money(3);
		refresh_money();
	}
	
	

	return state;
}

int update_building_money(int state) {	
	money += money_gain;
	refresh_money();
	
	return state;
}

void refresh_money() {
	
	/* update display */
	display.foreground = WHITE;
	char buffer[256];		
	int x = LCDWIDTH/2 + 17;
	int y = LCDHEIGHT/2 + 7;
	clear_screen_xy(x, 320, y, y+20);
	display_string_xy("Money: ", x, y);
	display_string(ltoa(money, buffer, 10));
	
	display_string_xy("Gain: ", x, y+10);
	display.foreground = GREEN;
	display_string("+");
	display_string(ltoa(money_gain, buffer, 10));
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


void add_money(long diff) {
	money += diff;
}

void add_money_gain(long gain) {
	money_gain += gain;
}

void add_girl_scout() {
	int price = 300;
	if (price <= money) {
		add_money(-price);
		add_money_gain(girl_scout_money);
		refresh_money();
	}
}

void add_grandma() {
	int price = 1000;
	if (price <= money) {
		add_money(-price);
		add_money_gain(grandma_money);
		refresh_money();
	}
}

void add_factory() {
	int price = 8000;
	if (price <= money) {
		add_money(-price);
		add_money_gain(factory_money);
		refresh_money();
	}
}

void add_reactor() {
	long price = 50000;
	if (price <= money) {
		add_money(-price);
		add_money_gain(reactor_money);
		refresh_money();
	}
}