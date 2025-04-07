/* Vainstein K 2025apr06 */
#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h> /* For  pow() */
#include <float.h> /* For  FLT_MANT_DIG and such. */


#define BUFSIZE 100
char buf[BUFSIZE]; /* Assume this will've been populated somehow */
int iNextAvail = 0; /* Assume this will've been reset to 0 */

int getch(void)  /* get a (possibly pushed-back) character */
{
    const int retval = (iNextAvail < strlen(buf)) ? buf[iNextAvail++] : EOF;
	printf("getch gets '%c' == %d\n", (char)retval, retval);
    return retval; /* (iNextAvail < strlen(buf)) ? buf[iNextAvail++] : EOF; */
}
void ungetch(int c)   /* push character back on input */
{
    if (! iNextAvail)
        printf("Cannot ungetch '%c' before buf populated\n", c);
    else {
		const char chprev = buf[--iNextAvail];
		if (chprev != c)
			printf("Strange: asking to ungetch '%c' but prevch '%c'\n", c,chprev);
		buf[iNextAvail] = c;
	}
}


   /* getint:  get next integer from input into *pn */
int getintBook(int *pn) /*Copied from p97.*/
{
   int c, sign;
   while (isspace(c = getch()))   /* skip white space */
	   ;
   if (!isdigit(c) && c != EOF && c != '+' && c != '-') {
	   ungetch(c);  /* it is not a number */
	   return 0;
   }
   sign = (c == '-') ? -1 : 1;
   if (c == '+' || c == '-')
	   c = getch();
   for (*pn = 0; isdigit(c); c = getch())
	   *pn = 10 * *pn + (c - '0');
   *pn *= sign;
   if (c != EOF)
	   ungetch(c);
   return c;
}
/* Contract:
	o	rets EOF if EOF;
	o	rets 0 if next inp not a valid number (integer in this case); contents of *pn indeterminate.
	o	rets some M>0 if inp contains a valid number, stored into *pn.
*/

int getintRefusesMereSign(int *const pn)
{
	int ctDigits = 0;
	int c, sign=0;
	while (isspace(c = getch()))   /* skip whitespace */
		;
	if (c == EOF)
		return c;
	if (!isdigit(c) && c != EOF && c != '+' && c != '-') {
		ungetch(c);  /* it is not a number */
		return 0;
	}
	if (c == '-')
		sign = -1;
	else if (c == '+')
		sign = 1;
	else {
		ungetch(c);
	}
	*pn = 0;
	while (isdigit(c = getch())) {
		++ctDigits;
		*pn *= 10;
		*pn += (c - '0');
	}
	if (sign)
		*pn *= sign;
	if (sign && ! ctDigits) {
		if (sign == -1)
			ungetch('-');
		else
			ungetch('+');
		return 0; /*A mere sign would be "not an integer". */
	}
	ungetch(c);
	return ctDigits;
}


typedef float ourFloat_t;

/* Same contract as getint(). */
int getfloat (ourFloat_t *const pf)
{
#define CHECK(cond) do {   \
	if (! (cond)) {   \
		printf("\t\e[31;7mLn%3d\e[0m  [ch='%c']  Cond \"" #cond "\" holds not.\n",   \
			__LINE__, (char)ch);   \
		return 0; }    } while (0)
	/* We shall consider an input valid iff all these hold:
		o	ctDigitsMant_integPart + ctDigitsMant_fractPart > 0.
		o	Sign must appear before corresponding digits.
		o	Either no "e" symbol, or "e" symbol followed by opt'l sign and ctDigitsExp > 0.
		o	ctDigitsMant_integPart <= FLT_MANT_DIG.         // See float.h
		o	ctDigitsMant_fractPart <= FLT_DIG.              // See float.h
		o	FLT_MIN_EXP <= exp <= FLT_MAX_EXP.              // See float.h
	*/
	bool nowMant_integPart=true, nowMant_fractPart=false, nowExp=false;
	int ctDigitsMant_integPart=0, ctDigitsMant_fractPart=0, ctDigitsExp=0;
		/* fractPart of mantissa is also called "digits of precision". */
	int valMant_integPart=0, valMant_fractPart=0, valExp=0;
	int signMant=0, signExp=0, ch;
	while (isspace(ch = getch()))   /* skip whitespace */
		;
	if (ch == EOF)
		return ch;
	while (! isspace(ch) && ch != EOF) {
		/* We must always be in exactly __one__ phase. */
		CHECK(1 == ((nowMant_integPart?1:0) + (nowMant_fractPart?1:0) + (nowExp?1:0)));
		if (ch == '.') {
			CHECK(nowMant_integPart);
			nowMant_integPart=false; nowMant_fractPart=true;
		} else if (tolower(ch) == 'e') {
			CHECK(! nowExp);
			nowMant_integPart=false; nowMant_fractPart=false; nowExp=true;
		} else if (ch == '-' || ch == '+') {
			CHECK(! nowMant_fractPart);
			if (nowMant_integPart) {
				CHECK(! ctDigitsMant_integPart);
				CHECK(! signMant);
				signMant = (ch=='+') ? 1 : -1;
			} else {
				CHECK(! ctDigitsExp);
				CHECK(! signExp);
				signExp = (ch=='+') ? 1 : -1;
			}
		} else {
			CHECK(isdigit(ch));
			int *pval=NULL, *pct=NULL;
			if      (nowMant_integPart) {   pval = &valMant_integPart;   pct=&ctDigitsMant_integPart;   }
			else if (nowMant_fractPart) {   pval = &valMant_fractPart;   pct=&ctDigitsMant_fractPart;   }
			else                        {   pval = &valExp;              pct=&ctDigitsExp;   }
			++*pct;
			*pval *= 10;
			*pval += (ch - '0');
		}
		ch = getch();
	}
	CHECK((ctDigitsMant_integPart+ctDigitsMant_fractPart) > 0);
	if (nowExp)
		CHECK(ctDigitsExp); /* If exponent was promised, we demand some exponent digits. */
	CHECK(ctDigitsMant_integPart <= FLT_MANT_DIG);
	CHECK(ctDigitsMant_fractPart <= FLT_DIG);
	if (signExp)
		valExp *= signExp;
	CHECK(FLT_MIN_EXP <= valExp);
	CHECK(valExp <= FLT_MAX_EXP);
	/* Finally satisfied, we assemble answer. */
	*pf = (ourFloat_t)valMant_integPart;
	if (ctDigitsMant_fractPart)
		*pf += (ourFloat_t)valMant_fractPart / pow(10.0 , ctDigitsMant_fractPart);
	if (signMant)
		*pf *= (ourFloat_t)signMant;
	if (ctDigitsExp)
		*pf *= pow(10.0 , valExp);
	return 1;
#undef CHECK
}
/* Bit of a slog, but quite comprehensive in error checking. */



main ()
{


{	int report, answConv;
#define TEST_getintRefusesMereSign(s)   \
	memset(buf, '\0', sizeof buf);   \
	strcpy(buf, (s));   \
	iNextAvail = 0;   \
	report = -10002;   \
	answConv = -30004;   \
	report = getintRefusesMereSign(&answConv);    \
	printf("Ln%3d:   getintRefusesMereSign(\"%s\")   ===>   %d\n", __LINE__, s, report);   \
	if (report > 0)   \
		printf("  \\_____ and stored in answConv:  %d\n", answConv)
	TEST_getintRefusesMereSign("+");
	TEST_getintRefusesMereSign("+5");
	TEST_getintRefusesMereSign("5");
}

	printf("\e[35m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\e[0m\n");
{	int report;
	ourFloat_t answConv;
	/* Test macro will set both report and answConv to some easily recognizable bogus values, before invoking getfloat(). */
#define TEST_getfloat(s)      \
	memset(buf, '\0', sizeof buf);   \
	strcpy(buf, (s));   \
	iNextAvail = 0;   \
	report = -10002;   \
	answConv = -300.04F;   \
	report = getfloat(&answConv);    \
	printf("Ln%3d:   getfloat(\"%s\")   ===>   %d\n", __LINE__, s, report);   \
	if (report > 0)   \
		printf("  \\_____ and stored in answConv:  %.6g\n", answConv)
	/* Expect success */
	TEST_getfloat("5");
	TEST_getfloat(".6");
	TEST_getfloat("5.6");
	TEST_getfloat("+5");
	TEST_getfloat("-5");
	TEST_getfloat("+.6");
	TEST_getfloat("-.6");
	TEST_getfloat("+5.6");
	TEST_getfloat("-5.6");
	TEST_getfloat("5e7");
	TEST_getfloat(".6e7");
	TEST_getfloat("5."); /* Strange but legal. */
	TEST_getfloat("5.e7"); /* Strange but legal. */
	TEST_getfloat("5.6e7");
	TEST_getfloat("5.6E7");
	TEST_getfloat("5.6E0"); /* Silly but legal. */
	TEST_getfloat("5.6E-1");
	/* Expect failure */
	TEST_getfloat("");
	TEST_getfloat(".");
	TEST_getfloat("e7");
	TEST_getfloat(".e7");
	TEST_getfloat("5e");
	TEST_getfloat("5e6e7");
	TEST_getfloat("5.6.7");
	TEST_getfloat("5..7");
	TEST_getfloat("..7");
	TEST_getfloat("5..");
	TEST_getfloat("+");
	TEST_getfloat("++5");
	TEST_getfloat("8-e9");
	TEST_getfloat("five");
	TEST_getfloat("-");
	TEST_getfloat("+e7");
	TEST_getfloat("5.6e7yeah!");
}

}
