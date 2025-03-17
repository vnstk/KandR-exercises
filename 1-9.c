/* Vainstein K 2025mar17 */
/* Invoke as e.g.
                   ./a.exe < 1-7.c
*/
#include <stdbool.h>
#include <stdio.h>
main () {
	bool inSpcStreak=false;
	char ch;
	while ((ch = getchar()) != EOF) {
		if (ch == ' ') {
			if (! inSpcStreak) {
				putchar(ch);
				inSpcStreak=true;
			}
		} else {
			putchar(ch);
			inSpcStreak=false;
		}
	}
}
