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
#define LOGO_WIDTH 51
#define LOGO_HEIGHT 3
#define LOGO_MOVE 4
#define NUM_FRAMES (S_WIDTH/LOGO_MOVE)
#define LOGO_Y_LOC 1

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

const unsigned char img [] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0,
	0xE0, 0xF0, 0xF0, 0xF8, 0xF8, 0xFC, 0xFC, 0xFC, 0xDC, 0x5C, 0xFC, 0xFC, 0xFC, 0xFC, 0x1C, 0x1C, 0x1C, 0x1C, 0x18, 0xB8,
	0xB8, 0xF0, 0xF0, 0xE0, 0xE0, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x80, 0xE0, 0xF8, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFB, 0xC1, 0x00,
	0x01, 0x07, 0xC3, 0xF1, 0xFC, 0xFE, 0xFE, 0x1F, 0x07, 0x03, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x03, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0x3F, 0x3F, 0x3B, 0x31, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x03, 0x1F, 0x7F, 0xFF, 0xFF, 0xF8, 0xE0, 0xC0,
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0x1F, 0x1F, 0x8F, 0xC6, 0xC6, 0xE0, 0x70, 0x70, 0x38, 0x18, 0x0C, 0x0C, 0x04,
	0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x7F, 0xFE, 0xFC, 0xF8, 0xE0, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0F, 0x3F, 0x7F, 0xFF, 0xFE, 0x7F, 0x1F,
	0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xC1, 0xFF, 0xFF, 0x7F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x1F, 0x1E, 0x3C, 0x3C, 0x38, 0x38,
	0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x30, 0x38, 0x38, 0x1C, 0x1E, 0x0F, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00};

void update_area_x_wrap(int i2c_fd, const uint8_t* data, int x, int y, int x_len, int y_len) {
	if(x+x_len <= S_WIDTH) update_area(i2c_fd, data, x, y, x_len, y_len);
	else{
		int part1_len = S_WIDTH - x;
		int part2_len = x_len - part1_len;
		uint8_t* part1_buf = (uint8_t*)malloc(part1_len * y_len);
		uint8_t* part2_buf = (uint8_t*)malloc(part2_len * y_len);

		for(int x=0; x<part1_len; x++){
			for(int y=0; y<y_len; y++){
				part1_buf[part1_len*y+x] = data[x_len*y+x];
			}
		}
		for(int x=0; x<part2_len; x++){
			for(int y=0; y<y_len; y++){
				part2_buf[part2_len*y+x] = data[x_len*y+part1_len+x];
			}
		}
		update_area(i2c_fd, part1_buf, x, y, part1_len, y_len);
		update_area(i2c_fd, part2_buf, 0, y, part2_len, y_len);

		free(part1_buf);
		free(part2_buf);
	}
}

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
	
    
    uint8_t *data3 = (uint8_t*)malloc(S_WIDTH*S_PAGES*NUM_FRAMES*5);

    for(int i=0; i<NUM_FRAMES; i++) {
	    for(int x=0; x<LOGO_WIDTH; x++) {
		    for(int y=0; y<LOGO_HEIGHT; y++) {
			    int target_x = (i*LOGO_MOVE + x) % S_WIDTH;
			    data3[S_WIDTH*S_PAGES*i + S_WIDTH*(y+LOGO_Y_LOC)+ target_x] = img[LOGO_WIDTH*y+x];
		    }
	    }
    }
    

    uint8_t* data_s = (uint8_t*)malloc((LOGO_WIDTH+LOGO_MOVE)*LOGO_HEIGHT);
    for(int y=0; y<LOGO_HEIGHT; y++){
	for(int x=0; x<LOGO_MOVE; x++){
		data_s[(LOGO_WIDTH + LOGO_MOVE) * y + x] = 0x0;
	}
	for(int x=0; x<LOGO_WIDTH; x++){
		data_s[(LOGO_WIDTH + LOGO_MOVE)*y + (x+LOGO_MOVE)] = img[LOGO_WIDTH*y+x];
	}
    }

    while(1) {
	    for(int i=0; i<NUM_FRAMES; i++) {
		    //update_full(i2c_fd, &data3[S_WIDTH*S_PAGES*i]);
		    update_area_x_wrap(i2c_fd, data_s, i*LOGO_MOVE, LOGO_Y_LOC, LOGO_WIDTH+LOGO_MOVE, LOGO_HEIGHT);
	    }
    }

    free(data);
    free(data2);
    free(data3);
    close(i2c_fd);
    return 0;
}
