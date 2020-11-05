#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <linux/i2c-dev.h>

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

int main() {
    int i2c_fd = open("/dev/i2c-1", O_RDWR);

    ssd1306_Init(i2c_fd);

    uint8_t *data = (uint8_t*)malloc(S_WIDTH*S_PAGES);

    for(int x =0;x <S_WIDTH;x++) {
        for(int y =0;y <S_PAGES;y++) {
            data[S_WIDTH*y +x]=(1<<y)-1;
        }
    }

    update_full(i2c_fd, data);
    free(data);
    close(i2c_fd);
    return 0;
}
