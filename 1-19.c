/* Vainstein K 2025mar17 */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#define BUFLEN 1024 /*Regrettably, a buffer is needed; and buffers are finite.*/
/*

A
AB
ABC
ABCD
*/

void reverse (char *const s)
{
	const int slen = strlen(s);
	int iL, iR;
	for (iL = 0, iR = slen - 1; iL < iR; ++iL, --iR) {
		const char tmp = s[iL];
		s[iL] = s[iR];
		s[iR] = tmp;
	}
}

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
	reverse(buf);
	printf("%s\n", buf);
	return c != EOF;
}

main ()
{
	while (getline())
		;
}
