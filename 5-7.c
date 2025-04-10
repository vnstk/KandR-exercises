/* Vainstein K 2025apr09 --- my work is the diff between 5-linessorterBook.c and this file */

#include <stdio.h>
#include <string.h>
#define MAXLINES 5000	 /* max #lines to be sorted */
#define MAXLEN 1000  /* max length of any input line */
char *lineptr[MAXLINES];  /* pointers to text lines */
int readlines(char *lineptr[], int nlines, char (*)[MAXLEN]);
void writelines(char *lineptr[], int nlines);
void qsort(char *lineptr[], int left, int right);

static char linesStore[MAXLINES][MAXLEN];
/* sort input lines */
main()
{
	int nlines;	 /* number of input lines read */
	if ((nlines = readlines(lineptr, MAXLINES, linesStore)) >= 0) {
		qsort(lineptr, 0, nlines-1);
		writelines(lineptr, nlines);
		return 0;
	} else {
		printf("error: input too big to sort\n");
		return 1;
	}
}
int getline(char *, int);

char *alloc(int);
/* readlines:  read input lines */
int readlines(char *lineptr[], int maxlines, char linesStore[][MAXLEN])
{
	int len, nlines;
	char *p, line[MAXLEN];
	nlines = 0;
	while ((len = getline(line, MAXLEN)) > 0)
		if (nlines >= maxlines)
			return -1;
		else {
			p = linesStore[nlines];
			line[len-1] = '\0';  /* delete newline */
			strcpy(p, line);
			lineptr[nlines++] = p;
		}
	return nlines;
}

/* writelines:  write output lines */
void writelines(char *lineptr[], int nlines)
{
	while (nlines-- > 0)
		printf("%s\n", *lineptr++);
}

/* qsort:  sort v[left]...v[right] into increasing order */
void qsort(char *v[], int left, int right)
{
	int i, last;
	void swap(char *v[], int i, int j);
	if (left >= right)  /* do nothing if array contains */
		return;		 /* fewer than two elements */
	swap(v, left, (left + right)/2);
	last = left;
	for (i = left+1; i <= right; i++)
		if (strcmp(v[i], v[left]) < 0)
			swap(v, ++last, i);
	swap(v, left, last);
	qsort(v, left, last-1);
	qsort(v, last+1, right);
}

/* swap:  interchange v[i] and v[j] */
void swap(char *v[], int i, int j)
{
	char *temp;
	temp = v[i];
	v[i] = v[j];
	v[j] = temp;
}

/* getline:  read a line into s, return length  */
int getline(char s[],int lim)
{
	int c, i;
	c = EOF; /* to quieten -Wmaybe-uninitialized */
	for (i=0; i < lim-1 && (c=getchar())!=EOF && c!='\n'; ++i)
		s[i] = c;
	if (c == '\n') {
		s[i] = c;
		++i;
	}
	s[i] = '\0';
	return i;
}


/* Re "how much faster" question: the deeply scientific procedure of

bef=$SECONDS; for i in {1..500}; do ./a.exe < /c/v-dev/shar/2827-repro-dataset.txt >/dev/null ; done; echo $[SECONDS-bef]

, where 2927-repro-dataset.txt is 3890 lines of length 392 or less each,

	o		original ver took 58sec
	o		improved ver took 54sec

; so answer is "1.074 times faster!".  A good thing I'd cheated slightly, by making linesStore file-static and hence allocated at linktime; for otherwise the performance gain might have been not quite this impressive. */
