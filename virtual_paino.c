#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <termio.h>
#include <signal.h>
#include <sys/poll.h>

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

	system("mpg321 sound/Piano_C4_30.mp3");
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

	system("mpg321 sound/Piano_D4_0.1.mp3");
	printf("Thread : %s\n", thread_name);
}

int main() {
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

	while(1) {
		int i = 0;
		char key = get_input(1);
		printf("%c\n", key);

		if (key == 'q') {
			break;
		}
		
		else if (key == 'a') {
			thr_id = pthread_create(&p_thread[i], NULL, play_C, (void *)p0);
			/** while(1) { */
			/**         int key2 = get_input(1); */
			/**         if (key2 != 'a') { */
			/**                 pthread_kill(p_thread[i], 0); */
			/**                 break; */
			/**         } */
			/** } */
		}

		else if (key == 's') {
			thr_id = pthread_create(&p_thread[i], NULL, play_D, (void *)p1);
		}

		i++;
	}

	return 0;
}
