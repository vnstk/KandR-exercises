/* What K&R call "dcl" */

/* func+line info; invaluable to provide context. */
#define FLemit __func__,__LINE__
#define FLtake const char *fu, const int li
#define FLfmt "/%s:%d/   "
#define FLpass fu, li

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define MAXTOKEN  100
enum { NAME, PARENS, BRACKETS };

#define BUFSIZE 100
char buf[BUFSIZE];	/* buffer for ungetch */
int bufp = 0;		 /* next free position in buf */
int getch(void);
void ungetch(int);

const char *toktyStr (const int tokty)
{
	switch (tokty) {
		case EOF: return "EOF";
		case NAME: return "NAME";
		case PARENS: return "PARENS";
		case BRACKETS: return "BRACKETS";
		case 10: return "Newl";
		case 40: return "Lparen";
		case 41: return "Rparen";
		case 42: return "Aster";
		case 44: return "Comma";
		case 91: return "Lbrack";
		case 93: return "Rbrack";
		default: return "???";
	}
}
int reachedEnd (const int tokty)
{
	return tokty == EOF || tokty == '\n';
}
void skipWhitespace (int *const pch)
{
	while ((*pch = getch()) == ' ' || *pch == '\t')
		;
}

int possPartOf_storClassSpecifier (const char *const s)
{
	return
		!strcmp(s,"auto") || !strcmp(s,"register") ||
		!strcmp(s,"static") || !strcmp(s,"extern");
}
int possPartOf_qualDatatype (const char *const s)
{
	return
		!strcmp(s,"void") || !strcmp(s,"char") || !strcmp(s,"int") ||
		!strcmp(s,"float") || !strcmp(s,"double") ||
		!strcmp(s,"short") || !strcmp(s,"long") ||
		!strcmp(s,"signed") || !strcmp(s,"unsigned") ||
		!strcmp(s,"const") || !strcmp(s,"volatile");
}

typedef struct {
	char _name[MAXTOKEN];	 /* identifier name */
	char _qualDatatype[MAXTOKEN*4]; /* char|int|...; also const|volatile */
	char _out[1000];
} bufConj_t;
bufConj_t toplev;

void dcl(void);
void dirdcl(void);
int gettoken(void);

int tokentype;		   /* type of last token */
char token[MAXTOKEN];	/* last token string */
char storageClassSpecifiers[MAXTOKEN*2]; /* auto|register|static|extern */
char name[MAXTOKEN];	 /* identifier name */
char datatype[MAXTOKEN]; /* data type = char, int, etc. */
char out[1000];


#define PRstate printf("\t\e[33m" FLfmt " tokty<%s> tok=\"%s\" ungotten='%c'\e[0m\n", FLemit,toktyStr(tokentype),token,(bufp>0)?buf[0]:'?')

main()  /* convert declaration to words */
{
	while (gettoken() != EOF) {   /* 1st token on line */
		if (possPartOf_storClassSpecifier(token)) {
			strcat(storageClassSpecifiers, " ");
			strcat(storageClassSpecifiers, token);
			continue;
		}
#if 1
strcpy(datatype, token);  /* is the datatype */
#else
		while (possPartOf_qualDatatype(token)) {
			strcat(datatype, " ");
			strcat(datatype, token);
			tokentype = gettoken();
			continue;
		}
		ungetch(tokentype);
#endif
		PRstate;
		out[0] = '\0';
		dcl();	   /* parse rest of line */
		if (tokentype != '\n')
			printf(FLfmt "\e[31;1syntax error\e[0m\n", FLemit);
		printf("%s: %s %s %s\n", name, storageClassSpecifiers, out, datatype);
	}
	return 0;
}

int gettoken(void)  /* return next token */
{
	int c;
	char *p = token;
	skipWhitespace(&c);
	if (c == '(') {
		skipWhitespace(&c);  /** So doesn't choke on extra whtspc **/
		ungetch(c);          /** So doesn't choke on extra whtspc **/
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

/* dcl:  parse a declarator */
void dcl(void)
{
	PRstate;
	int ns;
	for (ns = 0; gettoken() == '*'; ) /* count *'s */
		ns++;
	dirdcl();
	while (ns-- > 0)
		strcat(out, " pointer to");
}
/* dirdcl:  parse a direct declarator */
void dirdcl(void)
{
	PRstate;
	int type;
	if (tokentype == '(') {		 /* ( dcl ) */
		dcl();
		if (tokentype != ')')
			printf(FLfmt "\e[31;1merror: missing )\n\e[0m", FLemit);
	} else if (tokentype == NAME)  /* variable name */
		strcpy(name, token);
	else
		printf(FLfmt "\e[31;1merror: expected name or (dcl)\n\e[0m", FLemit);
	while ((type=gettoken()) == PARENS || type == BRACKETS)
		if (type == PARENS)
			strcat(out, " function returning");
		else {
			strcat(out, " array");
			strcat(out, token);
			strcat(out, " of");
		}
}

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
