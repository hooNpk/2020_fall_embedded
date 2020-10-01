#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <unistd.h>
#define BUTTON 0 // 17
#define BUTTON2 2 // 27

int main(void)
{
    if(wiringPiSetup()==-1)
	   return -1;
	pinMode(BUTTON,INPUT);
		while(1)
	{
		if (digitalRead(BUTTON) != HIGH && digitalRead(BUTTON2) != HIGH) {
			printf("Button not pressed\n");	
		}

		else {
			if(digitalRead(BUTTON) == HIGH) {
				printf("Button C pressed\n");
			}

			if(digitalRead(BUTTON2) == HIGH) {
				printf("Button D pressed\n");
			}
		}
		
		sleep(0.08);
	}
	return 0;
}
