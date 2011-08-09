#include "screen.h"
#include "os_stdio2.h"
void clearline() {
	int i;
	for  (i = 0; i < MAX_COLS; i++) {
		putc(' ');
	}
}
