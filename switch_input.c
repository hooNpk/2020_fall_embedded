#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <unistd.h>
#define BUTTON 2 //27
#define LED 0 //17
int main(void)
{
    if(wiringPiSetup()==-1)
	   return -1;
	pinMode(BUTTON,INPUT);
	pinMode(LED, OUTPUT);
		while(1)
	{
		if(digitalRead(BUTTON)==HIGH){
			printf("Button was pressed !! \n");
			digitalWrite(LED,HIGH);
		} else {
			printf("Button was NOT pressed !! \n");
			digitalWrite(LED,LOW);
		}
		sleep(1);
	}
	return 0;
}