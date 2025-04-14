/* Vainstein K 2025apr13 */
/*
   Same as 5-11.c except for cmdline usage and the
   decide_tabstopList() part.

   Expect same result from   ./a.exe --detab -3 +5 <<<''
   and from                  ./a.exe --detab +5 -3 <<<''
*/
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef DFLT_TABSTOP_WIDTH
#	define DFLT_TABSTOP_WIDTH 8
#endif

#define consoleWIDTH 40
#define bufSZ (consoleWIDTH+1)

bool getline (char *const buf)
{
	return fgets(buf, bufSZ, stdin);
}

int tabstopListSz=-1, *tabstopList=NULL;
int nearestTabstop (int leftBound) /* return 0 if none at leftBound or beyond. */
{
	int i;
	for (i = 0; i < tabstopListSz; ++i)
		if (tabstopList[i] >= leftBound)
			return tabstopList[i];
	return 0;
}
int nextTabstop (int nextAfter) /* return consoleWIDTH if none beyond nextAfter. */
{	        /* NB: nextAfter is expected to be one of the values in tabstopList. */
	int i;
	for (i = 0; i < tabstopListSz; ++i)
		if (tabstopList[i] > nextAfter)
			return tabstopList[i];
	return consoleWIDTH;
}

void detabLine (const char *const buf) /* Adapted from Ex 1-20 */
{	                                   /* The #if...endif is adaptation delta. */
	const int origLen = strlen(buf);
	int iInput, iOutput;
	for (iInput = 0, iOutput = 0; iInput < origLen; ++iInput) {
		const char ch = buf[iInput];
		if (ch == '\t') {
			/* Advance to nearest tabstop, but no less than 1 space's worth. */
			int k = iOutput + 1;
#if 0
			if (k % tabstopWidth)
				k += tabstopWidth - (k % tabstopWidth);
#else
			int kNearest = nearestTabstop(k);
			if (kNearest)
				k = kNearest;
#endif
			printf("%*s", k-iOutput, "");
			iOutput = k;
		} else {
			putchar(ch);
			++iOutput;
		}
	}
	putchar('\n');
}

void entabLine (const char *const buf) /* Adapted from Ex 1-21 */
{	                                   /* The #if...endif is adaptation delta. */
	const int origLen = strlen(buf);
	int iRmostUnexpand=0, i;
	int iSpaceStreakStarted = iRmostUnexpand = -1;
	for (i = 0; i < origLen; ++i) {
#if 0
		const bool atTabstop = i && ! (i % tabstopWidth);
#else
		const bool atTabstop = i == nearestTabstop(i);
		const int tabstopWidth = nextTabstop(nearestTabstop(i)) - nearestTabstop(i);
#endif
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
				printf("%*s", i-iSpaceStreakStarted, "");
				iSpaceStreakStarted = -1;
			}
			putchar(ch);
		}
	}
	putchar('\n');
}

bool decide_tabstopList (const int argc, const char *argv[]);
main (const int argc, const char *argv[])
{
	char buf[bufSZ];
	const bool detabMode = argc==4 && !strcmp(argv[1],"--detab");
	const bool entabMode = argc==4 && !strcmp(argv[1],"--entab");
	if ((!entabMode && !detabMode) || ! decide_tabstopList(argc,argv)) {
		fprintf(stderr, "USAGE:  --entab|--detab -M +N\n"
		        "\t\t(Meaning a tabstop every N columns, starting at Mth.)\n");
		exit(1);
	}
	while (getline(buf))
		if (detabMode)
			detabLine(buf);
		else
			entabLine(buf);
	free(tabstopList);
}

bool decide_tabstopList (const int argc, const char *argv[])
{
	int fromM=-1, everyN=-1;
	int i, numval;
	for (i=2; i<4; ++i) {
		const char *s = argv[i];
		if ((strlen(s) < 2) || (s[0]!='-' && s[0]!='+'))
			return false;
		numval = atoi(s+1);
		if (numval <= 0)
			return false;
		if (s[0] == '-')
			fromM = numval;
		if (s[0] == '+')
			everyN = numval;
	}
	if (fromM < 1 || everyN < 1)
		return false;
	tabstopListSz = (consoleWIDTH - fromM) / everyN + 1;
	tabstopList = calloc(tabstopListSz, sizeof(int));
	for (i = 0; i < tabstopListSz; ++i)
		tabstopList[i] = fromM + i * everyN;
	fprintf(stderr, "\e[35;3mDecided tabstopList:  {");
	for (i = 0; i < tabstopListSz; ++i)
		fprintf(stderr, " %d", tabstopList[i]);
	fprintf(stderr, " }\e[0m\n");
}
