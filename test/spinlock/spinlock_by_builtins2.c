/*
* referece from http://stackoverflow.com/questions/6935442/x86-spinlock-using-cmpxchg
*/

void spinlock(int *p)
{
    while(!__sync_bool_compare_and_swap(p, 0, 1));
}

void spinunlock(int volatile *p)
{
    /* acts as a memory barrier. */
    /* asm volatile ("");    comment out the following line for comparison */
    *p = 0;
}
