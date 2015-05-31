#include <stdlib.h>
#include "rtm-goto.h"

char buf[1024*1024];

char *
xabort_code(int xst)
{
	//int status = XABORT_CODE(xst);
	int status = xst;

	if (status & XABORT_RETRY) 
		return "EXPLICIT";
	else if (status & XABORT_RETRY) 
		return "RETRY";
	else if (status & XABORT_CONFLICT) 
		return "CONFLICT";
	else if (status & XABORT_CAPACITY) 
		return "CAPACITY";
	else if (status & XABORT_DEBUG) 
		return "DEBUG";
	else if (status & XABORT_NESTED) 
		return "NESTEST";
	else 
		return "Unknown"; 	
}

int
rtm_test(int sz)
{
	unsigned status = 0;
	int i,j;

	//memset(buf, 0, 1024*1024);

	XBEGIN(abort_handler);
	 for (i = 0; i < sz; i++) {
	 	buf[i] = i;
	 }
	XEND(); 
	printf("%d pass\n",sz);
	return 0;

	XFAIL_STATUS(abort_handler, status);
	printf("%d aborted %x, %d\n", sz, status, XABORT_CODE(status));
	//printf("%d fail %x %s\n",sz, status, xabort_code(status));
	return -1;
}

main()
{
	int sz, lastsz;

	for (sz = 1024; sz < 1024 * 16; sz += 256) {
		if (rtm_test(sz) == -1)
			break;
		lastsz = sz;	
	}
	while(1) {
		if (rtm_test(lastsz) == -1)
			;
	}
}

