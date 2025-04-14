/* Vainstein K 2025apr13 */
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h> /* for exit(3) */
#include <string.h>

#ifndef bufSZ
#	define bufSZ 1024
#endif

#define DFLT_max_nLinesKept 10
int nLinesKept = 0, max_nLinesKept = DFLT_max_nLinesKept, i_oldestLineKept = 0;

#define allocCHK(ptrVar) do {   \
	if (! ptrVar)   \
		fprintf(stderr, "\e[31m(Ln%d) Alloc/realloc into"   \
                " '%s' failed.\e[0m\n", __LINE__, #ptrVar);   \
	} while(0)

typedef struct {
	char *_storage;
	int   _storageSz;
	int   _occupantLen;
} keptSlot_t;
keptSlot_t *linesKept; /* Will serve as a circular array. */

bool adjust_max_nLinesKept (const int argc, const char *argv[]);


main (const int argc, const char *argv[])
{
	char buf[bufSZ];
	bool startingNewLine;
	int i, i_currSlot;

	if (argc > 2 || ! adjust_max_nLinesKept(argc,argv)) {
		fprintf(stderr, "\e[35mUSAGE: [-n]\n\t\t(n defaults to 10.)\e[0m\n");
		exit(1);
	}
	linesKept = (keptSlot_t*) calloc(max_nLinesKept, sizeof(keptSlot_t));
	allocCHK(linesKept);

	/* Phase-1: scan through input lines */
	startingNewLine=true;
	i_currSlot = -1;
	while (memset(buf,'\0',bufSZ)  &&  fgets(buf,bufSZ,stdin)) {
		const size_t readLen = strlen(buf);
		/* fgets always stores '\0' into last char, so the intesting
		   question is what's been stored into the penultimate char. */
		const bool didReadEOL = buf[bufSZ-2] == '\n' || buf[bufSZ-2] == '\0';

		if (startingNewLine) {
			i_currSlot = (i_currSlot + 1) % max_nLinesKept;
			if (nLinesKept && (i_currSlot == i_oldestLineKept))
				i_oldestLineKept = (i_oldestLineKept + 1) % max_nLinesKept;
			if (nLinesKept < max_nLinesKept)
				nLinesKept++;
		}

		keptSlot_t *const currSlot = linesKept + i_currSlot;

		if (startingNewLine) {
			currSlot->_occupantLen = 0; /* Clear out previous occupant. */
		}

		/* Alloc more storage in curr slot, if need to. */
		if (currSlot->_storageSz < (currSlot->_occupantLen + readLen)) {
			if (! currSlot->_storage) {
				currSlot->_storageSz = bufSZ;
				currSlot->_storage = malloc(currSlot->_storageSz);
			} else {
				currSlot->_storageSz *= 2; /* incr 2x each time, std approach */
				currSlot->_storage = realloc(currSlot->_storage, currSlot->_storageSz);
			}
			allocCHK(currSlot->_storage);
		}

		/* Append from buf, and update _occupantLen; prepare for next fgets. */
		assert(currSlot->_storageSz > (currSlot->_occupantLen + readLen));
		strcpy(currSlot->_storage + currSlot->_occupantLen, buf);
		currSlot->_occupantLen += readLen;
		startingNewLine = didReadEOL;
	}

	/* Phase-2: output the last N (or fewer) lines */
	for (i = i_oldestLineKept; nLinesKept--; ++i) {
		const keptSlot_t *currSlot = linesKept + (i % max_nLinesKept);
		/* A few sanity checks, first... */
		assert(currSlot->_storage);
		assert(currSlot->_storageSz);
		assert(currSlot->_occupantLen);
		assert(currSlot->_occupantLen == strlen(currSlot->_storage));
		/*... and print it. */
		fputs(currSlot->_storage, stdout);
		free(currSlot->_storage);
	}
	free(linesKept);
}


bool adjust_max_nLinesKept (const int argc, const char *argv[])
{
	const char *s = argv[1];
	if (! s)
		return true; /* No problem, stay with default */
	if (*s != '-')
		return false;
	max_nLinesKept = atoi(s+1);
	if (max_nLinesKept > 0)
		return true;
	fprintf(stderr, "\e[31mIgnoring illegal arg \"%s\"\e[0m\n", s);
	max_nLinesKept = DFLT_max_nLinesKept;
	return true; /* No problem, revert to default */
}
