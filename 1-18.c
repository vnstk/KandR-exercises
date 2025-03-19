/* Vainstein K 2025mar17 */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#define BUFLEN 1024 /*Regrettably, a buffer is needed; and buffers are finite.*/

bool getline ()
{
	char buf[BUFLEN];
	memset(buf, '\0', sizeof buf); 
	int c=EOF, i;
	for (i=0; (c=getchar())!=EOF && c!='\n'; ++i) {
		if (i >= BUFLEN-2) {
			fprintf(stderr, "Buffer too small.\n");
			return false;
		}
		buf[i] = c;
	}
	if (c == EOF) {
		return false;
	}
	if (i == 0) /*Empty __to begin with__*/ {
		return true;
	}
	/* Need to undo that one last "++i" step. */
	--i;
	while (i >= 0 && (buf[i] == ' ' || buf[i] == '\t')) {
		buf[i] = '\0';
		--i;
	}
	if (i < 0) { /* Empty __once trailing whtspc trimmed__ */
		return true;
	}
	printf("%s\n", buf);
	return c != EOF;
}

main ()
{
	while (getline())
		;
}
