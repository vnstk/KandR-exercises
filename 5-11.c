/* Vainstein K 2025apr12 */
/*
   To validate detab, fine to use this very sourcefile as
   input; to validate entab, try   <(head -40 1-20.c)   as input.

   To validate just the decide_tabstopList() part, try e.g.
   ./a.exe --detab <<<''    and     ./a.exe --detab 11 22 33 <<<''

   To use default tabstop width of say 7 instead of 8, compile
   with    -DDFLT_TABSTOP_WIDTH=7
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

void decide_tabstopList (const int argc, const char *argv[]);
main (const int argc, const char *argv[])
{
	char buf[bufSZ];
	const bool detabMode = argc>=2 && !strcmp(argv[1],"--detab");
	const bool entabMode = argc>=2 && !strcmp(argv[1],"--entab");
	if (!entabMode && !detabMode) {
		fprintf(stderr, "USAGE:  --entab|--detab [tabstop1, tabstop2, ...]\n");
		exit(1);
	}
	decide_tabstopList(argc,argv);
	while (getline(buf))
		if (detabMode)
			detabLine(buf);
		else
			entabLine(buf);
	free(tabstopList);
}

void decide_tabstopList (const int argc, const char *argv[])
{
	int i;
	if (argc == 2) { /* Dflt tabstop settings? */
		tabstopListSz = consoleWIDTH / DFLT_TABSTOP_WIDTH;
		tabstopList = calloc(tabstopListSz, sizeof(int));
		for (i = 0; i < tabstopListSz; ++i)
			tabstopList[i] = (i+1) * DFLT_TABSTOP_WIDTH;
	} else {         /* Custom tabstop settings? */
		tabstopListSz = argc - 2;
		tabstopList = calloc(tabstopListSz, sizeof(int));
		for (i = 2; i < argc; ++i)
			tabstopList[i-2] = atoi(argv[i]);
		for (i = 0; i < tabstopListSz; ++i)
			assert(tabstopList[i] > 0);
	}
	fprintf(stderr, "\e[35;3mDecided tabstopList:  {");
	for (i = 0; i < tabstopListSz; ++i)
		fprintf(stderr, " %d", tabstopList[i]);
	fprintf(stderr, " }\e[0m\n");
}
