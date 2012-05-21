#include <stdio.h>
#include <pthread.h>

#ifndef MAX_LOCK_TIME
#define MAX_LOCK_TIME 2 
#endif

void spinlock(void *lock);
void spinunlock(void *lock);

static int glock = 0;
static int glock2 = 0;

static void thr_foolock(void *arg) {
	printf("acquire lock\n");
	spinlock(&glock);	
	printf("foolock released\n");
}

static void thr_foounlock(void *arg) {
	int counter = MAX_LOCK_TIME;
	printf("foounlock releasing lock\n");
	while (--counter) {
		printf("block remaining %d seconds\n", counter);
		sleep(1);
	}

	printf("foounlock left\n");
	spinunlock(&glock);	
}

#define TEST_THREAD_MAX 2

typedef void *(*thr_func_t)(void *) ;

int main(int argc, char **argv)
{
	pthread_t thread[TEST_THREAD_MAX]= {0};
	void *thread_ret[TEST_THREAD_MAX] = {0};
	int i ; 

	pthread_create(&thread[0], NULL, (thr_func_t) &thr_foolock, NULL); 
	pthread_create(&thread[1], NULL, (thr_func_t) &thr_foounlock, NULL); 

	for (i = 0; i < TEST_THREAD_MAX; i ++)
		pthread_join(thread[i], &thread_ret[i]);
	return 0;
}
