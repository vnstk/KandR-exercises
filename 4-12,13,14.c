/* Vainstein K 2025mar22 */
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

void itoaRecursive (int n, char *s) /* Ex 4-12 */
{
	const bool isNeg = n < 0; 
	if (abs(n) < 10) {
		if (isNeg)
			s[0] = '-';
		s[strlen(s)] = '0' + abs(n);
	} else {
		itoaRecursive(n / 10, s);
		const int sLen = strlen(s);
		s[sLen] = '0' + (abs(n) % 10);
	}
}

void reverseRecursive (char *const s) /* Ex 4-13 */
{
	const int sLen = strlen(s);
	if (sLen > 1) {
		char tmp = s[sLen-1];
		s[sLen-1] = '\0';
		reverseRecursive(s+1);
		s[sLen-1] = s[0];
		s[0] = tmp;
	}
}

#define swap(t,x,y) /*Ex 4-14 */ \
	do {                         \
		t tmp = x;               \
		x = y;                   \
		y = tmp;                 \
	} while(0)

main ()
{
	char buf[64];

#define TEST_itoaRecursive(n)        \
	memset(buf, '\0', sizeof buf);   \
	itoaRecursive((n), buf);         \
	printf("Ln%3d:   itoaRecursive(%7d)  ===>  \"%s\"\n", __LINE__, (n), buf)

	TEST_itoaRecursive( 314159);
	TEST_itoaRecursive(      9);
	TEST_itoaRecursive(     -9);
	TEST_itoaRecursive(-314159);
	TEST_itoaRecursive(      0);

	printf("\e[35m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\e[0m\n");
#define TEST_reverseRecursive(s)   \
	strcpy(buf, s);                \
	reverseRecursive(buf);         \
	printf("Ln%3d:   reverseRecursive(\"%s\")  ===>  \"%s\"\n", __LINE__, s, buf)

	TEST_reverseRecursive("abcdef"); /*even # of chars*/
	TEST_reverseRecursive("abcde"); /*odd # of chars*/
	TEST_reverseRecursive("ab");
	TEST_reverseRecursive("a");
	TEST_reverseRecursive("");

	printf("\e[35m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\e[0m\n");
#define TEST_swap(t,fmtCode)   \
	t xsav = x, ysav = y;      \
	swap(t,x,y);               \
	printf("Ln%3d:   swap("fmtCode","fmtCode")   ===>   {"fmtCode","fmtCode")\n",   \
	       __LINE__, xsav, ysav, x, y)

	{	char x = 'e', y = 'k';
		TEST_swap(char,"%c");
	}
	{	float x = 3.14, y = 0.789;
		TEST_swap(float,"%f");
	}
	{	unsigned x = 314, y = 56789;
		TEST_swap(unsigned,"%u");
	}
	 {/*const char *x = "foo", *y = "bar";*/
		/* Won't work.  But could work if t were the last arg instead of first. */
	}
}
