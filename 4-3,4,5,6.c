/* Vainstein K 2025mar23 --- my work is the diff between 4-calcBook.c and this file */
/* To run scripted tests,
							bash 4-3,4,5,6_runTests.sh
*/
/* Summary of changes:
	o	Parsing of doubles now done by strtod(), not atof(); that incicdentally gains us negative numbers.

	o	Added functionality commands:
					--	--		top
					--	--		dup
					--	--		swap
					--	--		clear
					--	X		sin|exp|log|sqrt
					X	Y		pow
					X	Y		%
					X	[a-z]	store
					--	[a-z]	load

	o	Added debugging commands:
					--	--		prstk
					--	--		prsym

	o	Stack elements are now typed operand_t, not double.

	o	Command logic moved out of main() and into per-command functions: f_add(), f_Sub(), etc.

	o	Command dispatch now done by scanning a lookup table, not by lengthy if-else-else... logic.

	o	Up to 26 variables on which store() and load() can operate; special "_" variable on which only load() may operate.

	o	Error handling now more comprehensive, and error reporting more informative (see the FL* macros).

	o	getop() renamed to gettok().

	o	'val' should ideally be renamed, to say 'stack'.

	o	No longer an error to arrive at newline with an empty stack.  (Was causing distracting errors in multi-line tests.)  Since this does change behavior, and not in a way asked for by the exercises, have made it conditional on a constant (EMPTY_STACK_AT_EOL_IS_ERROR) overridable at compiletime.
*/
#ifndef EMPTY_STACK_AT_EOL_IS_ERROR
#	define EMPTY_STACK_AT_EOL_IS_ERROR 0
#endif

typedef enum { kindBAD=0, kindNUM=1, kindVAR=2 } operand_kind_t;

typedef struct {
	union uValue {
		double _num;
		char   _var;
	}               _value;
	operand_kind_t  _kind;
} operand_t;


/* Want to print decent errmsg, which means context; func name + line */
#define FLemit __func__,__LINE__
#define FLtake const char *fu, const int li
#define FLfmt "/%s:%d/   "
#define FLpass fu, li

#include <stdio.h>

/* A few debugging aids
*/
const char *sprintf_operand (const operand_t *const x)
{
	static char buf[1024];
	if (x->_kind == kindNUM)
		sprintf(buf, "%g", x->_value._num);
	else if (x->_kind == kindVAR)
		sprintf(buf, "%c", x->_value._var);
	else  
		sprintf(buf, "???");
	return buf;
}
void printf_operand (const operand_t *const x)
{
	printf("\t%s\n", sprintf_operand(x));
}
void print_stack (void);


#define symtabSIZE 27
/* We've 27 variables: special "_" pseudo-variable, and the 26 lowercase letters. */
static const char firstSymName = '_';
int symNameToOrd (const char ch) {
	if (ch == firstSymName)
		return 0;
	else if (ch >= 'a' && ch <= 'z')
		return ch -'a' + 1;
	else
		return -1;
}
char symOrdToName (const int ord) {
	return (ord == 0) ? firstSymName : ('a' + ord - 1);
}
void storeMostRecentlyPrintedNumber (double f);

#include <stdlib.h> /* for  strtod() */
#define MAXOP   100  /* max size of operand or operator */

int gettok(char []);

int stack_depth (void);
double    popNum (FLtake);
char      popVar (FLtake);
operand_t popAny (FLtake);
void pushNum (FLtake, double);
void pushVar (FLtake, char);
void pushAny (FLtake, operand_t);
void peekAny (FLtake, operand_t*);

#define CHKcond(cond,errmsgIfFalse) do {   \
		if (! (cond)) {   \
			printf(FLfmt "ERR: " errmsgIfFalse "\n", FLemit);   \
			return;   \
		}   \
	} while (0)

/* We'll use INT_MAX to indicate that one of our own errors happened; we
   shall have already printed an errmsg in that case. */
#define CHKerrno() do {   \
		if (errno) {   \
			if (errno < INT_MAX)   \
				printf(FLfmt "ERR: %s\n", FLemit, strerror(errno));   \
			return;   \
		}   \
	} while (0)


#include <errno.h>
#include <math.h>
#include <string.h>


typedef void (*command_t) (void) ;
/* Caller should zero errno before invoking any of these.
*/
void f_add (void) {
	const double op2 = popNum(FLemit); /* Would be more compact to declare both op2 */
	const double op1 = popNum(FLemit); /* and op1 on a single line, but then the    */
	CHKerrno();                        /* func:lineNum info would be same for both  */
	pushNum(FLemit, op1 + op2);        /* popNum() invocations.                     */
}
void f_sub (void) {
	const double op2 = popNum(FLemit);
	const double op1 = popNum(FLemit);
	CHKerrno();
	pushNum(FLemit, op1 - op2);
}
void f_mul (void) {
	const double op2 = popNum(FLemit);
	const double op1 = popNum(FLemit);
	CHKerrno();
	pushNum(FLemit, op1 * op2);
}
void f_div (void) {
	const double op2 = popNum(FLemit);
	const double op1 = popNum(FLemit);
	CHKerrno();
	CHKcond( (op2 != 0.0), "zero divisor");
	pushNum(FLemit, op1 / op2);
}
void f_mod (void) {
	const double op2 = popNum(FLemit);
	const double op1 = popNum(FLemit);
	const int op2int = (int)op2, op1int = (int)op1;
	CHKerrno();
	CHKcond( (op2int), "zero modulus");
	pushNum(FLemit, op1int % op2int);
}
void f_top (void) {
	operand_t copyOfTop;
	peekAny(FLemit, &copyOfTop);
	CHKerrno();
	printf_operand(&copyOfTop);
	if (copyOfTop._kind == kindNUM)
		storeMostRecentlyPrintedNumber(copyOfTop._value._num);
}
void f_dup (void) {
	operand_t copyOfTop;
	peekAny(FLemit, &copyOfTop);
	CHKerrno();
	pushAny(FLemit, copyOfTop);
}
void f_swap (void) {
	const operand_t op2 = popAny(FLemit);
	const operand_t op1 = popAny(FLemit);
	CHKerrno();
	pushAny(FLemit, op2);
	pushAny(FLemit, op1);
}
void f_clear (void) {
	int nElems = stack_depth();
	while (nElems--)
		(void) popAny(FLemit);
}
void f_sin (void) {
	const double op1 = popNum(FLemit);
	CHKerrno();
	const double answ = sin(op1);
	/* No need to check errno here: sin always succeeds. */
	pushNum(FLemit, answ);
}
void f_exp (void) {
	const double op1 = popNum(FLemit);
	CHKerrno();
	const double answ = exp(op1);
	CHKerrno(); /* Will respond to errno==EDOM or errno==ERANGE if exp() had set that. */
	pushNum(FLemit, answ);
}
void f_log (void) {
	const double op1 = popNum(FLemit);
	CHKerrno();
	const double answ = log(op1);
	CHKerrno(); /* Will respond to errno==EDOM or errno==ERANGE if exp() had set that. */
	pushNum(FLemit, answ);
}
void f_sqrt (void) {
	const double op1 = popNum(FLemit);
	CHKerrno();
	const double answ = sqrt(op1);
	CHKerrno(); /* Will respond to errno==EDOM or errno==ERANGE if exp() had set that. */
	pushNum(FLemit, answ);
}
void f_pow (void) {
	const double op2 = popNum(FLemit);
	const double op1 = popNum(FLemit);
	CHKerrno();
	const double answ = pow(op1,op2);
	pushNum(FLemit, answ);
}


#include <stdbool.h>

typedef struct {
	double  _value;
	bool    _defined;
} variable_t;
static variable_t symtab[symtabSIZE]; /* Zero-initialized, hence all initially undefined. */

void storeMostRecentlyPrintedNumber (double f) {
	symtab->_value = f;
	symtab->_defined = true;
}

/* Possible improvement: popNum() et al to figure location of cmd from
   input ptr, and we can then dispense with passing of __func__ all over. */
void f_store (void) {
	const char op2 = popVar(FLemit);
	const double op1 = popNum(FLemit);
	CHKerrno();
	CHKcond((op2 != firstSymName), "may not explicitly store into the '_' pseudo-variable");
	variable_t *var = symtab + symNameToOrd(op2);
	var->_value = op1;
	var->_defined = true;
}
void f_load (void) {
	const char op1 = popVar(FLemit);
	CHKerrno();
	variable_t *var = symtab + symNameToOrd(op1);
	CHKcond( (var->_defined) , "variable not defined yet");
	pushNum(FLemit, var->_value);
}
void f_prsym (void) { /* For debugging; dumps symbol table i.e. our variables */
	int i;
	printf("\t\t\e[35mDefined vars: {\e[0m\n");
	for (i = 0; i < symtabSIZE; ++i) {
		const variable_t *var = symtab + i;
		if (var->_defined)
			printf("%s\e[35;1m%c\e[0m=\e[32m%g\e[0m",
			       (i ? " " : "\t\t\t"), symOrdToName(i), var->_value);
	}
	printf("\n\t\t\e[35m}\e[0m\n");
}
void f_prstk (void) { /* For debugging */
	print_stack();
}


typedef struct {
	char       _name[6];
	command_t  _func;
} registration_t;

static const registration_t roster[] = {
	{"+",f_add} ,
	{"*",f_mul} ,
	{"-",f_sub} ,
	{"/",f_div} ,
	{"%",f_mod} ,
	{"top",f_top} ,
	{"dup",f_dup} ,
	{"swap",f_swap} ,
	{"clear",f_clear} ,
	{"sin",f_sin} ,
	{"exp",f_exp} ,
	{"log",f_log} ,
	{"sqrt",f_sqrt} ,
	{"pow",f_pow} ,
	{"store",f_store} ,
	{"load",f_load} ,
	{"prstk",f_prstk} ,
	{"prsym",f_prsym} ,
};
static const int rosterSIZE = sizeof(roster) / sizeof(roster[0]);



/* reverse Polish calculator */
main()
{
	int type, i;
	double f;
	char s[MAXOP];
	while ((type = gettok(s)) != EOF) {
		if (type == '\n') {
			if (EMPTY_STACK_AT_EOL_IS_ERROR || stack_depth()) {
				errno=0;
				f = popNum(FLemit);
				if (! errno) {
					printf("\t%.8g\n", f);
					storeMostRecentlyPrintedNumber(f);
				}
			}
		} else {
			const int sLen = strlen(s);
			if (sLen == 1 && (symNameToOrd(s[0]) != -1)) {
				pushVar(FLemit, s[0]);
			} else {
				char *unconvertedSuffix = NULL;
				errno=0;
				f = strtod(s, &unconvertedSuffix);
				if (errno) { /* fabs(f)<DBL_EPSILON || f<DBL_MIN || f>DBL_MAX */
					printf(FLfmt "ERR: numeric operand \"%s\" unusable: %s\n",
					       FLemit, s, strerror(errno));
				} else if ((int)(unconvertedSuffix - s) == sLen) {
					pushNum(FLemit, f);
				} else {
					for (i = 0; i < rosterSIZE; ++i)
						if (! strcmp(s, roster[i]._name)) {
							errno=0;
							roster[i]._func();
							break;
						}
					if (i == rosterSIZE) /* if we found no match in roster */
						printf(FLfmt "ERR: unknown command \"%s\"\n", FLemit, s);
				}
			}
		}
	}
}


#define MAXVAL  100  /* maximum depth of val stack */
int sp = 0;		  /* next free stack position */
operand_t val[MAXVAL];  /* value stack */

void print_stack (void)
{
	int i;
	printf("\t\t\e[35m%d-deep stack:\e[0m\n", sp);
	for (i = 0; i < sp; ++i)
		printf("\t\t\t\e[35;1m%2d:\e[0m  <\e[32m%s\e[0m>\n", i, sprintf_operand(val+i));
}

int stack_depth (void)
{
	return sp;
}

void peekAny (FLtake, operand_t *const dest)
{
	memset(dest, '\0', sizeof(operand_t));
	if (0 == sp) {
		printf(FLfmt "ERR: empty stack, can't peek\n", FLpass);
		errno = INT_MAX;
	} else {
		const operand_t *src = & val[sp-1];
		memcpy(dest, src, sizeof(operand_t));
	}
}

/* push:  push f onto value stack */
void pushAny (FLtake, operand_t x)
{
	if (sp < MAXVAL)
		val[sp++] = x;
	else {
		printf(FLfmt "ERR: stack full\n", FLpass);
		errno = INT_MAX;
	}
}
void pushNum (FLtake, double f)
{
	operand_t x;
	x._kind = kindNUM;
	x._value._num = f;
	pushAny(FLpass, x);
}
void pushVar (FLtake, char v)
{
	operand_t x;
	x._kind = kindVAR;
	x._value._var = v;
	pushAny(FLpass, x);
}

/* pop:  pop and return top value from stack */
operand_t popAny (FLtake)
{
	if (sp > 0)
		return val[--sp];
	else {
		printf(FLfmt "ERR: stack empty\n", FLpass);
		errno = INT_MAX;
		operand_t dummy;
		memset(&dummy, '\0', sizeof dummy); /* This sets _kind to kindBAD. */
		return dummy;
	}
}
double popNum (FLtake)
{
	errno = 0;
	operand_t x = popAny(FLpass);
	if (! errno && x._kind != kindNUM) {
		printf(FLfmt "ERR: popped operand <%s> is not a number\n", FLpass, sprintf_operand(&x));
		errno = INT_MAX;
	}
	return errno ? 1.2345 /*some dummy double*/ : x._value._num;
}
char popVar (FLtake)
{
	errno = 0;
	operand_t x = popAny(FLpass);
	if (! errno && x._kind != kindVAR) {
		printf(FLfmt "ERR: popped operand <%s> is not a variable\n", FLpass, sprintf_operand(&x));
		errno = INT_MAX;
	}
	return errno ? '@' /*some dummy char*/ : x._value._var;
}


int getch(void);
void ungetch(int);

/* gettok:  get next token (command name or variable operand or numeric operand) */
/* Will return '\n' if encountered '\n', EOF if encountered EOF, else 0. */
int gettok(char s[])
{
	int i, c;
	/* Skip any leading spaces or tabs */
	while((c = getch()) == ' ' || c == '\t')
		;
	if (c == '\n' || c == EOF)
		return c;
	memset(s, '\0', MAXOP);
	i = 0;
	s[i++] = c;
	while ((c = getch()) != ' ' && c != '\t' && c != '\n' && c != EOF)
		s[i++] = c;
	if (c != EOF)
		ungetch(c);
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
