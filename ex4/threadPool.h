//Shay Tzirin
//315314930

#ifndef __THREAD_POOL__
#define __THREAD_POOL__

#include <sys/types.h>
#include "osqueue.h"

#define TRUE 1
#define FALSE 0

typedef struct task
{
    void (*func)(void* arg);
    void* arg;
}Task;

/**
 * threadpool struct
 */
typedef struct thread_pool
{
    //The field x is here because a struct without fields
    //doesn't compile. Remove it once you add fields of your own
    int numOfThreads;
    OSQueue* taskQue;
    pthread_t* threads;
    int cantAdd;
    int destroyedCompletely;
    int waitForAllTask;
    pthread_mutex_t tpLock;
    pthread_cond_t condForBusy;
}ThreadPool;

ThreadPool* tpCreate(int numOfThreads);
void tpDestroy(ThreadPool* threadPool, int shouldWaitForTasks);
int tpInsertTask(ThreadPool* threadPool, void (*computeFunc) (void *), void* param);

#endif
