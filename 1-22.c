/* Vainstein K 2025mar18 */
#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#define BUFLEN 1024

bool isSpcOrTab (const char ch)
{
	return ch == ' ' || ch == '\t';
}

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

int decide_maxOutputWidth (const int argc, char *argv[])
{
	if (argc > 1) {
		const int argVal = atoi(argv[1]);
		if (argVal)
			return argVal;
	}
	fprintf(stderr, "Must supply <maxOutputWidth> cmdline arg.\n");
	exit(2);
}

main (int argc, char *argv[])
{
	const int maxOutputWidth = decide_maxOutputWidth(argc,argv);
	int iRmostUnprinted, iRmostNonspcToSpc, i, j;
	char buf[BUFLEN];
	while (getline(buf, sizeof buf)) {
		iRmostUnprinted = iRmostNonspcToSpc = 0;
		const int origLen = strlen(buf);
		for (i = 0; i < origLen; ++i) {
			if (i && !isspace(buf[i-1]) && isspace(buf[i])) {
				iRmostNonspcToSpc = i;
			}
			if ((i - iRmostUnprinted) == maxOutputWidth) {
				if (iRmostNonspcToSpc) { /* We fold if can... */
					printf("%.*s\n", iRmostNonspcToSpc-iRmostUnprinted, buf+iRmostUnprinted);
					iRmostUnprinted = iRmostNonspcToSpc;
					iRmostNonspcToSpc = 0;
				} else { /* ...and hyphenate if must. */
					printf("%.*s-\n", i-iRmostUnprinted, buf+iRmostUnprinted);
					iRmostUnprinted = i;
				}
				/* Either way, let's now skip past any spaces/tabs here obtaining. */
				while (isSpcOrTab(buf[iRmostUnprinted]))
					++iRmostUnprinted;
				if (iRmostUnprinted > i)
					i = iRmostUnprinted;
			}
			if (! buf[i+1]) { /* Reached EOL? */
				if (iRmostUnprinted < i) {
					printf("%.*s\n", i-iRmostUnprinted+1, buf+iRmostUnprinted);
				}
				break;
			}
		}
	}
}
