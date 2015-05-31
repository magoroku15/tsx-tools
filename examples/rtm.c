#define TYPE_DOUBLE
#include <malloc.h>
#include "immintrin.h"
//#include "rtm.h"        /* For gcc 4.8 use immintrin.h and -mrtm */


char *buf;

int
rtm_test(int sz)
{
        int status;
        int i,j;
#ifdef TYPE_DOUBLE
	double *dp = (double *) buf;
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


main()
{
        int sz, lastsz;

	buf = (char *) memalign(4096, 1024*1024);
        memset(buf, 0, 1024*1024);

        for (sz = 1024; sz < 1024 * 128; sz += 1024) {
                if (rtm_test(sz) == -1)
                        break;
                lastsz = sz;
        }

        while(1) {
                if (rtm_test(lastsz) == -1)
                        return -1;
        }
}
