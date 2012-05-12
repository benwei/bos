
/* sample code for div zero */

int
user_div_zero(void)
{
	int a = 0;
	int ret = 1;
	ret = ret / a;
	return ret;
}
