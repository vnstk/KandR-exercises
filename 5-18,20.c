/* Vainstein K 2025apr14 --- my work is the diff between 5-cdecl2englishBook.c and this file */
/*Test steps:
	bash 5-18,20_runTests.sh
*/

/* func+line info; invaluable to provide context. */
#define FLemit __func__,__LINE__
#define FLtake const char *fu, const int li
#define FLfmt "/%s:%d/   "
#define FLpass fu, li

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define MAXTOKEN  100
/* Made NAME be 1, to avoid confusion with NUL. */
enum { NAME=1, PARENS, BRACKETS };

#define BUFSIZE 100
char buf[BUFSIZE];	/* buffer for ungetch */
int bufp = 0;		 /* next free position in buf */
int getch(void);
void ungetch(FLtake, int);

char *vstrcat (char *dest, ...) /* Purely for more concise code. */
{
	va_list ap;
	va_start(ap, dest);
	const char *src;
	while ((src = va_arg(ap, const char *))) {
		(void) strcat(dest, src);
	}
	va_end(ap);
	return dest;
}

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

int reachedEnd_wholeDecl (const int tokty)
{
	return tokty == EOF || tokty == '\n' || tokty == '\0';
}
int reachedEnd_fuParam (const int tokty)
{
	return tokty == ',' || tokty == ')';
}

void skipWhitespace (int *const pch)
{
	while ((*pch = getch()) == ' ' || *pch == '\t')
		;
}

void indent (int nestDepth, char *dest)
{
		int i;
		for (i = 0; i <= nestDepth; ++i)
			strcat(dest, "\t");
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
		!strcmp(s,"const") || !strcmp(s,"volatile") ||
		!strcmp(s,"bool") || !strcmp(s,"va_list");
}

typedef struct {
	char _name[MAXTOKEN];	 /* identifier name */
	char _pointerObj_cvQual[32]; /* If see "const char *const p", the
		_2nd_ const therein is a pointer-obj CV-qual. */
	char _qualDatatype[MAXTOKEN*4]; /* char|int|...; also const|volatile */
	char _out[1000];
	bool _did_printDeclDescription;
} bufConj_t;
bufConj_t toplev;

void printDeclDescription (bufConj_t *bufCj);
void subdecl(int nestDepth, bufConj_t*, bool reuseDatatypeMode);
void fuParms(int nestDepth, bufConj_t*);
void dcl(int nestDepth, bufConj_t*);
void dirdcl(int nestDepth, bufConj_t*);

int gettoken(FLtake);

int tokentype;		   /* type of last token */
char token[MAXTOKEN];	/* last token string */
char storageClassSpecifiers[MAXTOKEN*2]; /* auto|register|static|extern */

#if 0
#define PRstate \
	printf("\t\e[33m" FLfmt " tokty<%s> tok=\"%s\" ungotten='%c'\e[0m  _name[%s]  _dtyp[%s]  nestDepth=%d\n",    \
	       FLemit,toktyStr(tokentype),token,(bufp>0)?buf[0]:'?', bufCj->_name,bufCj->_qualDatatype, nestDepth)
#endif


main()  /* convert declaration to words */
{
	bufConj_t *bufCj = &toplev;
#if 0
	while (gettoken(FLemit) != EOF) {   /* 1st token on line */
#endif
	while (! reachedEnd_wholeDecl(gettoken(FLemit))) {
		const bool reuseDatatypeMode = tokentype == ',';
		if (reuseDatatypeMode) {
			if (! bufCj->_did_printDeclDescription)
				printDeclDescription(bufCj);
		} else {
			if (possPartOf_storClassSpecifier(token)) {
				vstrcat(storageClassSpecifiers, " ", token, NULL);
				continue;
			}
		}

		subdecl(/*nestDepth*/ 0, bufCj, reuseDatatypeMode);
	}
	if (! bufCj->_did_printDeclDescription)
		printDeclDescription(bufCj);
	return 0;
}

#ifdef DBG
#define SETdatatyp   \
		printf("\t\e[31;3m" FLfmt "\e[23m ^%d   Now datatype= \e[0m<\e[31;7m%s\e[0m>\n", FLemit, nestDepth, bufCj->_qualDatatype)
#define SETout  \
		printf("\t\e[37;3m" FLfmt "\e[23m ^%d   Now out= \e[0m<\e[37;7m%s\e[0m>\n", FLemit, nestDepth, bufCj->_out)
#else
#define SETdatatyp
#define SETout
#endif

#ifdef DBG
#define SETtoken(s) do {   \
		strcpy(token,s);   \
		printf("\t\e[36;3m" FLfmt "\e[23m ^%d   Now token= \e[0m<\e[35;7m%s\e[0m>\n", FLemit, nestDepth, strlen(token)?token:"EMPTY");   \
	} while (0)
#define SETname(s) do {   \
		strcpy(bufCj->_name,s);   \
		printf("\t\e[33;3m" FLfmt "\e[23m ^%d   Now name= \e[0m<\e[33;7m%s\e[0m>\n", FLemit, nestDepth, bufCj->_name);   \
	} while (0)
#else
#define SETtoken(s) strcpy(token,s)
#define SETname(s) strcpy(bufCj->_name,s)
#endif

#ifdef DBG
#define GETTOtokty(expr) do {   \
		tokentype = (expr);   \
		printf("\t\e[35;3mcaller\e[23m" FLfmt "|  Getto  tokty=\e[1m%s\e[22m or %d"   \
		       "\e[0m\n", FLpass, toktyStr(tokentype), tokentype);  \
		return tokentype;   \
	} while (0)
#define GETTOtokty_and_tok(expr) do {   \
		tokentype = (expr);   \
		printf("\t\e[35;3mcaller\e[23m" FLfmt "|  Getto  tokty=\e[1m%s\e[22m or %d"   \
		       ";  Now token= \e[0m<\e[36;7m%s\e[0m>\n", FLpass, toktyStr(tokentype), tokentype, token);  \
		return tokentype;   \
	} while (0)
#else
#define GETTOtokty(expr) return (tokentype = (expr));
#define GETTOtokty_and_tok(expr) return (tokentype = (expr));
#endif

int gettoken(FLtake)  /* return next token */
{
	int c;
	char *p = token;
	skipWhitespace(&c);
	if (c == '(') {
		skipWhitespace(&c);  /** So doesn't choke on extra whtspc **/
		ungetch(FLemit,c);          /** So doesn't choke on extra whtspc **/
		if ((c = getch()) == ')') {
			strcpy(token, "()");
			GETTOtokty_and_tok(PARENS);
		} else {
			ungetch(FLemit,c);
			GETTOtokty('(');
		}
	} else if (c == '[') {
		for (*p++ = c; (*p++ = getch()) != ']'; )
			;
		*p = '\0';
		GETTOtokty_and_tok(BRACKETS);
	} else if (isalpha(c)) {
		for (*p++ = c; isalnum(c = getch()); )
			*p++ = c;
		*p = '\0';
		ungetch(FLemit,c);
		GETTOtokty_and_tok(NAME);
	} else
		GETTOtokty(c);
}
#undef GETTOtokty
#undef GETTOtokty_and_tok


/* Either
	o	Entire decl, except for storage-class specifiers.
		o	Terminated by LF.
		o	Name mandatory.
	o	A func param.
		o	Terminated by comma or Rparen.
		o	Name optional.
*/
void subdecl(int nestDepth, bufConj_t *bufCj, bool reuseDatatypeMode)
{
	if (reuseDatatypeMode) {
		gettoken(FLemit); /* For dcl()'s benefit. */
		printf("\n"); /* So remaining vars go on their own lines each. */
		/* We're keeping datatype and storage class specifiers; reset the rest. */
		memset(bufCj->_name,'\0',sizeof bufCj->_name);
		memset(bufCj->_pointerObj_cvQual,'\0',sizeof bufCj->_pointerObj_cvQual);
		memset(bufCj->_out,'\0',sizeof bufCj->_out);
		/* ungetch state */
		bufp=0;
		memset(buf,'\0',sizeof buf);
		/* and, of course, */
		bufCj->_did_printDeclDescription=false;
		goto done_with_datatype;
	}
	while (possPartOf_qualDatatype(token)) {
		vstrcat(bufCj->_qualDatatype, " ", token, NULL);
		SETdatatyp;
		SETtoken("");
		tokentype = gettoken(FLemit);
		continue;
	}
	ungetch(FLemit,tokentype);
	if (! *bufCj->_qualDatatype) {
		strcpy(bufCj->_qualDatatype, "int "); /* If datatype missing, assume int. */
		SETdatatyp;
	}
done_with_datatype:
	dcl(nestDepth, bufCj);	   /* parse rest of line */
#if 0
printf("\t\e[32m "FLfmt"  nestDepth=%d ; tokty='%s' or %d ; reuseDatatypeMode=%c  _did_printDeclDescription=%c\e[0m\n",FLemit,nestDepth,toktyStr(tokentype),tokentype,reuseDatatypeMode?'Y':'N',bufCj->_did_printDeclDescription?'Y':'N');
#endif
#if 1
		if (nestDepth == 0 && (reachedEnd_wholeDecl(tokentype)))
#else
	if (nestDepth == 0 && (tokentype==',' || reachedEnd_wholeDecl(tokentype)))
#endif
		printDeclDescription(bufCj);
}


void printDeclDescription (bufConj_t *bufCj)
{
			printf("%s: %s %s %s %s\n",
			       bufCj->_name, bufCj->_pointerObj_cvQual,
			       storageClassSpecifiers, bufCj->_out, bufCj->_qualDatatype);
	bufCj->_did_printDeclDescription=true;
}


typedef enum { plainPTR = 1, constPTR } pointyEnd_t;
/* dcl:  parse a declarator */
void dcl(int nestDepth, bufConj_t *bufCj)
{
	int ns, i=0;
	pointyEnd_t stack[20];
	memset(stack,'\0',sizeof stack);
	for (ns = 0;;) {
		if (ns)
			gettoken(FLemit);
		if (tokentype != '*') { /* Not pointy? */
			if (tokentype==NAME && !strcmp(token,"const")) {
				strcpy(bufCj->_pointerObj_cvQual,"const");
				continue;
			}
			break;
		}
		tokentype = gettoken(FLemit);
		if (NAME == tokentype && !strcmp(token,"const")) {
			stack[ns++] = constPTR;
		} else {
			stack[ns++] = plainPTR;
			if ('*' != tokentype) {
				ungetch(FLemit,tokentype);
				break;
			}
		}
	}
	dirdcl(nestDepth, bufCj);
	while (i < ns) {
		if (stack[i++] == constPTR)
			strcat(bufCj->_out, " const-pointer-to");
		else
			strcat(bufCj->_out, " pointer-to");
		SETout;
	}
}


void fuParms(int nestDepth, bufConj_t *bufCj)
{
	int expectMoreParams;
	if (tokentype == '(')
		gettoken(FLemit);
	for (;;) {
		char currparambuf[2048];
		bufConj_t sublevel;
		memset(&sublevel,'\0',sizeof sublevel);
		subdecl(nestDepth+1, &sublevel, /*reuseDatatypeMode?*/ false);
		sprintf(currparambuf, "%s: %s %s %s\n", sublevel._name,
		        sublevel._pointerObj_cvQual, sublevel._out, sublevel._qualDatatype);
		strcat(bufCj->_out, "\n");
		indent(nestDepth+1,bufCj->_out);
		strcat(bufCj->_out,currparambuf);
		SETout;
		expectMoreParams = tokentype == ',';
		if (! expectMoreParams) {
			const int peektokty = gettoken(FLemit);
			expectMoreParams = peektokty == ',';
			if (! expectMoreParams) {
				ungetch(FLemit,peektokty);
				break;
			}
		}
		gettoken(FLemit);
		sndent(nestDepth+1,bufCj->_out);
		strcat(bufCj->_out, ",\n");
		SETout;
	}
}


/* dirdcl:  parse a direct declarator */
void dirdcl(int nestDepth, bufConj_t *bufCj)
{
	int type;
	if (tokentype == '(') {
		int starting_fuParams;
		const peektokty = gettoken(FLemit);
		starting_fuParams = peektokty == NAME && possPartOf_qualDatatype(token);
		if (starting_fuParams) {
			ungetch(FLemit,peektokty);
			ungetch(FLemit, '(');
		} else {
			dcl(nestDepth,bufCj); /* As of old. */
			if (tokentype != ')')
				printf(FLfmt "\e[31;1merror: missing )\n\e[0m", FLemit);
		}
	} else if (tokentype == NAME) { /* variable name */
		SETname(token);
	} else {
		if (nestDepth && reachedEnd_fuParam(tokentype))
			ungetch(FLemit, tokentype);
	}
	while ((type=gettoken(FLemit)) == PARENS || type == BRACKETS || type == '(') {
		if (type == '(') {
			strcat(bufCj->_out, " function taking");
			fuParms(nestDepth, bufCj);
			if (tokentype != ')') {
				gettoken(FLemit);
				if (tokentype != ')')
					printf(FLfmt "\e[31;1merror: missing )\n\e[0m", FLemit);
			}
			indent(nestDepth, bufCj->_out);
			strcat(bufCj->_out, " and returning");
			return;
		} else if (type == PARENS)
			strcat(bufCj->_out, " function returning");
		else {
			vstrcat(bufCj->_out, " array", token, " of", NULL);
		}
		SETout;
	}
	if (type == ',' && nestDepth==0) {
		ungetch(FLemit,type);
	}
}

int getch(void)  /* get a (possibly pushed-back) character */
{
	return (bufp > 0) ? buf[--bufp] : getchar();
}
void ungetch(FLtake, int c)   /* push character back on input */
{
	if (bufp >= BUFSIZE)
		printf("ungetch: too many characters\n");
	else
		buf[bufp++] = c;
}
