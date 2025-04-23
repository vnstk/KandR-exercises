/* Vainstein K 2025mar28 --- my work is the diff between 4-calcBook.c and this file */
/*Test steps:
	./a.exe <<<'1 2 - 4 5 + *'
  ; should print -9.
*/

#include <ctype.h>
#include <stdio.h>
#include <string.h> /* for  memset() */
#include <stdbool.h>
#include <stdlib.h>  /* for  atof() */

#define MAXOP   100  /* max size of operand or operator */
#define NUMBER  '0'  /* signal that a number was found */
int getop(char []);
void push(double);
double pop(void);
/* reverse Polish calculator */
main()
{
	int type;
	double op2;
	char s[MAXOP];
	while ((type = getop(s)) != EOF) {
		switch (type) {
		case NUMBER:
			push(atof(s));
			break;
		case '+':
			push(pop() + pop());
			break;
		case '*':
			push(pop() * pop());
			break;
		case '-':
			op2 = pop();
			push(pop() - op2);
			break;
		case '/':
			op2 = pop();
			if (op2 != 0.0)
				push(pop() / op2);
			else
				printf("error: zero divisor\n");
			break;
		case '\n':
			printf("\t%.8g\n", pop());
			break;
		default:
			printf("error: unknown command %s\n", s);
			break;
		}
	}
	return 0;
}

#define MAXVAL  100  /* maximum depth of val stack */
int sp = 0;		  /* next free stack position */
double val[MAXVAL];  /* value stack */
/* push:  push f onto value stack */
void push(double f)
{
	if (sp < MAXVAL)
		val[sp++] = f;
	else
		printf("error: stack full, can't push %g\n", f);
}
/* pop:  pop and return top value from stack */
double pop(void)
{
	if (sp > 0)
		return val[--sp];
	else {
		printf("error: stack empty\n");
		return 0.0;
	}
}



int getch(void);
void ungetch(int);
/* getop:  get next character or numeric operand */
int getop(char s[])
{
	int i, c;
	while ((s[0] = c = getch()) == ' ' || c == '\t')
		;
	s[1] = '\0';
	if (!isdigit(c) && c != '.')
		return c;	  /* not a number */
	i = 0;
	if (isdigit(c))	/* collect integer part */
		while (isdigit(s[++i] = c = getch()))
		   ;
	if (c == '.')	  /* collect fraction part */
		while (isdigit(s[++i] = c = getch()))
		   ;
	s[i] = '\0';
	if (c != EOF)
		ungetch(c);
	return NUMBER;
}

/* While getch() and ungetch() could be dispensed with in terms of correctness,
   readability would suffer.  So, instead re-implement them.
*/
#define BUFSIZE 100
char buf[BUFSIZE];	/* buffer for ungetch */
int bufp = 0;            /* next free position in buf */ 

char linbuf[BUFSIZE]; /* For getline and getch and ungetch. */
int iNextAvailCh = -1;


/* Copied from 1-20.c */
bool getline (char *const buf, const int buflen)
{
    memset(buf, '\0', buflen);
    int c=EOF, i;
    for (i=0; (c=getchar())!=EOF && c!='\n'; ++i) {
        if (i >= buflen-2) {
            fprintf(stderr, "Buffer too small.\n");
            return false;
        }
        buf[i] = c;
    }
    return c != EOF;
}

int getch(void)  /* get a (possibly pushed-back) character */
{
	if (iNextAvailCh < 0 || iNextAvailCh >= strlen(linbuf)) {
		const bool keepGoing = getline(linbuf, sizeof linbuf); /* If EOF not reached */
		const size_t len = strlen(linbuf);
printf("getch didst getline[%s] len=%zd keepGoing=%c\n", linbuf, len, keepGoing?'Y':'N');
		if (keepGoing)
			linbuf[len] = '\n';
		else
			return EOF;
		iNextAvailCh = 0;
	}
	return linbuf[iNextAvailCh++];
}

void ungetch(int c)   /* push character back on input */
{
	if (iNextAvailCh < 0 || iNextAvailCh >= strlen(linbuf)) {
		printf("\t\tCannot ungetch '%d' before any, or after all, input read in.\n", c);
		return;
	}
	if (! iNextAvailCh) {
		printf("\t\tCannot ungetch '%d' before having ever getched it.\n", c);
		return;
	}
	const int prevch = linbuf[iNextAvailCh-1];
	if (prevch != c) {
		printf("\t\tTrying to ungetch '%d', but prev given char was '%d'.\n",c,prevch);
	}
	--iNextAvailCh;
}
