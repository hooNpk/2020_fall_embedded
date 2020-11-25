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

int get_input(int is_echo) {
	int ch;
	struct termios old;
	struct termios current;

	tcgetattr(0, &old);

	current = old;

	current.c_lflag &= ~ICANON;

	if (is_echo) {
		current.c_lflag |= ECHO;
	} else {
		current.c_lflag &= ~ECHO;
	}

	tcsetattr(0, TCSANOW, &current);
	ch = getchar();
	tcsetattr(0, TCSANOW, &old);

	return ch;
}

void* play_C(void *data) {
	pid_t pid;
	pthread_t tid;
	/** static int retval = 999; */
	/** struct pollfd fds[1]; */
	/** int ret; */

	/** fds[0].fd = STDIN_FILENO; */
	/** fds[0].events = POLLIN; */
        /**  */
	/** fds[1].fd = STDOUT_FILENO; */
	/** fds[1].events = POLLOUT; */

	pid = getpid();
	tid = pthread_self();

	pthread_detach(tid);

	char* thread_name = (char*)data;
	char buf[1024];

	system("mpg321 sound/Piano_C4_0.5.mp3");
	printf("Thread : %s\n", thread_name);
        /**  */
	/** while(1) { */
	/**         ret = poll(fds, 2, 500); */
	/**         ret = poll(fds, 1, 500); */
        /**  */
	/**         if(!ret) { */
	/**                 pthread_exit((void*)&retval); */
	/**         } */
        /**  */
	/**         if(fds[0].revents & POLLIN) { */
	/**                 int len = read(STDIN_FILENO, buf, 1024); */
	/**                 if(len) { */
	/**                         if(*buf != 'a') { */
	/**                                 break; */
	/**                         } */
	/**                 } */
	/**         } */
	/** } */
}

void* play_D(void *data) {
	pid_t pid;
	pthread_t tid;

	pid = getpid();
	tid = pthread_self();

	pthread_detach(tid);

	char* thread_name = (char*)data;

	system("mpg321 sound/Piano_D4_0.5.mp3");
	printf("Thread : %s\n", thread_name);
}

void* play_E(void *data) {
	pid_t pid;
	pthread_t tid;

	pid = getpid();
	tid = pthread_self();

	pthread_detach(tid);

	char* thread_name = (char*)data;

	system("mpg321 sound/Piano_E4_0.5.mp3");
	printf("Thread : %s\n", thread_name);
}

void* play_F(void *data) {
	pid_t pid;
	pthread_t tid;

	pid = getpid();
	tid = pthread_self();

	pthread_detach(tid);

	char* thread_name = (char*)data;

	system("mpg321 sound/Piano_F4_0.5.mp3");
	printf("Thread : %s\n", thread_name);
}

void* play_G(void *data) {
	pid_t pid;
	pthread_t tid;

	pid = getpid();
	tid = pthread_self();

	pthread_detach(tid);

	char* thread_name = (char*)data;

	system("mpg321 sound/Piano_G4_0.5.mp3");
	printf("Thread : %s\n", thread_name);
}

void* play_A(void *data) {
	pid_t pid;
	pthread_t tid;

	pid = getpid();
	tid = pthread_self();

	pthread_detach(tid);

	char* thread_name = (char*)data;

	system("mpg321 sound/Piano_A4_0.5.mp3");
	printf("Thread : %s\n", thread_name);
}

void* play_B(void *data) {
	pid_t pid;
	pthread_t tid;

	pid = getpid();
	tid = pthread_self();

	pthread_detach(tid);

	char* thread_name = (char*)data;

	system("mpg321 sound/Piano_B4_0.5.mp3");
	printf("Thread : %s\n", thread_name);
}

void* play_C5(void *data) {
	pid_t pid;
	pthread_t tid;

	pid = getpid();
	tid = pthread_self();

	pthread_detach(tid);

	char* thread_name = (char*)data;

	system("mpg321 sound/Piano_C5_0.5.mp3");
	printf("Thread : %s\n", thread_name);
}

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

int main() {
	#define PERIPHERAL_BASE 0x3F000000UL
	#define GPIO_BASE (PERIPHERAL_BASE + 0x200000)

	int fdmem = open("/dev/mem", O_RDWR);
	if (fdmem < 0) {
		printf("Error opening /dev/mem\n");
		return -1;
	}

	void* gpio_ctr = mmap(0, 4096, PROT_READ + PROT_WRITE, MAP_SHARED, fdmem, GPIO_BASE);
	if (gpio_ctr == MAP_FAILED) {
		printf("mmap error\n");
		return -1;
	}

	pthread_t p_thread[7];
	int thr_id;
	int status;

	char p0[] = "Thread 0";
	char p1[] = "Thread 1";
	char p2[] = "Thread 2";
	char p3[] = "Thread 3";
	char p4[] = "Thread 4";
	char p5[] = "Thread 5";
	char p6[] = "Thread 6";
	char p7[] = "Thread 7";

	set_gpio_output(gpio_ctr, 13);
	set_gpio_output(gpio_ctr, 19);
	set_gpio_output(gpio_ctr, 26);

	set_gpio_input(gpio_ctr, 4);
	set_gpio_pullup(gpio_ctr, 4);

	int gpio_4_value = 0;
	int gpio_flag = 0;

	while(1) {
		int i = 0;
		char key = get_input(1);
		printf("%c\n", key);

		if (key == 'q') {
			break;
		}
		
		else if (key == 'a') {
			thr_id = pthread_create(&p_thread[i], NULL, play_C, (void *)p0);
		}

		else if (key == 's') {
			thr_id = pthread_create(&p_thread[i], NULL, play_D, (void *)p1);
		}

		else if (key == 'd') {
			thr_id = pthread_create(&p_thread[i], NULL, play_E, (void *)p2);
		}

		else if (key == 'f') {
			thr_id = pthread_create(&p_thread[i], NULL, play_F, (void *)p3);
		}

		else if (key == 'j') {
			thr_id = pthread_create(&p_thread[i], NULL, play_G, (void *)p4);
		}

		else if (key == 'k') {
			thr_id = pthread_create(&p_thread[i], NULL, play_A, (void *)p5);
		}

		else if (key == 'l') {
			thr_id = pthread_create(&p_thread[i], NULL, play_B, (void *)p6);
		}

		else if (key == ';') {
			thr_id = pthread_create(&p_thread[i], NULL, play_C5, (void *)p7);
		}

		i++;

		get_gpio_input_value(gpio_ctr, 4, &gpio_4_value);
		if (gpio_4_value && gpio_flag == 0) {
			set_gpio_output_value(gpio_ctr, 13, 1);
			set_gpio_output_value(gpio_ctr, 19, 0);
			set_gpio_output_value(gpio_ctr, 26, 0);
			gpio_flag++;
		}

		else if (gpio_4_value && gpio_flag == 1) {
			set_gpio_output_value(gpio_ctr, 13, 0);
			set_gpio_output_value(gpio_ctr, 19, 1);
			set_gpio_output_value(gpio_ctr, 26, 0);
			gpio_flag++;
		}

		else if (gpio_4_value && gpio_flag == 2) {
			set_gpio_output_value(gpio_ctr, 13, 0);
			set_gpio_output_value(gpio_ctr, 19, 0);
			set_gpio_output_value(gpio_ctr, 26, 1);
			gpio_flag++;
		}

		else if (gpio_4_value && gpio_flag == 3) {
			set_gpio_output_value(gpio_ctr, 13, 0);
			set_gpio_output_value(gpio_ctr, 19, 0);
			set_gpio_output_value(gpio_ctr, 26, 0);
			gpio_flag = 0;
		}
	}

	return 0;
}
