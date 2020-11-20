#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <linux/i2c-dev.h>
#include "fonts/font.h"

#define S_WIDTH 128
#define S_HEIGHT 64
#define S_PAGES (S_HEIGHT/8)
#define SSD1306_I2C_DEV 0x3C
#define FONT_WIDTH 6
#define FONT_HEIGHT 1

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

void ssd1306_data(int i2c_fd, const uint8_t *data, size_t size) {
    uint8_t *buffer=(uint8_t*)malloc(size+1);
    buffer[0]=(0<<7)|(1<<6); //Co = 0 , D/C# = 1
    memcpy(buffer+1,data,size);

    if(write(i2c_fd,buffer,size+1)!=size+1) {
        printf("i2c write failed!\n");
    }

    free(buffer);
}

void update_full(int i2c_fd, uint8_t *data) {
    ssd1306_command(i2c_fd,0x20); // addressing mode
    ssd1306_command(i2c_fd,0x0); //horizontal addressing mode
    ssd1306_command(i2c_fd,0x21); //set column start/end address
    ssd1306_command(i2c_fd,0);
    ssd1306_command(i2c_fd,S_WIDTH-1);

    ssd1306_command(i2c_fd,0x22);//set page start/end address
    ssd1306_command(i2c_fd,0);
    ssd1306_command(i2c_fd,S_PAGES-1);

    ssd1306_data(i2c_fd,data,S_WIDTH*S_PAGES);
}

void update_area(int i2c_fd, const uint8_t* data, int x, int y, int x_len, int y_len) {
	ssd1306_command(i2c_fd, 0x20);
	ssd1306_command(i2c_fd, 0x0);

	ssd1306_command(i2c_fd, 0x21);
	ssd1306_command(i2c_fd, x);
	ssd1306_command(i2c_fd, x+x_len-1);

	ssd1306_command(i2c_fd, 0x22);
	ssd1306_command(i2c_fd, y);
	ssd1306_command(i2c_fd, y+y_len-1);

	ssd1306_data(i2c_fd, data, x_len* y_len);
}

void write_char(int i2c_fd, char c, int x, int y) {
	if(c < ' ') c = ' ';
	update_area(i2c_fd, font[c-' '], x, y, FONT_WIDTH, FONT_HEIGHT);
}

void write_str(int i2c_fd, char* str, int x, int y) {
	char c;
	while (c = *str++) {
		write_char(i2c_fd, c, x, y);
		x += FONT_WIDTH;
	}
}

const uint_8 img =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, //                                                                    #
	0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, //                #   #                               # #              
	0x00, 0x00, 0x08, 0x80, 0x02, 0x00, 0x02, 0x20, 0x00, //                     #   #             #               #   #         
	0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x14, 0x00, 0x00, //                         #                          # #              
	0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x80, //                         #                                       #   
	0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //                 #                                                   
	0x00, 0x01, 0x00, 0x08, 0x00, 0x00, 0x04, 0x24, 0x00, //                #            #                        #    #  #      
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //                                                                     
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //                                                                     
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //                                                                     
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //                                                                     
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x00, 0x50, //                                                  # #             # #
	0x00, 0x02, 0x08, 0x80, 0x02, 0x00, 0x20, 0x00, 0x40, //               #     #   #             #           #              #  
	0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //   # #                                                               
	0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, //                         #                                           
	0x08, 0x00, 0x80, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, //     #           #                                 #                 
	0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // #     #                                                             
};

int main() {
    int i2c_fd = open("/dev/i2c-1", O_RDWR);

    if(ioctl(i2c_fd, I2C_SLAVE, SSD1306_I2C_DEV) < 0) {
	    printf("err setting i2c slave address\n");
	    return -1;
    }

    ssd1306_Init(i2c_fd);

    uint8_t *data = (uint8_t*)malloc(S_WIDTH*S_PAGES);
    uint8_t *data2 = (uint8_t*)malloc(40*3);

    memset(data2, 0xaa, 40*3);
    update_area(i2c_fd, data, 64, 2, 40 ,3);

    free(data2);

    for(int x =0;x <S_WIDTH;x++) {
        for(int y =0;y <S_PAGES;y++) {
            data[S_WIDTH*y +x]=(1<<y)-1;
        }
    }
	
	/*
	for(int x=0; x<S_WIDTH;x++){
		for(int y=0; y<S_PAGES; y++){
			data[S_WIDTH*y + x] = 1;
		}

	}
	*/
    update_full(i2c_fd, data);
    free(data);
    write_str(i2c_fd, "Hello Embedded!", 10, S_PAGES-1);
    close(i2c_fd);
    return 0;
}
