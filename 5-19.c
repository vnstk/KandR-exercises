/* Vainstein K 2025apr14 --- my work is the diff between 5-english2cdeclBook.c and this file */
/*Test steps:
	bash 5-19_runTests.sh
*/

/* What K&R call "undcl" */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define MAXTOKEN  100
enum { NAME, PARENS, BRACKETS };
void dcl(void);
void dirdcl(void);
int gettoken(void);
int tokentype;		   /* type of last token */
char token[MAXTOKEN];	/* last token string */
char name[MAXTOKEN];	 /* identifier name */
char datatype[MAXTOKEN]; /* data type = char, int, etc. */
char out[1000];


const char *toktyStr (const int tokty)
{
	switch (tokty) {
		case EOF: return "EOF";
		case NAME: return "NAME";
		case PARENS: return "PARENS";
		case BRACKETS: return "BRACKETS";
		case 10: return "Newl";
		case 32: return "Blank";
		case 40: return "Lparen";
		case 41: return "Rparen";
		case 42: return "Aster";
		case 44: return "Comma";
		case 91: return "Lbrack";
		case 93: return "Rbrack";
		default: return "???";
	}
}

#ifdef DBG
#define PRstate   \
	printf("\t\e[33m(Ln%3d)   tokty<%s> or %d;   tok=\"%s\"\e[0m\n",   \
	       __LINE__,toktyStr(tokentype),tokentype,token)
#define NOWout  \
	printf("\t\e[37;3m(Ln%3d)   Now out= \e[0m<\e[37;7m%s\e[0m>\n",    \
	       __LINE__, out)
#else
#define PRstate
#define NOWout
#endif

int gettoken(void)  /* return next token */
{
	int c, getch(void);
	void ungetch(int);
	char *p = token;
	while ((c = getch()) == ' ' || c == '\t')
		;
	if (c == '(') {
		if ((c = getch()) == ')') {
			strcpy(token, "()");
			return tokentype = PARENS;
		} else {
			ungetch(c);
			return tokentype = '(';
		}
	} else if (c == '[') {
		for (*p++ = c; (*p++ = getch()) != ']'; )
			;
		*p = '\0';
		return tokentype = BRACKETS;
	} else if (isalpha(c)) {
		for (*p++ = c; isalnum(c = getch()); )
			*p++ = c;
		*p = '\0';
		ungetch(c);
		return tokentype = NAME;
	} else
		return tokentype = c;
}

/* undcl:  convert word descriptions to declarations */
main()
{
	int type;
	char temp[4096];
	while (gettoken() != EOF) {
		PRstate;
		strcpy(out, token);
		NOWout;
		while ((type = gettoken()) != '\n') {
			PRstate;
			if (type == PARENS || type == BRACKETS)
				strcat(out, token);
			else if (type == '*') {
#if 0
				if (!strcmp(out,token)) {
					sprintf(temp, "(*%s)", out);
				} else {
					sprintf(temp, "*%s", out);
				}
#else
					sprintf(temp, "(*%s)", out);
#endif
				strcpy(out, temp);
			} else if (type == NAME) {
				sprintf(temp, "%s %s", token, out);
				strcpy(out, temp);
			} else
				printf("invalid input at %s\n", token);
			NOWout;
		}
		printf("%s\n", out);
	}
	return 0;
}


#define BUFSIZE 100
char buf[BUFSIZE];	/* buffer for ungetch */
int bufp = 0;		 /* next free position in buf */
int getch(void)  /* get a (possibly pushed-back) character */
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
