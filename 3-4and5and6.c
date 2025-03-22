/* Vainstein K 2025mar22 */
#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef signed short ourSigned_t; /* The "int" we'll be using as study aid. */
#define WORD_BITS (sizeof(ourSigned_t) * 8)
#define BINSTR_BUFLEN (WORD_BITS  +  ((WORD_BITS/4)-1)  +  1)

const char *binstr (const ourSigned_t z, char *const buf)
{
	int iBuf, jBit, val;
	for (iBuf = 0, jBit = 0; jBit < WORD_BITS; ++jBit) {
		if (jBit && ! (jBit % 4))
			buf[iBuf++] = '\''; /* nybble separator mark */
		val = z & (0x1 << (WORD_BITS - 1 - jBit));
		buf[iBuf++] = jBit ? (val ? '1' : '0') : (val ? '-' : '+');
	}
	buf[iBuf] = '\0';
	return buf;
}

void reverse (char s[])
{
	int i, slen = strlen(s);
	for (i = 0; i < slen/2; ++i) {
		char c = s[i];   s[i] = s[slen - 1 - i];   s[slen - 1 - i] = c;
	}
}

void itoaBook (ourSigned_t n, char s[]) /* copied from p64 */
{
	int i, sign;
	if ((sign = n) < 0)
		n = -n;
	i = 0;
	do {
		s[i++] = n % 10 + '0';
	} while ((n /= 10) > 0);
	if (sign < 0)
		s[i++] = '-';
	s[i] = '\0';
	reverse(s);
}

void itoaEvenMIN (int n, char s[]) /* Ex 3-4 */
{
	const bool isNeg = n < 0;
	int i = 0;
	do {
		s[i++] = abs(n % 10) + '0';
	} while (n /= 10);
	if (isNeg)
		s[i++] = '-';
	s[i] = '\0';
	reverse(s);
}

void itob (int n, char s[], int /*base*/ b) /* Ex 3-5 */
{
	assert(b>=2 && b<=36);
	const bool isNeg = n < 0;
	int i = 0, val;
	do {
		val = abs(n % b);
		s[i++] = (val < 10) ? (val + '0') : (val - 10 + 'a');
	} while (n /= b);
	if (isNeg)
		s[i++] = '-';
	s[i] = '\0';
	reverse(s);
}

void itoaPadding (int n, char s[], int minFieldWidth) /* Ex 3-6 */
{
	int nBlanks, i, sLen;
	itoaEvenMIN(n,s);
	sLen = strlen(s);
	if ((nBlanks = minFieldWidth - sLen) > 0) {
		for (i = minFieldWidth-1; i >= nBlanks; --i)
			s[i] = s[i - nBlanks]; /* Pretend I don't know of memcpy */
		memset(s, ' ', nBlanks);
	}
}

main ()
{
	char bufA[BINSTR_BUFLEN], bufito[66];
	short x;

#define TEST_itoaBook(n)                               \
	x = (n);   binstr(x,bufA);   itoaBook(x,bufito);   \
	printf("Ln%3d:   itoaBook(%10s  %6hd  %s)   ===>   \"%s\"\n",   \
	       __LINE__, #n, x, bufA, bufito)
	TEST_itoaBook(19);
	TEST_itoaBook(-19);
	TEST_itoaBook(SHRT_MAX);
	TEST_itoaBook(SHRT_MIN);
	TEST_itoaBook(SHRT_MIN+1);
	TEST_itoaBook(-SHRT_MAX);
	TEST_itoaBook(-SHRT_MIN);
	/* itoaBook(SHRT_MIN) doesn't work because it runs "n = -n", and
	   -32768 is not representable with 2's compl. */

	printf("\e[35m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\e[0m\n");
#define TEST_itoaEvenMIN(n)                                            \
	x = (n);   binstr(x,bufA);   itoaEvenMIN(x,bufito);                \
	printf("Ln%3d:   itoaEvenMIN(%10s  %6hd  %s)   ===>   \"%s\"\n",   \
	       __LINE__, #n, x, bufA, bufito)
	TEST_itoaEvenMIN(19);
	TEST_itoaEvenMIN(-19);
	TEST_itoaEvenMIN(SHRT_MAX);
	TEST_itoaEvenMIN(SHRT_MIN);
	TEST_itoaEvenMIN(SHRT_MIN+1);
	TEST_itoaEvenMIN(-SHRT_MAX);
	TEST_itoaEvenMIN(-SHRT_MIN);

	printf("\e[35m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\e[0m\n");
#define TEST_itob(n,base)                                          \
	itob((n), bufito, base);                                       \
	printf("Ln%3d:   itob(%9s, s, base-%-2d)   ===>   \"%s\"\n",   \
	       __LINE__, #n, base, bufito)
	TEST_itob(37,10);
	TEST_itob(-37,10);
	TEST_itob(37,16);
	TEST_itob(37,8);
	TEST_itob(37,2);
	TEST_itob(37,16);
	TEST_itob(37,36); /*Prints 2 ones; good. */
	TEST_itob(175,16);
	TEST_itob(175,32);
	TEST_itob(360,36);
	TEST_itob(INT_MAX,36);
	TEST_itob(INT_MIN,36);
	TEST_itob(INT_MAX,2); /*Prints 31 ones; good. */
	TEST_itob(INT_MAX,10);
	TEST_itob(INT_MIN,10);
	TEST_itob(-SHRT_MIN,10);

	printf("\e[35m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\e[0m\n");
#define TEST_itoaPadding(n,minFieldWidth)                                \
	memset(bufito, '\0', sizeof bufito);                                 \
	itoaPadding((n), bufito, minFieldWidth);                             \
	printf("Ln%3d:   itoaPadding(%9s, s, %2d-wide)   ===>   \"%s\"\n",   \
	       __LINE__, #n, minFieldWidth, bufito)
	TEST_itoaPadding(SHRT_MAX   , 5); /* Just enough */
	TEST_itoaPadding(SHRT_MAX   , 9); /* More than enough */
	TEST_itoaPadding(SHRT_MAX   , 3); /* Not enough */
	TEST_itoaPadding(       1   ,20);
	TEST_itoaPadding(       0   ,20);
	TEST_itoaPadding(       0   , 1);
	TEST_itoaPadding( INT_MIN   ,20);
	TEST_itoaPadding( INT_MIN   , 1);
}
