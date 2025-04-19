/* Vainstein K 2025apr19 */
/*
To list (deduped) the words we thought input was composed of,

		./a.exe Book < 6-1.c | sort -u | tr -dc '\012[ -~]' | column

To see what difference my deltas make,

		diff -y   <(./a.exe Book < 6-1.c | sort -u)   <(./a.exe Mine < 6-1.c | sort -u)

To print counts also,  "sort | uniq -c"  instead of  "sort -u"  above.

NB:
	In the "fixed output", things you _should_ see:
	In the "fixed output", things you _should not_ see:
*/

/* Shall leverage solution to 1-23.c (strip comments).
*/

#include <ctype.h>
#include <stdbool.h>
#	include <stdio.h>
#include <stdlib.h> /* for exit(3) */
#include <string.h>

/* Copied from 1-23.c solution; does not remove LF from read-in line. */
bool getline (char *const buf, const int buflen)
{
	memset(buf, '\0', buflen);
	int c=EOF, i;
	for (i=0; (c=getchar())!=EOF; ++i) {
		if (i >= buflen-2) {
			fprintf(stderr, "Buffer too small.\n");
			return false;
		}
		buf[i] = c;
		if (c == '\n')
			break;
	}
	return c != EOF;
}



/* getword:  get next word or character from input */
int getwordBook(char *word, int lim) /* Copied from p136 */
{
	int c, getchBook(void);
	void ungetch(int);
	char *w = word;
	while (isspace(c = getchBook()))
		;
	if (c != EOF)
		*w++ = c;
	if (!isalpha(c)) {
		*w = '\0';
		return c;
	}
	for ( ; --lim > 0; w++)
		if (!isalnum(*w = getchBook())) {
			ungetch(*w);
			break;
		}
	*w = '\0';
	return word[0];
}
/* where a word is one of:
	-	string of letters and digits, beginning with a letter
	-	a single non-whtspc char
*/

int isNonNewlineWhtspc(int ch)
{
	return ch == ' ' || ch == '\t';
}

/* where a word is one of:
	-	string of letters digits undersc, beginning with a letter or undersc
	-	string of letters preceded by '#' (strip any whitespc betw '# and the letters)
	-	a single non-whtspc char
 , unless encountered inside a string literal or a comment --- those, ignore.
*/
int getwordMine(char *word, int lim)
{
	int c, column, getchMine(int*);
	void ungetch(int);
	char *w = word;
	while (isNonNewlineWhtspc(c = getchMine(&column)))
		;
	if (c != '\n') {
		*w++ = c;
	}
	/* Look for start of a word */
	if (!isalpha(c) && c!='_' && !(column==0 && c=='#')) {
		*w = '\0';
		return c;
	}
	if (column==0 && c=='#') {
		while (isNonNewlineWhtspc(c = getchMine(&column)))
			;
		ungetch(c);
	}
	for ( ; --lim > 0; w++) {
		if (!isalnum(*w = getchMine(&column)) && (*w != '_')) {
			ungetch(*w);
			break;
		}
	}
	*w = '\0';
	return word[0];
}


char                    *p_lineOutp;
/* Adapted from the main() of 1-23.c solution */
#define BUFLEN 1024
char                       lineOutp[BUFLEN];
void strip_comments_and_stringLiterals (void (*postProcessLine)(void))
{
	bool insideComment=false, insideStrLiteral=false, insideCharLiteral=false;
	char lineInp[BUFLEN]                   ; /* We must buffer output, instead
		of just printing it to stdout right away, because of the case of starting
		a comment: when we see *, we've already printed the / directly preceding! */
	while (getline(lineInp, sizeof lineInp)) {
		const int origLen = strlen(lineInp);
		char chPrev='\0'         , chCurr;
		bool chPrev_escaped=false, chCurr_escaped=false;
		int iInp=0, iOutp=0;
		memset(lineOutp, '\0', sizeof lineOutp);
		for (; iInp < origLen; ++iInp) {
			chCurr = lineInp[iInp];
			chCurr_escaped = (chPrev == '\\' && ! chPrev_escaped);
			if (insideComment) {
 				/* check for end of comment */
				if (chCurr == '/' && chPrev == '*') {
					insideComment = false;
				}
			} else {
				if (insideStrLiteral) {
 					/* check for end of str lit */
					if (chCurr == '"' && (chPrev != '\\' || (chPrev == '\\' && chPrev_escaped))) {
						insideStrLiteral = false;
					}
				} else {
					/* check for beginning and end of char lit */
					if (chCurr == '\'' && (chPrev != '\\' || (chPrev == '\\' && chPrev_escaped))) {
						insideCharLiteral = ! insideCharLiteral;
					}
					/* check for beginning of str lit */
					if (chCurr == '"' && ! insideCharLiteral && (chPrev != '\\' || ! chPrev_escaped)) {
						insideStrLiteral = true;
					}
					/* check for beginning of comment */
					if (chCurr == '*' && chPrev == '/') {
						insideComment = true;
						lineOutp[iOutp-1] = '\0';
						--iOutp;
					}
				}
				if (! insideComment && ! insideStrLiteral) {
					lineOutp[iOutp++] = chCurr;
				}
			}
			chPrev         = chCurr;
			chPrev_escaped = chCurr_escaped;
		}
#ifdef DBG
		printf("\n\t\t\e[32;7m len=%-2zu\e[27m  %s\e[0m", strlen(lineOutp),lineOutp);
		/*fputs(lineOutp, stdout);*/
#endif
		postProcessLine();
	}
}



#define MAXWORD 100

void cb__postProcessLine (void) {
	p_lineOutp = lineOutp; /* p_lineOutp is used by getchMine() */
	char word[MAXWORD];
	while (getwordMine(word,MAXWORD) != '\n') {
if(!*word)break;
		puts(word);
	}
}

main (const int argc, const char *argv[])
{
	char word[MAXWORD];
	if (argc==2 && !strcmp(argv[1],"Book")) {
		while (getwordBook(word,MAXWORD) != EOF) {
			puts(word);
		}
	} else if (argc==2 && !strcmp(argv[1],"Mine")) {
		strip_comments_and_stringLiterals(cb__postProcessLine);
	} else {
		fprintf(stderr,"USAGE:  Book|Mine\n");
		exit(1);
	}
}


/* copied from p79 -- BEGIN */
#define BUFSIZE 100
char buf[BUFSIZE];	/* buffer for ungetch */
int bufp = 0;		 /* next free position in buf */
int getchBook(void)  /* get a (possibly pushed-back) character */
{
	return (bufp > 0) ? buf[--bufp] : getchar();
}
void ungetch(int c)   /* push character back on input */
{
	if (bufp >= BUFSIZE)
		printf("ungetch: too many characters\n");
	else
		buf[bufp++] = c;
}
/* copied from p79 -- END */


int getchMine(int *column)  /* get a (possibly pushed-back) character */
{
	*column = p_lineOutp - lineOutp;
	return (bufp > 0) ? buf[--bufp] : *p_lineOutp++;
}
