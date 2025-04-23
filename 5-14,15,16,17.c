/* Vainstein K 2025apr13 */
/*Test steps:
	bash 5-14,15,16,17_runTests.sh
*/

/* Rules:
	o	The problem statement makes clear that different fields' sort specs
		are to be treated as _independent_ specs.  Hence, result of "-3n -5f"
		will be sorted by 5f, but possibly not by 3n.
	o	Not promising that sorts will be always stable, or always not stable.
	o	sort(1) handles field ranges, e.g. "-k2,5r" means to reverse-sort
		by fields 2 through 5 considered as a single field; that is why
		to reverse-sort by just 2nd field we give "-k2,2r" to sort(1) .  We
		do not handle field ranges, so our equivalent would be just "-2r".
	o	If sorting entire line, all flags must be given in a single
		group.  If sorting by discrete fields, each field's spec
		must be contained within a single group.
	o	Order of orthogonal letters within a group does not matter; field
		numbers, however, are not orthogonal.
	o	Rightmost wins; thus, "-r3r1" is same as "-r1".
	o	If asked to sort by some discrete field, then may not also ask for
		entire-line sorting.
	o	Acceptable field numbers are 1 through 9, only.
	o	Adjacent fields must be separated by [ \t]+
	o	If asked to sort by Nth field, then every input line must
		contain at least N fields.
*/

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h> /*For: atof; and for qsort, whose signature is
	void qsort(void *base, size_t elemsCt, size_t elemSizeof,
	           int (*cmp)(const void *, const void *));       */
#include <string.h>

#define bufSZ 1024 /*max line len*/
#define MAXLINES 5000     /* max #lines to be sorted */
char *lineptr[MAXLINES];  /* pointers to text lines */

int readlines(char *lineptr[], int nlines);
void writelines(char *lineptr[], int nlines);

/* numcmp:  compare s1 and s2 numerically */   /* Copied from p121 */
int numcmp(char *s1, char *s2)
{
	double v1, v2;
	v1 = atof(s1);
	v2 = atof(s2);
	if (v1 < v2)
		return -1;
	else if (v1 > v2)
		return 1;
	else
		return 0;
}

void smashCase (char *s)
{
	char clower; /* My cov enjoys grazing on clower. */
	while (*s) {
		clower = tolower(*s);
		*s++ = clower;
	}
}

void stripAllBut_alpha_num_blanks (char *s)
{
	char c, *p = s;
	while ((c = *p++))
		if (isalpha(c) || isdigit(c) || c == ' ')
			*s++ = c;
	*s = '\0';
}

typedef struct SortSpec {
	struct SortSpec *_next;
	int _field; /* 0 means "the whole line considered as one field". */
	bool _reverse;
	bool _numeric; /* If not numeric, then alpha (ASCIIbetical). */
	bool _dirStyle, _smashCase;
} sortSpec_t;

const char *describeSS (const sortSpec_t *const p)
{
	static char s[64];
	if (p->_field)
		sprintf(s, "%dth", p->_field);
	else
		strcpy(s, "whole");
	if (p->_numeric)
		strcat(s, "+Numer");
	if (p->_reverse)
		strcat(s, "+Rev");
	if (p->_dirStyle)
		strcat(s, "+Dire");
	if (p->_smashCase)
		strcat(s, "+Fold");
	return s;
}

/* If error, will return NULL; otherwise, will return linked list with
   at least one node. */
sortSpec_t *parseFlags (int argc, char *argv[])
{
	sortSpec_t *ssList=NULL, *currSS;
	int wholeLine_groups_seen=0, singleField_groups_seen=0;
	char c;
	int i;
	for (i = 1; i < argc; ++i) {
		const char *s = *++argv;
		if (strlen(s) < 2 || *s != '-') {
			fprintf(stderr, "\e[33;1mIgnoring bad arg \"%s\".\e[0m\n", s);
			continue;
		}
		currSS = calloc(1U, sizeof(sortSpec_t));   assert(currSS);
		if (ssList) { /* Already at least one node in list? */
			sortSpec_t *ssPrev = ssList;
			while (ssPrev->_next)
				ssPrev = ssPrev->_next;
			ssPrev->_next = currSS;
		} else {
			ssList = currSS;
		}
		while ((c = *++s)) {
			if (c >= '1' && c <= '9') {
				currSS->_field = c - '0';
				continue;
			}
			switch (c) {
				case 'd': currSS->_dirStyle=true;  break;
				case 'f': currSS->_smashCase=true; break;
				case 'n': currSS->_numeric=true;   break;
				case 'r': currSS->_reverse=true;   break;
				default: fprintf(stderr, "\e[33;1mIgnoring bad arg '%c'.\e[0m\n", c);
			}
		}
		++*(currSS->_field ? &singleField_groups_seen : &wholeLine_groups_seen);
	}
	if (singleField_groups_seen && wholeLine_groups_seen) {
		fprintf(stderr, "\e[31;1mAsked both per-field and whole-line.\e[0m\n");
		return NULL;
	} else if (wholeLine_groups_seen > 1) {
		fprintf(stderr, "\e[31;1mMOre than 1 whole-line arg group.\e[0m\n");
		return NULL;
	}
	if (! ssList) { /* No flags given? */
		ssList = calloc(1U, sizeof(sortSpec_t));   assert(ssList);
	}
	return ssList;
}

static sortSpec_t *activeSS = NULL;

void populateComparandBuffer (const char *const p /*inputLine, length < bufSZ*/,
                              char *buf /*will be of size bufSZ*/)
{
	const char *s = p;
	if (activeSS->_field) { /* Want a particular field? */
		int fieldsSkipped = 0, fieldLen = 0;
		do {
			/* Skip the previous field. */
			s += fieldLen;
			/* Skip intervening field separator. */
			s += strspn(s, " \t\n");
			/* Scan to past the field. */
			fieldLen = strcspn(s, " \t\n");
			if (! fieldLen)
				break;
			++fieldsSkipped;
		}
		while (fieldsSkipped < activeSS->_field);
		if (fieldsSkipped != activeSS->_field) {
			fprintf(stderr, "\e[31;1mBut %d fields in \"%s\"\e[0m\n", fieldsSkipped, p);
			exit(3);
		}
		strncpy(buf, s, fieldLen);
	} else {
		strcpy(buf, s);
	}
}

int dispatcher(char **a, char **b)
{
	int answ;
	char *aPass, *bPass;
	char aTmp[bufSZ], bTmp[bufSZ];
	populateComparandBuffer(*a, aTmp);
	populateComparandBuffer(*b, bTmp);
	/* If both _dirStyle and _smashCase, then will be (slightly!)
	   faster overall if we take care of _dirStyle first. */
	if (activeSS->_dirStyle) {
		stripAllBut_alpha_num_blanks(aTmp);
		stripAllBut_alpha_num_blanks(bTmp);
	}
	if (activeSS->_smashCase) {
		smashCase(aTmp);
		smashCase(bTmp);
	}
	aPass = aTmp, bPass = bTmp;
	answ = activeSS->_numeric ? numcmp(aPass,bPass) : strcmp(aPass,bPass);
	if (activeSS->_reverse)
		answ *= -1;
	return answ;
}

main(int argc, char *argv[])
{
	sortSpec_t *ssList = parseFlags(argc,argv);
	if (! ssList) {
		printf("Bad args.\n");
		return 2;
	} else {
		/* number of input lines read */
		const int nlines = readlines(lineptr, MAXLINES);
		if (nlines > MAXLINES) {
			printf("input too big to sort\n");
			return 1;
		}
		activeSS = ssList;
		while (activeSS) {
			fprintf(stderr, "\t\e[36;3mSorting by <%s>\e[0m\n", describeSS(activeSS));
			qsort(lineptr, nlines, sizeof(char*),
		      	(int (*)(const void*,const void*))dispatcher);
			activeSS = activeSS->_next;
		}
		writelines(lineptr, nlines);
	}
}


/* Will return # of lines read; 'nlines' param is max lines we've space for. */
int readlines(char *lineptr[], int nLinesMax)
{
	char buf[bufSZ];
	int nLinesRead = 0;
	/* NB: we compare "<= nLinesMax", and not "< nLinesMax", specially
	   so that control could reach the "return nLinesMax+1" statement.
	*/
	while ((nLinesRead <= nLinesMax)  &&
	       memset(buf,'\0',bufSZ)  &&  fgets(buf,bufSZ,stdin))
	{
		if (nLinesRead == nLinesMax)
			return nLinesMax+1; /* Meaning, too many to handle. */
		/* Assert line was short enough to fit into buf. */
		assert(buf[bufSZ-2] == '\n' || buf[bufSZ-2] == '\0');
		const size_t len = strlen(buf);
		char **copyTo = &lineptr[nLinesRead++];
		*copyTo = malloc(len+1);
		strcpy(*copyTo,buf);
	}
	return nLinesRead;
}

void writelines(char *lineptr[], int nlines)
{
	while (nlines--)
		fputs(*lineptr++, stdout);
}
