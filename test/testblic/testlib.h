#ifndef TESTSHARED_H
#define TESTSHARED_H

#define ok_if(expr, msg) { if (expr) \
                                {  ; printf("%s:[passed]\n", msg); } \
                           else \
                                {  return -1; printf("%s:[failed]\n", msg); }}

#endif /* TESTSHARED_H */
