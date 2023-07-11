#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#define BUFFER_SIZE 2

//initializing mutexes before use
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;


int buffer[BUFFER_SIZE];
int buffer_count = 0;
int total = 0;
int threadcounter = 0;
int condition_flag = 0;

void* producerthread1(void* arg) 
{
    while(1)
    {
        //lock m2
        pthread_mutex_lock(&mutex2);
        //generates a number between 0-100
        int num = rand() % 101;
        printf("Thread 1 generated %d\n", num);
        //add the number 
        total += num;
        //add 1 to the thread counter 
        threadcounter++;
        //unlock m2
        pthread_mutex_unlock(&mutex2);
        
        if (threadcounter == BUFFER_SIZE) 
        {
            pthread_mutex_lock(&mutex1);
            condition_flag = 1;
            pthread_cond_signal(&cond1);
            pthread_mutex_unlock(&mutex1);
            threadcounter = 0;
        }
        //sleep for a second
        sleep(1);
    }
}

void* producerthread2(void* arg) 
{
    while (1) 
    {
        //lock m2
        pthread_mutex_lock(&mutex2);
        //generated between 0-100
        int num = rand() % 101;
        printf("Thread 2 generated %d\n", num);
        //subtract the num from total
        total -= num;
        threadcounter++;
        //unlock m2
        pthread_mutex_unlock(&mutex2);

        if (threadcounter == BUFFER_SIZE)
        {
            pthread_mutex_lock(&mutex1);
            condition_flag = 1;
            pthread_cond_signal(&cond1);
            pthread_mutex_unlock(&mutex1);
            threadcounter = 0;
        }
        //sleeps for a second
        sleep(1);
    }
}

void* consumerthread(void* arg)
{
    //loop forever 
    while (1)
    {
        //lock m1
        pthread_mutex_lock(&mutex1);
        
        while (condition_flag == 0)
        {
            pthread_cond_wait(&cond1, &mutex1);
        }
        //display running total
        printf("Running total is %d\n", total);
        //set condition flag and unlock m1
        condition_flag = 0;
        pthread_mutex_unlock(&mutex1);
    }
}

int main()
{
    pthread_t producer1_thread, producer2_thread, consumer_thread;
    //seeds the random generator
    srand(time(NULL));
    //spawns producer and consumer threads
    pthread_create(&producer1_thread, NULL, producerthread1, NULL);
    pthread_create(&producer2_thread, NULL, producerthread2, NULL);
    pthread_create(&consumer_thread, NULL, consumerthread, NULL);
    
    //join all 3 threads
    pthread_join(producer1_thread, NULL);
    pthread_join(producer2_thread, NULL);
    pthread_join(consumer_thread, NULL);

    return 0;
}
