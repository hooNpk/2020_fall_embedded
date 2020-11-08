#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <linux/i2c-dev.h>

#define SSD1306_I2C_DEV 0x3c

#define S_WIDTH 128
#define S_HEIGHT 64
#define S_PAGES (S_HEIGHT/8)

/** void ssd1306_Init(int i2c_fd); */
// void ssd1306_command(int i2c_fd, uint8_tcmd);
// void ssd1306_data(int i2c_fd, const uint8_t *data, size_tsize);

void ssd1306_command(int i2c_fd, uint8_t cmd) {
    uint8_t buffer[2];
    buffer[0]=(0<<7)|(0<<6);//Co = 0, D/C# = 0
    buffer[1]=cmd;

    if(write(i2c_fd,buffer,2)!=2) {
        printf("i2c write failed!\n");
    }
}

void ssd1306_Init(int i2c_fd) {
		ssd1306_command(i2c_fd, 0xA8);
		ssd1306_command(i2c_fd, 0x3f);

		ssd1306_command(i2c_fd, 0xD3);
		ssd1306_command(i2c_fd, 0x00);

		ssd1306_command(i2c_fd, 0x40);
		ssd1306_command(i2c_fd, 0xA0);

		ssd1306_command(i2c_fd, 0xC0);

		ssd1306_command(i2c_fd, 0xDA);
		ssd1306_command(i2c_fd, 0x12);

		ssd1306_command(i2c_fd, 0x81);
		ssd1306_command(i2c_fd, 0x7F);

		ssd1306_command(i2c_fd, 0xA4);

		ssd1306_command(i2c_fd, 0xA6);

		ssd1306_command(i2c_fd, 0xD5);
		ssd1306_command(i2c_fd, 0x80);

		ssd1306_command(i2c_fd, 0x8D);
		ssd1306_command(i2c_fd, 0x14);

		ssd1306_command(i2c_fd, 0xAF);
}

int main() {
    int i2c_fd = open("/dev/i2c-1", O_RDWR);
    
	if(i2c_fd<0){
		printf("err opneing device\n");
	}

	if(ioctl(i2c_fd, I2C_SLAVE, SSD1306_I2C_DEV) <0 ){
		printf("err setting i2c slave address\n");
	}

	ssd1306_Init(i2c_fd);

	close(i2c_fd);
	
	return 0;
}
