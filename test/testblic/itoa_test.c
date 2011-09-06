#include <stdio.h>
#include "types.h"
#include "testlib.h"

char *itoa(int l, char *tol, int size);

int test_itoa(int l, char *tol, int size)
{
        char buf[12] = {0};
        const char *t = itoa(l, tol, size);
        snprintf(buf, sizeof(buf), "%d", l);
        printf("itoa(%d)=[%s]\n", l, t);
        return strcmp(buf, t) == 0;
}


const char *itohex(uint32_t c, char *s, int size, int upper);
int test_itohex(int l, int uppercase, char *tol, int size)
{
        const char *t = itohex(l, tol, size, uppercase);
        char buf[12] = {0};
	if (size >= 9) {
		tol[8] = 0;
	}
	{
		const char *fmt = (uppercase) ? "%X" : "%x";
		snprintf(buf, sizeof(buf), fmt, l);
	}
        printf("itohex(%s)=[%s]\n", buf, t);
	return strcmp(buf, t) == 0;
}

int itoa_testmain(int option)
{
        int rc = 0;
        char tol[12] = {0};
        int l = (1 << 31);
	/* itoa */
        rc = test_itoa(l, tol, sizeof(tol));
        ok_if(rc, "test(-max_int)");

        l = 0x7FFFFFFF;
        rc = test_itoa(l, tol, sizeof(tol));
        ok_if(rc, "test(max_int)");

        l = -1;
        rc = test_itoa(l, tol, sizeof(tol));
        ok_if(rc, "test(-1)");

        l = 0;
        rc = test_itoa(l, tol, sizeof(tol));
        ok_if(rc, "test(0)");

	/* itohex */
	rc = test_itohex(~0, 0,tol, sizeof(tol));
        ok_if(rc, "itohex test(~0)");
	rc = test_itohex(0x12, 0, tol, sizeof(tol));
        ok_if(rc, "itohex test 2");
	rc = test_itohex(0x67890000, 0, tol, sizeof(tol));
        ok_if(rc, "itohex test 3");
	rc = test_itohex(0xA7890000, 1, tol, sizeof(tol));
        ok_if(rc, "itohex test 3");
	rc = test_itohex(1<<31, 1, tol, sizeof(tol));
        ok_if(rc, "itohex test 4");

        return 0;
}

