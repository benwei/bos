#include <stdio.h>
#include "testlib.h"

char *itoa(int l, char *tol, int size);

int test_itoa(int l, char *tol, int size)
{
        char buf[12] = {0};
        const char *t = itoa(l, tol, size);
        printf("itoa(l)=[%s]\n", t);
        snprintf(buf, sizeof(buf), "%d", l);
        printf("printf(l)=[%s]\n", buf);
        return strcmp(buf, t) == 0;
}

int itoa_testmain(int option)
{
        int rc = 0;
        char tol[12] = {0};
        int l = (1 << 31);
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

        return 0;
}

