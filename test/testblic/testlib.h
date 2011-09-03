#ifndef TESTSHARED_H
#define TESTSHARED_H

#define ok_if(expr, msg) { if (expr) \
                                {  printf("%s:[passed]\n", msg); } \
                           else \
                                {  printf("%s:[failed]\n", msg); return -1; }}

#endif /* TESTSHARED_H */
