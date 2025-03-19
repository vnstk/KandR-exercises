/* Vainstein K 2025mar17 */
/*Adapted from listing on p29.*/
#include <stdio.h>
#define MAXLINE 50

int getline (char s[], int lim)
{
	int c=EOF, i;
	for (i=0; i<lim-1 && (c=getchar())!=EOF && c!='\n'; ++i)
		s[i] = c;
	if (c == '\n') { /* Already hit EOL? */
		s[i] = c;
		++i;
		s[i] = '\0';
		return i;
	} else if (c == EOF) {
		s[i] = '\0';
		return i;
	} else if (i == lim-1) {
		s[i] = '\0';
		for (; (c=getchar())!=EOF && c!='\n'; ++i) {
			;
		}
		return ++i;
	}
}

void copy (char to[], char from[], int lim)
{
	int i = 0;
	while ((to[i] = from[i]) != '\0') {
		++i;
	}
}

main ()
{
	int len, max = 0;
	char line[MAXLINE], longest[MAXLINE];
	while ((len = getline(line, MAXLINE)) > 0)
		if (len > max) {
			max = len;
			copy(longest, line, MAXLINE);
		}
	if (max > 0) {
		printf("(Len=%d)  %s", max,longest);
		if (max > MAXLINE)
			printf("  ...and %d more.\n", max-MAXLINE);
	}
}
