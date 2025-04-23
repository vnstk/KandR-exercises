/* Vainstein K 2025apr20 */
/*Test steps:
	./a.exe < 6-2.c
	./a.exe 4 < 6-2.c
	./a.exe 3 < 6-2.c
*/
/* Reckon to leverage the 6-1.c solution */

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#	include <stdio.h>
#include <stdlib.h> /* for exit(3) */
#include <string.h>

#define MAX_nVarnames 4096
char *varnames[MAX_nVarnames];
int nVarnames = 0;
void addFoundVarname (const char *s)
{
    const int sLen = strlen(s);
    char **pdest = (varnames + nVarnames++);
    *pdest = malloc(sLen+1);
    strcpy(*pdest,s);
}

void pr_varnames (void)
{
	int i;
	for (i = 0; i < nVarnames; ++i) puts(varnames[i]);
}


int isNonNewlineWhtspc(int ch)
{
	return ch == ' ' || ch == '\t';
}

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
	while (fgets(lineInp, sizeof lineInp, stdin)) {
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
#endif
		postProcessLine();
	}
}

int isKeywd_storClassSpecifier (const char *const s)
{
    return
        !strcmp(s,"auto") || !strcmp(s,"register") ||
        !strcmp(s,"static") || !strcmp(s,"extern");
}
int isKeywd_qualDatatype (const char *const s)
{
    return
        !strcmp(s,"void") || !strcmp(s,"char") || !strcmp(s,"int") ||
        !strcmp(s,"float") || !strcmp(s,"double") ||
        !strcmp(s,"short") || !strcmp(s,"long") ||
        !strcmp(s,"signed") || !strcmp(s,"unsigned") ||
        !strcmp(s,"const") || !strcmp(s,"volatile") ||
        !strcmp(s,"bool") || !strcmp(s,"va_list");
        !strcmp(s,"size_t") || !strcmp(s,"ssize_t") || !strcmp(s,"ptrdiff_t");
}
int isKeywd_controlFlow (const char *const s)
{
    return
        !strcmp(s,"if") || !strcmp(s,"else") || !strcmp(s,"do") ||
        !strcmp(s,"default") || !strcmp(s,"while") || !strcmp(s,"goto") ||
        !strcmp(s,"return") || !strcmp(s,"switch") || !strcmp(s,"case") ||
        !strcmp(s,"break") || !strcmp(s,"continue") || !strcmp(s,"for");
}
int isKeywd_other (const char *const s)
{
    return
        !strcmp(s,"struct") || !strcmp(s,"union") || !strcmp(s,"sizeof") ||
        !strcmp(s,"enum") || !strcmp(s,"NULL") || !strcmp(s,"EOF") ||
        !strcmp(s,"extern") || !strcmp(s,"assert") || !strcmp(s,"va_start") ||
		!strcmp(s,"va_arg") || !strcmp(s,"va_end") || !strcmp(s,"typedef");
}


#define MAXWORD 100

void cb__postProcessLine (void) {
	p_lineOutp = lineOutp; /* p_lineOutp is used by getchMine() */
	char word[MAXWORD];
	while (getwordMine(word,MAXWORD) != '\n') {
		if(!*word)
			break;
		/* Now filter out what could not possibly be a variable (ie, userdef symb) name. */
		if (!isalpha(*word)) /* The isalpha() test also gets rid of preprocessor directives. */
			continue;
		if (isKeywd_storClassSpecifier(word) || isKeywd_qualDatatype(word) ||
		    isKeywd_controlFlow(word) || isKeywd_other(word))
			continue;
#ifdef DBG
		printf("\t\e[32m%s\e[0m\n", word);
#endif
		addFoundVarname(word);
	}
}

int cmpVarnames (char **a, char **b) { return strcmp(*a,*b); }

void dedupeVarnames (void)
{
	int i,j;
	for (i=0,j=1;j<nVarnames;) {
		if (! varnames[j])
			break;
		else if (!strcmp(varnames[i],varnames[j])) { /* Same? */
			free(varnames[j]);
			varnames[j] = NULL;
			++j;
		} else {                               /* Differ? */
				++i;
				varnames[i] = varnames[j];
				varnames[j] = NULL;
				++j;
		}
	}
	nVarnames = i + 1;
}

main (const int argc, const char *argv[])
{
	int igroupFrom, igroupUntil, j;
	int commonPrefixLen = 6; /* Can oride from cmdline */
	if (argc==2) {
		if ((commonPrefixLen = atoi(argv[1])) <= 0) {
			fprintf(stderr,"USAGE:  [commonPrefixLen,=6]\n");
			exit(1);
		}
	}
	strip_comments_and_stringLiterals(cb__postProcessLine);
#if 0
    printf("ini nVarnames = %d\n",nVarnames);
	pr_varnames();
#endif
    /*phase-2: sort the gathered varnames */
	qsort(varnames, nVarnames, sizeof(char*),(int(*)(const void*,const void*))cmpVarnames);
    /*phase-3: dedupe */
	dedupeVarnames();
#if 0
    printf("post-dedupe nVarnames = %d\n",nVarnames);
#endif
    /*phase-4: group by prefix of desired len*/
	for (igroupFrom=0,igroupUntil=1; igroupUntil <= nVarnames;) {
		assert(igroupFrom<nVarnames);
		/* If prefix differs (we've now moved on to the next candidate group), or
		   if this is the last possible group, print the previous group --- if it
		   deserves the name.  (One elem is not a group.)
		*/
		if (igroupUntil==nVarnames ||
		    strncmp(varnames[igroupFrom],varnames[igroupUntil], commonPrefixLen))
		{ 
			if (igroupFrom+1 < igroupUntil)
				for (j = igroupFrom; j < igroupUntil; ++j)
					printf("%c%s", (j==igroupFrom)?'\n':' ', varnames[j]);
			igroupFrom = igroupUntil++;
		} else
			++igroupUntil;
	}
}


/* copied from p79 -- BEGIN */
#define BUFSIZE 100
char buf[BUFSIZE];	/* buffer for ungetch */
int bufp = 0;		 /* next free position in buf */
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
