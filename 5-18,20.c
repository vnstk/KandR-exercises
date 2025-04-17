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
void indent (int nestDepth)
{
	if (nestDepth) {
		int i;
		printf("\n");
		for (i = 1; i <= nestDepth; ++i)
			printf("\t");
	}
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

void subdecl(int nestDepth, bufConj_t*);
void dcl(int nestDepth, bufConj_t*);
void dirdcl(int nestDepth, bufConj_t*);
int gettoken(void);

int tokentype;		   /* type of last token */
char token[MAXTOKEN];	/* last token string */
char storageClassSpecifiers[MAXTOKEN*2]; /* auto|register|static|extern */
char name[MAXTOKEN];	 /* identifier name */
char qualDatatype[MAXTOKEN]; /* data type = char, int, etc. */
char out[1000];

#define MARKentry \
	printf("\t\e[32mEntered %7s, nestDepth=%d ttyp=%s token=\"%s\"\e[0m\n",   \
	       __func__, nestDepth, toktyStr(tokentype), token)

#define MARKxitg \
	printf("\t\e[36mExiting %7s, nestDepth=%d ttyp=%s token=\"%s\"\n\t\t_name[%s] _quaDa[%s] _out[%s]\e[0m\n",   \
	       __func__, nestDepth, toktyStr(tokentype), token, bufCj->_name, bufCj->_qualDatatype, bufCj->_out)

#define PRstate \
	printf("\t\e[33m" FLfmt " tokty<%s> tok=\"%s\" ungotten='%c'\e[0m\n",    \
	       FLemit,toktyStr(tokentype),token,(bufp>0)?buf[0]:'?')

main()  /* convert declaration to words */
{
	bufConj_t *bufCj = &toplev;
	while (gettoken() != EOF) {   /* 1st token on line */
		if (possPartOf_storClassSpecifier(token)) {
			strcat(storageClassSpecifiers, " ");
			strcat(storageClassSpecifiers, token);
			continue;
		}

		subdecl(/*nestDepth*/ 0, bufCj);
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


/* Either
	o	Entire decl, except for storage-class specifiers.
		o	Terminated by LF.
		o	Name mandatory.
	o	A func param.
		o	Terminated by comma or Rparen.
		o	Name optional.
*/
void subdecl(int nestDepth, bufConj_t *bufCj)
{
	MARKentry;
	PRstate;
	while (possPartOf_qualDatatype(token)) {
		strcat(bufCj->_qualDatatype, " ");
		strcat(bufCj->_qualDatatype, token);
		*token = '\0';
		tokentype = gettoken();
		continue;
	}
	ungetch(tokentype);
	if (! *bufCj->_qualDatatype)
		strcpy(bufCj->_qualDatatype, "int "); /* If datatype missing, assume int. */
	bufCj->_out[0] = '\0';
	dcl(nestDepth, bufCj);	   /* parse rest of line */
PRstate;
	if (nestDepth > 0) {
		if (tokentype == NAME)
			strcpy(bufCj->_name, token);
		else
	if (tokentype != '\n')
		printf(FLfmt "\e[31;1msyntax error\e[0m\n", FLemit);
	}
	printf("%s: %s %s %s\n", bufCj->_name, storageClassSpecifiers, bufCj->_out, bufCj->_qualDatatype);
	MARKxitg;
}

typedef enum { plainPTR = 1, constPTR } pointyEnd_t;
/* dcl:  parse a declarator */
void dcl(int nestDepth, bufConj_t *bufCj)
{
	int ns, i=0;
	MARKentry;
	pointyEnd_t stack[20];
	memset(stack,'\0',sizeof stack);
	PRstate;
	for (ns = 0;;) {
		if (gettoken() != '*') /* Not pointy? */
			break;
		tokentype = gettoken();
		if (NAME == tokentype && !strcmp(token,"const")) {
			stack[ns++] = constPTR;
		} else {
			stack[ns++] = plainPTR;
			if ('*' != tokentype)
				break;
			ungetch(tokentype);
		}
	}
	dirdcl(nestDepth, bufCj);
	while (i < ns) {
		if (stack[i++] == constPTR)
			strcat(bufCj->_out, " const-pointer-to");
		else
			strcat(bufCj->_out, " pointer-to");
	}
	MARKxitg;
}


void fuParms(int nestDepth, bufConj_t *bufCj)
{
	MARKentry;
	PRstate;
	for (;;) { /* If is a 2nd func params, do this dance again. */
		bufConj_t sublevel;
		memset(&sublevel,'\0',sizeof sublevel);
		subdecl(nestDepth+1, &sublevel);
		indent(nestDepth+1);
		printf("%s: %s %s\n",
				     sublevel._name, sublevel._out, sublevel._qualDatatype);
		if (tokentype != ',')
			break;
		indent(nestDepth+1);
		printf(",\n");
	}
	MARKxitg;
}


/* dirdcl:  parse a direct declarator */
void dirdcl(int nestDepth, bufConj_t *bufCj)
{
	int type;
	MARKentry;
	PRstate;
	if (tokentype == '(') {		 /* ( dcl ) */
		PRstate;
		const int peektokty = gettoken();
		ungetch(peektokty);
printf("\t\e[36m""peektokty=%s\n",toktyStr(peektokty));
		if (peektokty == '*' || peektokty == ')') {
			dcl(nestDepth,bufCj); /* As of old. */
			if (tokentype != ')')
				printf(FLfmt "\e[31;1merror: missing )\n\e[0m", FLemit);
		} else {
			fuParms(nestDepth,bufCj);
		}
	} else if (tokentype == NAME)  /* variable name */
		strcpy(bufCj->_name, token);
	else {
		/* below toplev, names are optional; at toplev, we error-recover by... */
		if (nestDepth == 0)
			strcpy(bufCj->_name, "freddie"); /* ...inventing a name. */
	}
	while ((type=gettoken()) == PARENS || type == BRACKETS)
		if (type == PARENS)
			strcat(bufCj->_out, " function returning");
		else {
			strcat(bufCj->_out, " array");
			strcat(bufCj->_out, token);
			strcat(bufCj->_out, " of");
		}
	MARKxitg;
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
