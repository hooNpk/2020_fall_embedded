#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>
#include <wiringPi.h>
#include <stdint.h>
#include <sys/mman.h>

#define PERIPHERAL_BASE 0x3F000000UL
#define GPIO_BASE (PERIPHERAL_BASE + 0x200000)

void set_gpio_output_value(void *gpio_ctr, int gpio_nr, int value) {
	int reg_id = gpio_nr / 32;
	int pos = gpio_nr % 32;

	if(value) {
		#define GPIO_SET_OFFSET 0x1c
		uint32_t* output_set = (uint32_t*) (gpio_ctr + GPIO_SET_OFFSET + 0x4 * reg_id);
		*output_set = 0x1<<pos;
	}

	else {
		#define GPIO_CLR_OFFSET 0x28
		uint32_t* output_clr = (uint32_t*) (gpio_ctr + GPIO_CLR_OFFSET + 0x4 * reg_id);
		*output_clr = 0x1 << pos;
	}
}

void set_gpio_output(void *gpio_ctr, int gpio_nr) {
	int reg_id = gpio_nr / 10;
	int pos = gpio_nr % 10;

	uint32_t* fsel_reg =(uint32_t*) (gpio_ctr + 0x4 * reg_id);

	uint32_t fsel_val = *fsel_reg;

	uint32_t mask = 0x7 << (pos*3);
	fsel_val = fsel_val & ~mask;

	uint32_t gpio_output_select = 0x1 << (pos*3);
	fsel_val = fsel_val | gpio_output_select;

	*fsel_reg = fsel_val;
}

int main(){
	int fdmem = open("/dev/mem", O_RDWR);
	if (fdmem < 0) {
		printf("asdasdsad"); return -1; }

	void* gpio_ctr = mmap(0, 4096, PROT_READ+PROT_WRITE, MAP_SHARED, fdmem, GPIO_BASE);
	if(gpio_ctr == MAP_FAILED) {
		printf("asdsadsa"); return -1;}

	set_gpio_output(gpio_ctr,13);
	set_gpio_output(gpio_ctr,19);
	set_gpio_output(gpio_ctr,26);

	int fd, ret;
	fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);
	if(fd==-1){printf("Port open error"); exit(-1);}

	struct termios config;
	memset(&config, 0, sizeof(config));

	config.c_iflag = IGNPAR;
	config.c_oflag = 0;
	config.c_cflag = CS8 | CLOCAL;
	config.c_lflag = 0;

	config.c_cc[VMIN] = 1;
	config.c_cc[VTIME] = 0;

	cfsetispeed(&config, B9600);
	cfsetospeed(&config, B9600);

	tcflush(fd, TCIFLUSH);

	ret = tcsetattr(fd, TCSANOW, &config);
	if(ret<0){printf("Configuration error\n"); exit(-1);}

	const char w_buf[]="Hello! Can you see this?\n";
	write(fd, w_buf, strlen(w_buf));

	char r_buf[256] = {0};
	while(ret=read(fd, r_buf, 255) >=0){
		if(ret){
			printf("Received %d bytes : %s\n", ret, r_buf);
		}

		if(*r_buf == '1') {
			set_gpio_output_value(gpio_ctr, 13, 1);
		}

		else if(*r_buf == '0') {
			set_gpio_output_value(gpio_ctr, 13, 0);
		}

	}

	close(fd);

}
