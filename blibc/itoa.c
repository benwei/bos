
char *itoa(int l, char *tol, int size)
{
	int r;
	int neg = 0;
	int count = 0;
	int x = l;
	if (size < 2) {
		*tol = 0;
		return tol;
	}

	if (l == 0) {
		tol[0] = '0';
		tol[1] = '\0';
		return tol;
	} else if (x < 0) {
		neg = 1;
	}

	count = size - 2;
	while (x != 0 && count > 0) {
		r = x % 10;
		tol[count--] = '0' + ((r > 0) ? r : -r);
		x /= 10;
	}
	if (neg) {
		tol[count] = '-';
	} else {
		count++;
	}
	return tol + count;/* num only without sign and prefix '0' */
}
