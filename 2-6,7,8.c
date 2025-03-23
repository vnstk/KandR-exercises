/* Vainstein K 2025mar21 */
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* The "word" we'll be using for all these exercises */
typedef unsigned short ourUnsigned_t;

#define WORD_BITS (sizeof(ourUnsigned_t) * 8)


/*	(WORD_BITS / 4) - 1   ---   nybble separator marks
	                  4   ---   "0b" prefix, "U" suffix, NUL terminator
*/
#define BINSTR_BUFLEN (WORD_BITS + ((WORD_BITS / 4) - 1) + 4)

/* Stringify as a C23 binary number literal,
   with separator mark to set nybbles apart. */
const char *binstr (const ourUnsigned_t u, char *const buf)
{
	int iBuf, jBit;
	strcpy(buf, "0b");
	for (iBuf = 2, jBit = 0; jBit < WORD_BITS; ++jBit) {
		if (jBit && ! (jBit % 4))
			buf[iBuf++] = '\'';
		buf[iBuf++] = (u & (0x1 << (WORD_BITS - 1 - jBit))) ? '1' : '0';
	}
	buf[iBuf++] = 'U';
	buf[iBuf] = '\0';
	return buf;
}

/* x with the n bits that begin at position p set to the Rmost n bits of y */
ourUnsigned_t setbits (ourUnsigned_t x, int p, int n, ourUnsigned_t y) /* Ex 2-6 */
{
	assert(p + n <= WORD_BITS);
	const ourUnsigned_t rangeDst = ~(~0 << n) << (WORD_BITS - p - n);
	const ourUnsigned_t rangeSrc = ~(~0 << n);
	return (x & ~rangeDst) | ((y & rangeSrc) << (WORD_BITS - p - n));
}

/* x with the n bits that begin at position p inverted */
ourUnsigned_t invert (ourUnsigned_t x, int p, int n) /* Ex 2-7 */
{
	assert(p + n <= WORD_BITS);
	const ourUnsigned_t rangeToFlip = ~(~0 << n) << (WORD_BITS - p - n);
	const ourUnsigned_t notChanged = x & ~rangeToFlip;
	const ourUnsigned_t yesChanged = (rangeToFlip & x) ^ rangeToFlip;
	return notChanged | yesChanged;
}

ourUnsigned_t rightrot (ourUnsigned_t x, int n) /* Ex 2-8 */
{
	assert(n <= WORD_BITS);
	return ((~(~0 << n) & x) << (WORD_BITS - n))  |   (x >> n);
}

main ()
{
	char bufA[BINSTR_BUFLEN], bufB[BINSTR_BUFLEN], bufC[BINSTR_BUFLEN];
	ourUnsigned_t answ;
	printf("WORD_BITS = %llu\n",WORD_BITS);

	printf("\e[35m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\e[0m\n");
#define TEST_setbits(x,p,n,y)                                             \
	answ = setbits(x,(p),(n),y);                                          \
	printf("Ln%3d:   setbits(%s ,%2d,%2d, %s)   ==>   %s\n", __LINE__,    \
		binstr(x,bufA) ,(int)(p),(int)(n), binstr(y,bufB) , binstr(answ,bufC))
	TEST_setbits(0xf00U,WORD_BITS-7,5,0xffU);
	TEST_setbits(0x0U,WORD_BITS-7,5,0xffU);

	printf("\e[35m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\e[0m\n");
#define TEST_invert(x,p,n)                                           \
	answ = invert(x,(p),(n));                                        \
	printf("Ln%3d:   invert(%s ,%2d,%2d)   ==>   %s\n", __LINE__,    \
		binstr(x,bufA) ,(int)(p),(int)(n), binstr(answ,bufC))
	TEST_invert(0XffU, WORD_BITS  , 0); 
	TEST_invert(0XffU, WORD_BITS-9, 9); 
	TEST_invert(0XffU, WORD_BITS-9, 6); 
	TEST_invert(0XffU, WORD_BITS-9, 0); 
	TEST_invert(0XffU, WORD_BITS-7, 7); 
	TEST_invert(0XffU, WORD_BITS-7, 6); 
	TEST_invert(0XffU, WORD_BITS-7, 0); 
	TEST_invert(0XffU, 0          , 6); 
	TEST_invert(0XffU, 0          , WORD_BITS-7); 
	TEST_invert(0XffU, 0          , WORD_BITS); 

	printf("\e[35m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\e[0m\n");
#define TEST_rightrot(x,n)                                         \
	answ = rightrot(x,(n));                                        \
	printf("Ln%3d:   rightrot(%s ,%2d)   ==>   %s\n", __LINE__,    \
		binstr(x,bufA) ,(int)(n), binstr(answ,bufC))
	TEST_rightrot(0XfffU, WORD_BITS);
	TEST_rightrot(0XfffU, WORD_BITS / 2);
	TEST_rightrot(0XfffU, WORD_BITS-3);
	TEST_rightrot(0XfffU, 3);
	TEST_rightrot(0XfffU, 0);
	TEST_rightrot(0X1fcU, 4);
	TEST_rightrot(0X1fcU, 8);
	TEST_rightrot(0X1fcU, 12);
	TEST_rightrot(0Xf00U, 3);
	TEST_rightrot(0Xf00U, 15);
}
