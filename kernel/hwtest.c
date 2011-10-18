

#define __hwsetup __attribute__ ((__section__(".hwsetup.text")))
#define __hwdata __attribute__ ((__section__(".hwsetup.data")))

__hwsetup void hello_setup(void)
{
    int a = 0;
        a++;
}

void hello_public_text(void)
{
}

__hwdata void hello_data(void)
{
    int a = 0;
        a++;
}
