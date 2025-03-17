
/* Vainstein K 2025mar17 */
#include <stdbool.h>
#include <stdio.h>
main () {
	char ch;
	bool inSeparatorStreak=true; /* So that doesn't print LF if input **starts with** a separator. */
	while ((ch = getchar()) != EOF) {
		switch (ch) {
			case ' ':
			case '\t':
			case '\n':
				if (!inSeparatorStreak) {
					putchar('\n');
					inSeparatorStreak=true;
				}
				break;
			default:
				putchar(ch);
				inSeparatorStreak=false;
		}
	}
}
