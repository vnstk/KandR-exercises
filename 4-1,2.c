/* Vainstein K 2025mar22 */
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void reverse (char s[])
{
    int i, slen = strlen(s);
    for (i = 0; i < slen/2; ++i) {
        char c = s[i];   s[i] = s[slen - 1 - i];   s[slen - 1 - i] = c;
    }
}

/* Finds Lmost pos of t in s */
int strindexBook (char s[], char t[]) /* copied from p69 */
{
	int i, j, k;
	for (i = 0; s[i] != '\0'; i++) {
		for (j=i, k=0; t[k]!='\0' && s[j]==t[k]; j++, k++)
			;
		if (k > 0 && t[k] == '\0')
			return i;
	}
	return -1;
}

/* Finds Rmost pos of t in s */
int strrindex (char /*haystack*/s[], char /*needle*/t[]) /* Ex 4-1 */
{
	/* This is arguably cheating; yet is surely good engineering. */
	char sCopy[64], tCopy[64];
	strcpy(sCopy,s);   reverse(sCopy);
	strcpy(tCopy,t);   reverse(tCopy);
	int answ = strindexBook(sCopy,tCopy);
	return answ;
}

double atofBook (char s[]) /* copied from p71 */
{
	double val, power;
	int i, sign;
	for (i = 0; isspace(s[i]); ++i) /*skip leading whtspc */
		;
	sign = (s[i] == '-') ? -1 : 1;
	if (s[i] == '+' || s[i] == '-')
		++i;
	for (val = 0.0; isdigit(s[i]); ++i)
		val = 10.0 * val + (s[i] - '0');
	if (s[i] == '.')
		++i;
	for (power = 1.0; isdigit(s[i]); ++i) {
		val = 10.0 * val + (s[i] - '0');
		power *= 10.0;
	}
	return sign * val / power;
}

double atofSci (char s[])
{
	bool dotSeen=false, exponentSeen=false;
	double val;
	int i, sign, digitsAfterDot=0, exponent=0;
	for (i = 0; isspace(s[i]); ++i) /*skip leading whtspc */
		;
	sign = (s[i] == '-') ? -1 : 1;
	if (s[i] == '+' || s[i] == '-')
		++i;
	for (val = 0.0; isdigit(s[i]) || s[i]=='.'; ++i) {
		if (! dotSeen && (dotSeen = s[i] == '.'))
			continue;
		if (dotSeen)
			++digitsAfterDot;
		val = 10.0 * val + (s[i] - '0');
	}
	if (tolower(s[i]) == 'e') {
		++i;
		exponentSeen=true;
		exponent = atoi(s + i);
	}
	const double sansExponent = sign * val / pow(10.0,(double)digitsAfterDot);
	return exponentSeen ? sansExponent*pow(10.0,(double)exponent) : sansExponent;
}

main ()
{
	int answPos;
#define TEST_strrindex(s,t)      \
	answPos = strrindex(s,t);    \
	printf("Ln%3d:   strrindex(|%2zu|\"%s\" , \"%s\"|%zu|)   ===>   %d\n",   \
	       __LINE__, strlen(s), s, t, strlen(t), answPos)
	TEST_strrindex("abcdefghij","efg");
	TEST_strrindex("abcdefghij","gfe");
	TEST_strrindex("abcdefghij","xyz");
	TEST_strrindex("abcdefghij","abd");
	TEST_strrindex("abcdefghij","f");
	TEST_strrindex("abcdefghij","a");
	TEST_strrindex("abcdefghij","j");
	TEST_strrindex("abcdefghij","abcdefghij");

	printf("\e[35m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\e[0m\n");
	double answConv;
#define TEST_atofSci(s)     \
	answConv = atofSci(s);  \
	printf("Ln%3d:   atofSci(\"%s\")   ===>   %.6f\n", __LINE__, s, answConv)
	TEST_atofSci("3");
	TEST_atofSci("3.");
	TEST_atofSci("3.14159");
	TEST_atofSci( ".14159");
	TEST_atofSci("3e7");
	TEST_atofSci("3.e7");
	TEST_atofSci(".3e7");
	TEST_atofSci("3.1415e7");
	TEST_atofSci("3.1415e-7");
	TEST_atofSci("-3.1415e7");
	TEST_atofSci("-3.1415e-7");
	TEST_atofSci("1.000732e106");
	TEST_atofSci("3e4"); /*ought to be 30000 */
}
