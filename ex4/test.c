#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include "threadPool.h"

void  function1();
void  function2();
void  function3();
void  function1WithSleep();
void  function2Sleep();


//המספרים צריכים להיות מודפסים בצורה סנכרונית, קודם 11..1 ואז 2...2 ואז 3...3
/*int main() {
    ThreadPool* threadPool =tpCreate(1);
    char * args = (char *)malloc(10);
    tpInsertTask(threadPool,function1,args);
    tpInsertTask(threadPool,function2,args);
    tpInsertTask(threadPool,function3,args);
    int temp;
    scanf("%d",&temp);
    free(args);
    return 0;
}*/

//מספרים מודפסים באיזה סדר שבא להם
/*int main() {
    ThreadPool* threadPool =tpCreate(3);
    char * args = (char *)malloc(10);
    tpInsertTask(threadPool,function1,args);
    tpInsertTask(threadPool,function2,args);
    tpInsertTask(threadPool,function3,args);
    int temp;
    scanf("%d",&temp);
    free(args);
    return 0;
}*/


//מספרים מודפסים באיזה סדר שבא להם
/*int main() {
    ThreadPool* threadPool =tpCreate(3);
    char * args = (char *)malloc(10);
    tpInsertTask(threadPool,function1WithSleep,args);
    tpInsertTask(threadPool,function2Sleep,args);
    tpInsertTask(threadPool,function3,args);
    int temp;
    scanf("%d",&temp);
    free(args);
    return 0;
}*/

//דסטרוי רגיל, חשוב לשים לב שכל הפונקציות מסתיימות לפני הדסטרוי
/*int main() {
    ThreadPool *threadPool = tpCreate(3);
    char *args = (char *) malloc(10);
    tpInsertTask(threadPool, function1, args);
    tpInsertTask(threadPool, function2, args);
    tpInsertTask(threadPool, function3, args);
    tpDestroy(threadPool, 0);
    int temp;
    scanf("%d", &temp);
    free(args);
    return 0;
}*/



//////////////////////////
// דסטרוי רגיל, רק המתודה הראשונה אמורה להתבצע
/*
int main() {
    ThreadPool *threadPool = tpCreate(1);
    char *args = (char *) malloc(10);

    tpInsertTask(threadPool, function1WithSleep, args);
    tpInsertTask(threadPool, function2Sleep, args);
    tpInsertTask(threadPool, function3, args);

    sleep(20);
    tpDestroy(threadPool, 0);
    int temp;

    printf("give in the tenp:\n");
    scanf("%d", &temp);
    free(args);
    return 0;
}

//דסטרוי מיוחד, יש לוודא שכל המשימות מתבצעות
/*int main() {
    ThreadPool *threadPool = tpCreate(1);
    char *args = (char *) malloc(10);
    tpInsertTask(threadPool, function1WithSleep, args);
    tpInsertTask(threadPool, function2Sleep, args);
    tpInsertTask(threadPool, function3, args);
    tpDestroy(threadPool, 1);
    int temp;
    scanf("%d", &temp);
    free(args);
    return 0;
}*/

//בדיקה שאחרי שקראנו לדסטרוי לא ניתן להוסיף משימות
int main() { ////////////////////////////////////////////////////////////////////check this one again
    ThreadPool *threadPool = tpCreate(1);
    char *args = (char *) malloc(10);
    tpInsertTask(threadPool, function1WithSleep, args);
    tpInsertTask(threadPool, function2Sleep, args);
    printf("can add its %d \n",tpInsertTask(threadPool, function3, args));
    printf("destory\n");
    tpDestroy(threadPool, 1);
    printf("cant add its %d \n",tpInsertTask(threadPool, function3, args));
    int temp;
    scanf("%d", &temp);
    free(args);
    return 0;
}
/*
void hello (void* a)
{
    printf("hello\n");
    sleep(3);
}
void test_thread_pool_sanity()
{
    int i;
    ThreadPool* tp = tpCreate(5);
    for(i=0; i<10; ++i)
    {
        tpInsertTask(tp,hello,NULL);
    }
    sleep(5);
    tpDestroy(tp,1);
}
int main(){
    test_thread_pool_sanity();
    int y;
    scanf("%d",&y);
    return 0;
}*/



/*void hello (void* a)
{
    printf("hello\n");
}
void test_thread_pool_sanity()
{
    int i;
    ThreadPool* tp = tpCreate(5);
    for(i=0; i<5; ++i)
    {
        tpInsertTask(tp,hello,NULL);
    }
    tpDestroy(tp,0);
}
int main()
{
    test_thread_pool_sanity();
    return 0;
}*/


#include <stdio.h>
#include <stdlib.h>
#include "osqueue.h"
#include "threadPool.h"





void  function3() {
    int i;
    for(i=1; i<100;i++) {
        printf("3\n");

    }
}

void  function1() {
    int i;
    for(i=1; i<100;i++) {
        printf("1\n");

    }
}
void  function1WithSleep() {
    int i;
    for(i=1; i<10;i++) {
        printf("1\n");
        sleep(1);

    }
}

void  function2() {
    int i;
    for(i=1; i<100;i++) {
        printf("2\n");
    }
}
void  function2Sleep() {
    int i;
    for(i=1; i<10;i++) {
        printf("2\n");
        sleep(1);
    }
}


void * function3Sleep() {
    int i;
    for(i=1; i<100;i++) {
        printf("3\n");
        sleep(1);

    }
}