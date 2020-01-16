//gcc -Wall -g -std=c99 -pthread mutex.c -o mutex

#include <stdio.h>
#include <pthread.h>

#define NUM_LOOPS 10000000
long long sum = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void summex(int offset)
{
    sum += offset;
}

void *counting_thread(void *arg)
{
    int offset = *(int *)arg;
    for(int i = 0; i < NUM_LOOPS; i++) {
        //start critical section
        pthread_mutex_lock(&mutex);

        summex(offset);

        //End critical section
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

int main(int argc, char const *argv[]) {
    //Spawn thread
    pthread_t id1;
    int offset1 = 1;
    pthread_create(&id1, NULL, counting_thread, &offset1);

    pthread_t id2;
    int offset2 = -1;
    pthread_create(&id2, NULL, counting_thread, &offset2);

    //wait for thread to finish
    pthread_join(id1, NULL);
    pthread_join(id2, NULL);

    printf("Sum = %lld\n", sum);

    return 0;
}
