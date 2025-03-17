/* Vainstein K 2025mar17 */
#include <stdio.h>
main () {
	char ch;
	while ((ch = getchar()) != EOF) {
		if (ch == '\t') { /* HT */
			putchar('\\');
			putchar('t');
		} else if (ch == '\b') { /* BS; to insert, Ctl-V Ctl-H */
/*XY*/
			putchar('\\');
			putchar('b');
		} else if (ch == '\\') {
			putchar('\\');
			putchar('\\');
		} else
			putchar(ch);
	}
}
