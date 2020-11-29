#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <termio.h>
#include <signal.h>
#include <sys/poll.h>
#include <stdbool.h>
#include <string.h>
#include <sys/time.h>

bool keyboard_vector[8] = {0};
struct timeval start, cur;
int inst_number = 0;

char p0[] = "mpg321 -q sound/Piano_C4_0.5.mp3";
char p1[] = "mpg321 -q sound/Piano_D4_0.5.mp3";
char p2[] = "mpg321 -q sound/Piano_E4_0.5.mp3";
char p3[] = "mpg321 -q sound/Piano_F4_0.5.mp3";
char p4[] = "mpg321 -q sound/Piano_G4_0.5.mp3";
char p5[] = "mpg321 -q sound/Piano_A4_0.5.mp3";
char p6[] = "mpg321 -q sound/Piano_B4_0.5.mp3";
char p7[] = "mpg321 -q sound/Piano_C5_0.5.mp3";

char v0[] = "mpg321 -q sound/Violin_C4_0.5.mp3";
char v1[] = "mpg321 -q sound/Violin_D4_0.5.mp3";
char v2[] = "mpg321 -q sound/Violin_E4_0.5.mp3";
char v3[] = "mpg321 -q sound/Violin_F4_0.5.mp3";
char v4[] = "mpg321 -q sound/Violin_G4_0.5.mp3";
char v5[] = "mpg321 -q sound/Violin_A4_0.5.mp3";
char v6[] = "mpg321 -q sound/Violin_B4_0.5.mp3";
char v7[] = "mpg321 -q sound/Violin_C5_0.5.mp3";

char t0[] = "mpg321 -q sound/Trumpet_C4_0.5.mp3";
char t1[] = "mpg321 -q sound/Trumpet_D4_0.5.mp3";
char t2[] = "mpg321 -q sound/Trumpet_E4_0.5.mp3";
char t3[] = "mpg321 -q sound/Trumpet_F4_0.5.mp3";
char t4[] = "mpg321 -q sound/Trumpet_G4_0.5.mp3";
char t5[] = "mpg321 -q sound/Trumpet_A4_0.5.mp3";
char t6[] = "mpg321 -q sound/Trumpet_B4_0.5.mp3";
char t7[] = "mpg321 -q sound/Trumpet_C5_0.5.mp3";

void* play(void* file_dir) {
     pid_t pid;
     pthread_t tid;

     pid = getpid();
     tid = pthread_self();

     pthread_detach(tid);
	
 	system((char*)file_dir);
}

void* record_play(void* data){//저장된 벡터를 기반으로 음악 플레이
	FILE* fp = fopen("vector.txt", "r");
	char line[25];
	char time[8];
	float diff;
	pthread_t p_thread[20];
	int thr_id, i=0;

	fgets(line, sizeof(line), fp);
	strncpy(time, line+16, 7);
	time[7] = 0;
	diff = atof(time);
	usleep(diff*1000000);
	printf("sleep time : %f\n", diff);

	inst_number = data;
	printf("Instrument Number : %d\n", inst_number);

	while(fgets(line, sizeof(line), fp) != NULL){
		if(inst_number == 0) {
		if(line[0]=='1'){//'a'
			thr_id = pthread_create(&p_thread[i], NULL, play, (void*)p0);
		}
		else if(line[2]=='1'){//'s'
			thr_id = pthread_create(&p_thread[i], NULL, play, (void*)p1);	
		}
		else if(line[4]=='1'){//'d'
			thr_id = pthread_create(&p_thread[i], NULL, play, (void*)p2);
		}
		else if(line[6]=='1'){//'f'
			thr_id = pthread_create(&p_thread[i], NULL, play, (void*)p3);
		}
		else if(line[8]=='1'){//'j'
			thr_id = pthread_create(&p_thread[i], NULL, play, (void*)p4);
		}
		else if(line[10]=='1'){//'k'
			thr_id = pthread_create(&p_thread[i], NULL, play, (void*)p5);
		}
		else if(line[12]=='1'){//'l'
			thr_id = pthread_create(&p_thread[i], NULL, play, (void*)p6);
		}
		else if(line[14]=='1'){//';'
			thr_id = pthread_create(&p_thread[i], NULL, play, (void*)p7);
		}
		}

		else if(inst_number == 1) {
		if(line[0]=='1'){//'a'
			thr_id = pthread_create(&p_thread[i], NULL, play, (void*)v0);
		}
		else if(line[2]=='1'){//'s'
			thr_id = pthread_create(&p_thread[i], NULL, play, (void*)v1);	
		}
		else if(line[4]=='1'){//'d'
			thr_id = pthread_create(&p_thread[i], NULL, play, (void*)v2);
		}
		else if(line[6]=='1'){//'f'
			thr_id = pthread_create(&p_thread[i], NULL, play, (void*)v3);
		}
		else if(line[8]=='1'){//'j'
			thr_id = pthread_create(&p_thread[i], NULL, play, (void*)v4);
		}
		else if(line[10]=='1'){//'k'
			thr_id = pthread_create(&p_thread[i], NULL, play, (void*)v5);
		}
		else if(line[12]=='1'){//'l'
			thr_id = pthread_create(&p_thread[i], NULL, play, (void*)v6);
		}
		else if(line[14]=='1'){//';'
			thr_id = pthread_create(&p_thread[i], NULL, play, (void*)v7);
		}

		}

		else if(inst_number == 2) {
		if(line[0]=='1'){//'a'
			thr_id = pthread_create(&p_thread[i], NULL, play, (void*)t0);
		}
		else if(line[2]=='1'){//'s'
			thr_id = pthread_create(&p_thread[i], NULL, play, (void*)t1);	
		}
		else if(line[4]=='1'){//'d'
			thr_id = pthread_create(&p_thread[i], NULL, play, (void*)t2);
		}
		else if(line[6]=='1'){//'f'
			thr_id = pthread_create(&p_thread[i], NULL, play, (void*)t3);
		}
		else if(line[8]=='1'){//'j'
			thr_id = pthread_create(&p_thread[i], NULL, play, (void*)t4);
		}
		else if(line[10]=='1'){//'k'
			thr_id = pthread_create(&p_thread[i], NULL, play, (void*)t5);
		}
		else if(line[12]=='1'){//'l'
			thr_id = pthread_create(&p_thread[i], NULL, play, (void*)t6);
		}
		else if(line[14]=='1'){//';'
			thr_id = pthread_create(&p_thread[i], NULL, play, (void*)t7);
		}

		}

		strncpy(time, line+16, 7);
		time[7] = 0;
		diff = atof(time);
		printf("sleep time : %f\n", diff);
		usleep(diff*1000000);
		
		i++;
	}
	printf("Playing End\n");

	fclose(fp);
}

int get_input(int is_echo){
	int ch;
	struct termios old;
	struct termios current;

	tcgetattr(0, &old);
	current = old;
	current.c_lflag &= ~ICANON;

	if(is_echo){
		current.c_lflag |= ECHO;
	}
	else{
		current.c_lflag &= ~ECHO;
	}
	tcsetattr(0, TCSANOW, &current);
	ch = getchar();
	tcsetattr(0, TCSANOW, &old);

	return ch;
}

void* record(void* data) {
	pthread_t p_thread[8];
	int thr_id;
	int i =0;
	long diff_sec, diff_usec;
	FILE* fp = fopen("vector.txt", "w");
	struct timeval start, cur;
	float diff;
	printf("Press 'q' to end the recording\n");
	gettimeofday(&start, NULL);
	fprintf(fp, "0 0 0 0 0 0 0 0 ");

	while(1) {
		
		//initialize
		for(i=0; i<8; i++){
			keyboard_vector[i] = false;
		}

		char key = get_input(1);
		gettimeofday(&cur, NULL);
		diff_sec = cur.tv_sec - start.tv_sec;
		diff_usec = cur.tv_usec - start.tv_usec;
		diff = diff_usec/1000000.0f + diff_sec;
		fprintf(fp, "%.3f\n", diff);
		start = cur;

		if(key == 'q'){ //quit
			printf("Recording End\n");
			break;
		}
		else if(key == 'a'){
			thr_id = pthread_create(&p_thread[i], NULL, play, (void*)p0);
			keyboard_vector[0] = 1;
		}
		else if(key == 's'){
			thr_id = pthread_create(&p_thread[i], NULL, play, (void*)p1);
			keyboard_vector[1] = 1;
		}
		else if(key =='d'){
			thr_id = pthread_create(&p_thread[i], NULL, play, (void*)p2);
			keyboard_vector[2] = 1;
		}
		else if(key == 'f'){
			thr_id = pthread_create(&p_thread[i], NULL, play, (void*)p3);
			keyboard_vector[3] = 1;
		}
		else if(key == 'j'){
			keyboard_vector[4] = 1;
			thr_id = pthread_create(&p_thread[i], NULL, play, (void*)p4);
		}
		else if(key == 'k'){
			keyboard_vector[5] = 1;
			thr_id = pthread_create(&p_thread[i], NULL, play, (void*)p5);
		}
		else if(key == 'l'){
			keyboard_vector[6] = 1;
			thr_id = pthread_create(&p_thread[i], NULL, play, (void*)p6);
		}
		else if(key == ';'){
			keyboard_vector[7] = 1;
			thr_id = pthread_create(&p_thread[i], NULL, play, (void*)p7);
		}
		i++;
		fprintf(fp, "%d %d %d %d %d %d %d %d ", keyboard_vector[0], keyboard_vector[1],
				keyboard_vector[2], keyboard_vector[3], keyboard_vector[4], keyboard_vector[5],
				keyboard_vector[6], keyboard_vector[7]);
	}
	
	fclose(fp);
}


/*
int main(){	
	
	if(1// 특정 버튼이 눌린다면
			){
		record();
	}
	

	if(1//특정 버튼이 눌린다면
			){
		record_play();
	}

	return 0;
}

*/
