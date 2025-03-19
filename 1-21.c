/* Vainstein K 2025mar18 */
/*Re "When either a tab or a single space would suffice to reach a tab stop, which should be given preference?"  ===>  Decided thus: prefer a single space. */
/* NB: given input.txt,
                                       gcc 1-21.c && ./a.out 8 <input.txt
and
                                       unexpand --all -t 8 <input.txt
should give same output.  Also handy,
                                       cat -A
*/
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#define BUFLEN 1024

bool getline (char *const buf, const int buflen)
{
	memset(buf, '\0', buflen);
	int c=EOF, i;
	for (i=0; (c=getchar())!=EOF && c!='\n'; ++i) {
		if (i >= buflen-2) {
			fprintf(stderr, "Buffer too small.\n");
			return false;
		}
		buf[i] = c;
	}
	return c != EOF;
}

static const int DFLT_tabstopWidth = 8;
int decide_tabstopWidth (const int argc, char *argv[])
{
	if (argc > 1) {
		const int argVal = atoi(argv[1]);
		if (argVal)
			return argVal;
	}
	return DFLT_tabstopWidth;
}

main (int argc, char *argv[])
{
	const int tabstopWidth = decide_tabstopWidth(argc,argv);
	int iSpaceStreakStarted, iRmostUnexpand, i, j;
	char buf[BUFLEN];
	while (getline(buf, sizeof buf)) {
		const int origLen = strlen(buf);
		iSpaceStreakStarted = iRmostUnexpand = -1;
		for (i = 0; i < origLen; ++i) {
			const bool atTabstop = i && ! (i % tabstopWidth);
			if (atTabstop && (iSpaceStreakStarted != -1)) {
				const int spaceStreakLen = i - iSpaceStreakStarted;
				if (spaceStreakLen == tabstopWidth) {
					putchar('\t');
					iRmostUnexpand = i;
					iSpaceStreakStarted = -1;
				}
			}
			const char ch = buf[i];
			if (ch == ' ') {
				if (iSpaceStreakStarted == -1) {
					iSpaceStreakStarted = i;
				}
			} else {
				if (i && (iSpaceStreakStarted != -1)) { /* Transition, spaces to non-spaces */
					/* Instead of explicit for-loop, could be cute and
					printf("%*s", i-iSpaceStreakStarted, "");
					*/
					for (j = iSpaceStreakStarted; j < i; ++j)
						putchar(' ');
					iSpaceStreakStarted = -1;
				}
				putchar(ch);
			}
		}
		putchar('\n');
	}
}
