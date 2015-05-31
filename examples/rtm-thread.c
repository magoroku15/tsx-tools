#define _GNU_SOURCE
#include <pthread.h>  
#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <assert.h>  
#include <time.h>  
#include <sched.h>
#include "immintrin.h"


struct data {
    int tnumber;
    int data1;
    int *data2;
};

#define NCORE 4

#define BUFSZ 2048
#define TLSBUFSZ ((BUFSZ)+sizeof(struct data))
__thread char tlsbuf[TLSBUFSZ];

int
rtm_test(int sz)
{
        int status;
        int i,j;
#ifndef TYPE_DOUBLE
	char *buf = (char *) &tlsbuf[sizeof(struct data)]; 
#else
	double *dp = (double *) &tlsbuf[sizeof(struct data)];
#endif
retry:
        if ((status = _xbegin()) == _XBEGIN_STARTED) {

#ifndef TYPE_DOUBLE
		for (i = 0; i < sz; i++) {
			buf[i] = i;
		}
#else
		for (i = 0; i < sz/8; i++) {
			dp[i] = i + 1;
		}

		for (i = 0; i < sz/16; i++) {
			dp[i] = dp[sz/16] + dp[i];
		}

		for (i = 0; i < sz/16; i++) {
			dp[i] = dp[sz/16] * dp[i];
		}
#endif
                _xend();
		printf("%d pass\n" ,sz);
		return 0;
        } else {
/*
		if (status == 0) {
			printf("%d pass\n" ,sz);
			return 0;
		} else 
*/
		{
			printf("%d aborted %x, %d\n", sz, status, _XABORT_CODE(status));
			//goto retry;
			return -1;
		}
	}
}

  
void func()  
{  
    int sz, lastsz=0;
    struct data* pdata = (struct data*)tlsbuf;  

    printf("tnumber = %d, data2 = %8.8p, data1 = %d.\n",   
        pdata->tnumber, pdata->data2, pdata->data1);  
    fflush(stdout);   

    for (sz = 128; sz <= 1024 ; sz += 128) {
        if (rtm_test(sz) == -1)
            break;
        lastsz = sz;
    }
    while(1) {
        if (rtm_test(lastsz) == -1)
            break;
    }
}  
  

void* threadfunc(void* p)  
{  
    int id,i;  
    struct data* pdata = (struct data*)tlsbuf;  

    id = *((int *)p);

    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(id, &mask);
    if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
      perror("setaffinity:");
      exit(1);
    }
  
    pdata->tnumber = id;
    pdata->data1 = rand();  
    pdata->data2 = (int *) pdata;
      
    for ( i = 0 ; i < 5 ; i )  
    {  
        func();  
        //sleep(rand()%2);  
    }  
  
    return NULL;  
}  

  

int main(int argc, char** argv)  
{  
    pthread_t threads[NCORE];  
    int  id[NCORE];  
    int i;  
    srand(time(NULL));  
  
    for ( i=0 ; i < NCORE ; i++ )  
    {  
	id[i] = i;
        if (pthread_create(&threads[i], NULL, threadfunc, (void*)&id[i]) != 0 )  
            exit(1);  
    }  
  
    for ( i=0 ; i < NCORE ; i++ )  
    {  
        if (pthread_join(threads[i], NULL) != 0 )  
            exit(1);  
    }  
      
    return 0;  
}  

