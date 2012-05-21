/*
* referece from http://stackoverflow.com/questions/6935442/x86-spinlock-using-cmpxchg
*/

void spinlock(int *p)
{
    while(!__sync_bool_compare_and_swap(p, 0, 1));
}

void spinunlock(int volatile *p)
{
    asm volatile ("":::"memory"); // acts as a memory barrier.
    *p = 0;
}
