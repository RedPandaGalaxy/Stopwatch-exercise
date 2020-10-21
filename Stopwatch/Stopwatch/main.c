/*
 * Stopwatch.c
 *
 * Created: 20.11.2017 12:58:09
 * Author : MSI
 */ 

#define F_CPU 16E6
#include <avr/io.h> //necessary libraries
#include <stdio.h>

#include <util/delay.h>
#include "i2cmaster.h" //necessary libraries
#include "lcd.h"

typedef struct     //new data type
{
	int hours;
	int minutes;
	int seconds;
	int hundredths_of_seconds;
}time_t;	//name of new data type

time_t current_time;	//global variables of new type
time_t split_time;

void add_time();	//function prototypes
void print_time(int, time_t);
void reset_time();

int main(void)
{
	DDRD = 0xFF; //I/O board:PD4…7 as outputs, for LEDs
	DDRC = 0xF0; //I/O board PC0…3 as inputs, for buttons
	PORTC = 0x3F; // Enable internal pull at PC0..3 inputs
	PORTD = 0x00; // Set output LEDs to off

	lcd_init();
						//indicators
	char flag_run=0;	//when we measure current time (when ==1 then keeps running)
	char flag_split=0;	//when we want to print split time value
	char flag_get_split=1;
	
	while(1)
{	
	
	if (PINC == 0b00111110) //1st button
		flag_run=1;		//keeps measuring 
		
	while(flag_run==1)	//measuring / running
 {
	add_time();	//measuring time (adding 1 hundredth of second);
	print_time(1,current_time);//printing current time value
	
	if (flag_get_split==1)  //checks if we need to get split value
		split_time=current_time;
	
	if (flag_split==1)//checking is we need to print split time
		{
			print_time(2,split_time);//if needed it printed
			flag_split=0;//it will be printed only once
		}
		
	if (PINC == 0b00111101) //2nd button
		flag_run=0;//stops measuring/running
	
	if (PINC == 0b00111011) //3rd button
	{
		flag_split=1;//change of indicator-> now it will be printed
		flag_get_split=0;
	}
		else flag_get_split=1;
	
	if (PINC == 0b00110111) //4th button
	{
		flag_split=0;//stopping printing split time
		flag_run=0;//stopping measuring/running current time
		reset_time();//function call
 }
		
	}
	//outside while-loop when measuring is not running; makes other buttons 3 and 4 working even if
	//measuring is stopped (button3 is pressed) ; no printing outside former while-loop
	
	if (PINC == 0b00111011) //3rd button
	{
		flag_split=1;//printing split value(if we resume measuring time)
		split_time=current_time;//getting split value
		print_time(2,split_time);//print this split value
	}
	
	if (PINC == 0b00110111) //4th button
	{
		flag_split=0;//stop printing split value
		flag_run=0;//stop measuring and printing
		reset_time();//calling function
	}

}

}


void add_time()//function definition: measuring the time- adding 1 hundredth of second
{
	current_time.hundredths_of_seconds=current_time.hundredths_of_seconds+1;//adding 1 hundredth of second
	if (current_time.hundredths_of_seconds==100)//checking if we reached 1s
		{
			current_time.hundredths_of_seconds=0;//reseting hundredths of second value
			current_time.seconds=current_time.seconds+1;//increasing second value
			
			if (current_time.seconds==60)//checking if we reached minute 
			{
				current_time.seconds=0;//reseting seconds value
				current_time.minutes=current_time.minutes+1;//increasing minutes value
			}
				if (current_time.minutes==60)//checking if we reached hour value
				{
					current_time.minutes=0;//reseting minutes value
					current_time.hours=current_time.hours+1;//increasing hours value
				}
		}	
	_delay_ms(9.2);	//our "unit" of time- 1 hundredth of second +-program delay
}

void print_time (int line, time_t time_type)//functon definition: printing time (which line ; what type of time)
{
	lcd_gotoxy(line,1);		
	printf("%01d:%02d:%02d:%02d",time_type.hours,time_type.minutes,time_type.seconds,time_type.hundredths_of_seconds);
	//printing
}

void reset_time()//function definition: reseting time->setting everything to zero
{
	lcd_clear();//clearing the lcd screen
	current_time.hours=0;//setting all values to 0
	current_time.minutes=0;
	current_time.seconds=0;
	current_time.hundredths_of_seconds=0;
	print_time(1,current_time);
}