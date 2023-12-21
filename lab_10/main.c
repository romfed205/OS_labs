#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_READERS 10
#define BUFFER_SIZE 100

char buffer[BUFFER_SIZE];
pthread_mutex_t mutex;
pthread_cond_t condvar;
int recordCounter = 0;

void* writerThread(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);
        
        sprintf(buffer, "Record #%d", recordCounter);
        recordCounter++;
        
        pthread_cond_broadcast(&condvar);
        
        pthread_mutex_unlock(&mutex);
        
        sleep(1);
    }
    return NULL;
}

void* readerThread(void* arg) {
    long tid = (long)arg;
    while (1) {
        pthread_mutex_lock(&mutex);
        
        pthread_cond_wait(&condvar, &mutex);
        
		printf("Reader %ld tid %lx: %s\n", tid, pthread_self(),buffer);
        
        pthread_mutex_unlock(&mutex);
        
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t writer;
    pthread_t readers[NUM_READERS];
    
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&condvar, NULL);
    
    if (pthread_create(&writer, NULL, writerThread, NULL) != 0) {
        fprintf(stderr, "Writer thread creation error\n");
        return 0;
    }
    
    for (long i = 0; i < NUM_READERS; i++) {
        if (pthread_create(&readers[i], NULL, readerThread, (void*)i) != 0) {
            fprintf(stderr, "Reader thread creation error %ld\n", i);
            return 0;
        }
    }
    
    if (pthread_join(writer, NULL) != 0) {
        fprintf(stderr, "Writer thread waiting for termination error\n");
        return 0;
    }
    
    for (long i = 0; i < NUM_READERS; i++) {
        if (pthread_join(readers[i], NULL) != 0) {
            fprintf(stderr, "Reader thread waiting for termination error %ld\n", i);
            return 0;
        }
    }
    
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condvar);
    
    return 0;
}


