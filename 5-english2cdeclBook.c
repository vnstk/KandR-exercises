/* What K&R call "undcl" */

/*=================================== copied from pp 124-125: BEGIN ===*/
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
/*=================================== copied from pp 124-125: END ===*/

/*=================================== copied from p 126: BEGIN ===*/
   /* undcl:  convert word descriptions to declarations */
   main()
   {
       int type;
       char temp[MAXTOKEN];
       while (gettoken() != EOF) {
           strcpy(out, token);
           while ((type = gettoken()) != '\n')
               if (type == PARENS || type == BRACKETS)
                   strcat(out, token);
               else if (type == '*') {
                   sprintf(temp, "(*%s)", out);
                   strcpy(out, temp);
               } else if (type == NAME) {
                   sprintf(temp, "%s %s", token, out);
                   strcpy(out, temp);
               } else
                   printf("invalid input at %s\n", token);
       }
       return 0;
   }
/*=================================== copied from p 126: END ===*/


/*=================================== copied from p 79: BEGIN ===*/
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
/*=================================== copied from p 79: END ===*/
