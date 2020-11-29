#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <termio.h>
#include <signal.h>
#include <sys/poll.h>
#include <stdint.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <wiringPi.h>
#define BUTTON 0 // 17
#define BUTTON2 2 // 27
#define BUTTON_RECORD 3 // 22
#define BUTTON_RECORD_PLAY 4 // 23

#include "record/record.c"

bool RECORD_START = false;
int inst_num = 0;
struct timeval start, cur;
FILE* fp;				

void set_gpio_output_value(void *gpio_ctr, int gpio_nr, int value) {
	int reg_id = gpio_nr / 32;
	int pos = gpio_nr % 32;

	if(value) {
		#define GPIO_SET_OFFSET 0x1c
		uint32_t* output_set = (uint32_t*)(gpio_ctr + GPIO_SET_OFFSET + 0x4 * reg_id);
		*output_set = 0x1 << pos;
	}

	else {
		#define GPIO_CLR_OFFSET 0x28
		uint32_t* output_clr = (uint32_t*)(gpio_ctr + GPIO_CLR_OFFSET + 0x4 * reg_id);
		*output_clr = 0x1 << pos;
	}
}

void set_gpio_output(void *gpio_ctr, int gpio_nr) {
	int reg_id = gpio_nr / 10;
	int pos = gpio_nr % 10;

	uint32_t* fsel_reg = (uint32_t*)(gpio_ctr + 0x4 * reg_id);
	uint32_t fsel_val = *fsel_reg;
	uint32_t mask = 0x7 << (pos * 3);
	
	fsel_val = fsel_val & ~mask;

	uint32_t gpio_output_select = 0x1 << (pos * 3);
	fsel_val = fsel_val | gpio_output_select;

	*fsel_reg = fsel_val;
}

void set_gpio_input(void *gpio_ctr, int gpio_nr) {
	int reg_id = gpio_nr / 10;
	int pos = gpio_nr % 10;

	uint32_t* fsel_reg = (uint32_t*)(gpio_ctr + 0x4 * reg_id);
	uint32_t fsel_val = *fsel_reg;

	uint32_t mask = 0x7 << (pos * 3);
	fsel_val = fsel_val & ~mask;

	*fsel_reg = fsel_val;
}

void set_gpio_pullup(void *gpio_ctr, int gpio_nr) {
	int reg_id = gpio_nr / 32;
	int pos = gpio_nr % 32;

	#define GPIO_PUD_OFFSET 0x94
	#define GPIO_PUDCLK_OFFSET 0x98	
	uint32_t* pud_reg = (uint32_t*)(gpio_ctr + GPIO_PUD_OFFSET);
	uint32_t* pudclk_reg = (uint32_t*)(gpio_ctr + GPIO_PUDCLK_OFFSET + 0x4 * reg_id);

	#define GPIO_PUD_PULLUP 0x2
	*pud_reg = GPIO_PUD_PULLUP;
	usleep(1);
	*pudclk_reg = (0x1 << pos);
	usleep(1);
	*pud_reg = 0;
	*pudclk_reg = 0;
}

void get_gpio_input_value(void *gpio_ctr, int gpio_nr, int *value) {
	int reg_id = gpio_nr / 32;
	int pos = gpio_nr % 32;

	#define GPIO_LEV_OFFSET 0x34
	uint32_t* level_reg = (uint32_t*)(gpio_ctr + GPIO_LEV_OFFSET + 0x4 * reg_id);
	uint32_t level = *level_reg & (0x1 << pos);

	*value = level? 1:0;
}

void* play_background_drum(void *data) {
	static int retval = 999;
	pid_t pid;
	pthread_t tid;

	pid = getpid();
	tid = pthread_self();

	pthread_detach(tid);

	char* thread_name = (char*)data;

	system("mpg321 sound/Drum_Loop_62_BPM.mp3");
	printf("Thread : %s\n", thread_name);
}

void* play_background_djembe(void *data) {
	pid_t pid;
	pthread_t tid;

	pid = getpid();
	tid = pthread_self();

	pthread_detach(tid);

	char* thread_name = (char*)data;

	system("mpg321 sound/Djembe_Loop_90_Bpm.mp3");
	printf("Thread : %s\n", thread_name);
}

void* play_background_metronome(void *data) {
	pid_t pid;
	pthread_t tid;

	pid = getpid();
	tid = pthread_self();

	pthread_detach(tid);

	char* thread_name = (char*)data;

	system("mpg321 sound/Wood_Metronome_60_Bpm.mp3");
	printf("Thread : %s\n", thread_name);
}

void* play_background(void *data) {
	pid_t pid;
	pthread_t tid, pthread, pthread2, pthread3;
	int thr_id = 0;
	char p0[] = "Background Drum Thread";
	char p1[] = "Background Djembe Thread";
	char p2[] = "Background Metronome Thread";

	pid = getpid();
	tid = pthread_self();

	pthread_detach(tid);
	char* thread_name = (char*)data;
	printf("Thread : %s\n", thread_name);

	int which = 0;

	while(1) {
		usleep(500000);
		//printf("%d\n", which);
		if(digitalRead(BUTTON) != HIGH) {
		}

		else {
			if(which == 0) { // play drum
				thr_id = pthread_create(&pthread, NULL, play_background_drum, (void *)p0);
				which++;
			}

			else if(which == 1) { // play djembe
				/** pthread_kill(pthread, SIGKILL); */
				thr_id = pthread_create(&pthread2, NULL, play_background_djembe, (void *)p1);
				which++;
			}

			else if(which == 2) { // play metronome
				/** pthread_kill(pthread2, SIGKILL); */
				thr_id = pthread_create(&pthread3, NULL, play_background_metronome, (void *)p2);
				which++;
			}

			else if(which == 3) { // background terminate
				/** pthread_kill(pthread3, SIGKILL); */
				which = 0;
			}
		}
	}
}

void* change_instrument(void *data) {
	pid_t pid;
	pthread_t tid;

	pid = getpid();
	tid = pthread_self();

	pthread_detach(tid);
	char* thread_name = (char*)data;
	printf("Thread : %s\n", thread_name);

	while(1) {
		//printf("inst num : %d\n",inst_num);
		if(digitalRead(BUTTON2) != HIGH) {
		}

		else {
			if(inst_num == 2) {
				inst_num = 0;
			}

			else {
				inst_num++;
			}
		}
		usleep(500000);
	}
}

void* record_waiting(void* data) {
	pid_t pid;
	pthread_t tid, pthread;
	int thr_id = 0;

	char p0[] = "Recording Start!";
	pid = getpid();
	tid = pthread_self();

	pthread_detach(tid);
	char* thread_name = (char*)data;
	printf("Thread : %s\n", thread_name);

	while(1) {
		usleep(300000);
			if(digitalRead(BUTTON_RECORD) != HIGH) {
			}

			else {
				RECORD_START = !RECORD_START;
				//thr_id = pthread_create(&pthread, NULL, record, (void *)p0);

				if(RECORD_START){
					printf("RECORDING START!\n");
					gettimeofday(&start, NULL);
					fprintf(fp, "0 0 0 0 0 0 0 0 ");
				}
				else{
					printf("RECORDING END!\n");
					fclose(fp);
				}
			}
	}
}

void* record_play_waiting(void* data) {
	pid_t pid;
	pthread_t tid, pthread;
	int thr_id = 0;

	char p0[] = "Record Playing Start!";
	pid = getpid();
	tid = pthread_self();

	pthread_detach(tid);
	char* thread_name = (char*)data;
	printf("Thread : %s\n", thread_name);

	while(1) {
		usleep(300000);
			if(digitalRead(BUTTON_RECORD_PLAY) != HIGH) {
			}

			else {
				thr_id = pthread_create(&pthread, NULL, record_play, (void *)inst_num);
			}
	}
}

int main() {
	#define PERIPHERAL_BASE 0x3F000000UL
	#define GPIO_BASE (PERIPHERAL_BASE + 0x200000)

	long diff_sec, diff_usec;
	float diff;
	bool keyboard_vector[8] = {0};
	fp = fopen("vector.txt", "w");

	//int fdmem = open("/dev/mem", O_RDWR);
	//if (fdmem < 0) {
	//	printf("Error opening /dev/mem\n");
	//	return -1;
	//}

	//void* gpio_ctr = mmap(0, 4096, PROT_READ + PROT_WRITE, MAP_SHARED, fdmem, GPIO_BASE);
	//if (gpio_ctr == MAP_FAILED) {
	//	printf("mmap error\n");
	//	return -1;
	//}

	if(wiringPiSetup() == -1) {
		printf("wiringPi Setup error\n");
		exit(-1);
	}

	pinMode(BUTTON, INPUT);
	pinMode(BUTTON2, INPUT);
	pinMode(BUTTON_RECORD, INPUT);
	pinMode(BUTTON_RECORD_PLAY, INPUT);

	pthread_t p_thread[8];
	pthread_t p_thread_background;
	pthread_t p_thread_change;
	int thr_id;
	int status;

	char* pb = "Background Music Thread";
	char* pc = "Change Instrument Thread";
	char* pr = "Recording Thread";
	char* prp = "Recording Play Thread";

	//set_gpio_output(gpio_ctr, 13);
	//set_gpio_output(gpio_ctr, 19);
	//set_gpio_output(gpio_ctr, 26);

	//set_gpio_input(gpio_ctr, 4);
	//set_gpio_pullup(gpio_ctr, 4);

	int gpio_4_value = 0;
	int gpio_flag = 0;

	thr_id = pthread_create(&p_thread_background, NULL, play_background, (void *)pb);
	thr_id = pthread_create(&p_thread_change, NULL, change_instrument, (void *)pc);	
	thr_id = pthread_create(&p_thread_change, NULL, record_waiting, (void *)pr);
	thr_id = pthread_create(&p_thread_change, NULL, record_play_waiting, (void *)prp);	

	while(1) {
		int i = 0, j=0;
		
		char key = get_input(1);
		printf("%c\n", key);
		printf("inst num : %d\n", inst_num);
		
		if(RECORD_START){
			for(j=0; j<8; j++){
				keyboard_vector[j]=0;
			}
			gettimeofday(&cur, NULL);
			diff_sec = cur.tv_sec - start.tv_sec;
			diff_usec = cur.tv_usec - start.tv_usec;
			diff = diff_usec/1000000.0f + diff_sec;
			fprintf(fp, "%.3f\n", diff);
			start=cur;
		}


		if(inst_num == 0) { // piano sound

			//set_gpio_output_value(gpio_ctr, 13, 1);
			//set_gpio_output_value(gpio_ctr, 19, 0);
			//set_gpio_output_value(gpio_ctr, 26, 0);

			if (key == 'w') {
				break;
			}

			else if (key == 'a') {
				thr_id = pthread_create(&p_thread[i], NULL, play, (void *)p0);
				keyboard_vector[0] = 1;
			}

			else if (key == 's') {
				thr_id = pthread_create(&p_thread[i], NULL, play, (void *)p1);
				keyboard_vector[1] = 1;
			}

			else if (key == 'd') {
				thr_id = pthread_create(&p_thread[i], NULL, play, (void *)p2);
				keyboard_vector[2]=1;
			}

			else if (key == 'f') {
				thr_id = pthread_create(&p_thread[i], NULL, play, (void *)p3);
				keyboard_vector[3]=1;
			}

			else if (key == 'j') {
				thr_id = pthread_create(&p_thread[i], NULL, play, (void *)p4);
				keyboard_vector[4] =1;
			}

			else if (key == 'k') {
				thr_id = pthread_create(&p_thread[i], NULL, play, (void *)p5);
				keyboard_vector[5]=1;
			}

			else if (key == 'l') {
				thr_id = pthread_create(&p_thread[i], NULL, play, (void *)p6);
				keyboard_vector[6] = 1;
			}

			else if (key == ';') {
				thr_id = pthread_create(&p_thread[i], NULL, play, (void *)p7);
				keyboard_vector[7]=1;
			}

			i++;
		} // piano sound end

		else if(inst_num == 1) { // second sound

			//set_gpio_output_value(gpio_ctr, 13, 0);
			//set_gpio_output_value(gpio_ctr, 19, 1);
			//set_gpio_output_value(gpio_ctr, 26, 0);

			if (key == 'w') {
				break;
			}

			else if (key == 'a') {
				thr_id = pthread_create(&p_thread[i], NULL, play, (void *)v0);
				keyboard_vector[0] = 1;
			}

			else if (key == 's') {
				thr_id = pthread_create(&p_thread[i], NULL, play, (void *)v1);
				keyboard_vector[1] = 1;
			}

			else if (key == 'd') {
				thr_id = pthread_create(&p_thread[i], NULL, play, (void *)v2);
				keyboard_vector[2]=1;
			}

			else if (key == 'f') {
				thr_id = pthread_create(&p_thread[i], NULL, play, (void *)v3);
				keyboard_vector[3] = 1;
			}

			else if (key == 'j') {
				thr_id = pthread_create(&p_thread[i], NULL, play, (void *)v4);
				keyboard_vector[4]=1;
			}

			else if (key == 'k') {
				thr_id = pthread_create(&p_thread[i], NULL, play, (void *)v5);
				keyboard_vector[5]=1;
			}

			else if (key == 'l') {
				thr_id = pthread_create(&p_thread[i], NULL, play, (void *)v6);
				keyboard_vector[6] =1;
			}

			else if (key == ';') {
				thr_id = pthread_create(&p_thread[i], NULL, play, (void *)v7);
				keyboard_vector[7] =1;
			}

			i++;
		} // second sound end

		else if(inst_num == 2) { // third sound

			//set_gpio_output_value(gpio_ctr, 13, 0);
			//set_gpio_output_value(gpio_ctr, 19, 0);
			//set_gpio_output_value(gpio_ctr, 26, 1);

			if (key == 'w') {
				break;
			}

			else if (key == 'a') {
				thr_id = pthread_create(&p_thread[i], NULL, play, (void *)t0);
				keyboard_vector[0]=1;
			}

			else if (key == 's') {
				thr_id = pthread_create(&p_thread[i], NULL, play, (void *)t1);
				keyboard_vector[1]=1;
			}

			else if (key == 'd') {
				thr_id = pthread_create(&p_thread[i], NULL, play, (void *)t2);
				keyboard_vector[2]=1;
			}

			else if (key == 'f') {
				thr_id = pthread_create(&p_thread[i], NULL, play, (void *)t3);
				keyboard_vector[3]=1;
			}

			else if (key == 'j') {
				thr_id = pthread_create(&p_thread[i], NULL, play, (void *)t4);
				keyboard_vector[4]=1;
			}

			else if (key == 'k') {
				thr_id = pthread_create(&p_thread[i], NULL, play, (void *)t5);
				keyboard_vector[5]=1;
			}

			else if (key == 'l') {
				thr_id = pthread_create(&p_thread[i], NULL, play, (void *)t6);
				keyboard_vector[6]=1;
			}

			else if (key == ';') {
				thr_id = pthread_create(&p_thread[i], NULL, play, (void *)t7);
				keyboard_vector[7]=1;
			}

			i++;
		} // third sound end
		
		if(RECORD_START){
			fprintf(fp, "%d %d %d %d %d %d %d %d ", keyboard_vector[0], keyboard_vector[1],
					keyboard_vector[2], keyboard_vector[3], keyboard_vector[4],
					keyboard_vector[5], keyboard_vector[6], keyboard_vector[7]);
		}
	}

	return 0;
}
