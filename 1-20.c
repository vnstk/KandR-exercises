/* Vainstein K 2025mar17 */
/* NB: given input.txt,
                                       gcc 1-20.c && ./a.out 8 <input.txt
and
                                       expand -t 8 <input.txt
should give same output.  Also handy.
                                       cat -v -t -e
*/
/* Validation aids:
--------------
x-spc-tab-y
x 	y
--------------
tab-spc-spc-a-tab-b
	  a	b
--------------
spc-tab-p-spc-spc-tab-tab-q-spc-r-tab-s-tab-tab-tab
 	p  		q r	s			
*/
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#define BUFLEN 1024

bool getline (char *const buf, const int buflen)
{
	memset(buf, '\0', buflen); /* Need ??  */
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
	int iInput, iOutput, j;
	char buf[BUFLEN];
	while (getline(buf, sizeof buf)) {
		const int origLen = strlen(buf);
		for (iInput = 0, iOutput = 0; iInput < origLen; ++iInput) {
			const char ch = buf[iInput];
			if (ch == '\t') {
				/* Advance to nearest tabstop, but no less than 1 space's worth. */
				int k = iOutput + 1;
				if (k % tabstopWidth)
					k += tabstopWidth - (k % tabstopWidth);
				for (j = 0; j < (k - iOutput); ++j)
					putchar(' ');
				iOutput = k;
			} else {
				putchar(ch);
				++iOutput;
			}
		}
		putchar('\n');
	}
}
