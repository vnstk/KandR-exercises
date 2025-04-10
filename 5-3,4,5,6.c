/* Vainstein K 2025apr06 */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h> /* For SHRT_MAX, SHRT_MIN */
#include <math.h> /* For abs() */

#define BUFSIZE 16

char *strcatHandcraft (char *s /*appendTo*/, const char *ct) /* Ex 5-3 */
{
	char *const original_s = s;
	while (*s) /* Advance to end */
		s++;
	while (*s++ = *ct++)
		;
	return original_s;
}

/* Rets 1 if t occurs at end of s; else 0. */
int strend (const char *s /*compareToEndOf*/, const char *t) /* Ex 5-4 */
{
	char c;
	const size_t sLen = strlen(s), tLen = strlen(t);
	if (tLen > sLen) return 0;
	s += sLen - tLen;
	while ((c = *s++) == *t++)
		if (! c) /* Reached end? */
			return 1;
	return 0;
}

/*
copy at most n of ct to s; pad with NULs iff ct has fewer than n characters.
*/
char *strncpyHandcraft (char *s /*copyTo*/, const char *ct, const int nMax) /* Ex 5-5 */
{
	char ch, *const original_s = s;
	while (((s - original_s) < nMax) && (ch = *ct++))
		*s++ = ch;
	while ((s - original_s) < nMax)
		*s++ = '\0';
	return original_s;
}
/*
concatenate at most n characters of string ct to string s, terminate s with '\0'.
*/
char *strncatHandcraft (char *s /*appendTo*/, const char *ct, int nMax) /* Ex 5-5 */
{
	char ch, *const original_s = s;
	const size_t original_len = strlen(s);
	s += original_len; /* Advance to end; *s would now be '\0'. */
	while (((s - original_s) < nMax) && (ch = *ct++))
		*s++ = ch;
	/* Finally, append NUL. */
	*s = '\0';
	return original_s;
}
/*
compare at most n chars of cs to ct; ret <0 if cs<ct, 0 if cs==ct, >0 if cs>ct.
*/
int strncmpHandcraft (const char *cs, const char *ct, int nMax) /* Ex 5-5 */
{
	int nCompared = 0;
	while (nCompared++ < nMax) {
		const char hs = *cs++, ht = *ct++;
		if (! hs && ! ht) /* Both ended at same time? */
			break;
		else if (! hs && ht) /* cs shorter than ct? */
			return -256; /* Since abs(hs - ht) cannot exceed 255, this retval uniquely
			                identifies the present code location; aids debugging. */
		else if (hs && ! ht) /* ct shorter than cs? */
			return  256; /* Again, 256 > abs(hs-ht) */
		else if (hs == ht) /* Chars same? */
			continue;
		else /* If here, neither char is NUL, and the chars differ */
			return hs - ht;
	}
	return 0; /* Mayn't compare farther; respective chars have been equal so far. */
}



/* return Lmost pos of t in s; -1 if not found */
int strindexNoArr (const char *s, const char *const t) /* Ex 5-6 */
{
	const char *si, *tj, *const original_s = s;
	char hs, ht;
	while (*s) {
		si = s;
		tj = t;
		while ((ht = *tj++) && (hs = *si++))
			if (hs != ht)
				goto move_along_s;
		if (! ht) /* made it to end of t */
			return s - original_s;
move_along_s:
		s++;
	}
	return -1;
}


int strlenNoArr (const char *s)
{
	const char *original_s = s;
	while (*s++)
		;
	return s - original_s - 1;
}

void reverseNoArr (char *s)
{
    const int slen = strlenNoArr(s);
	char *b = s + slen - 1;
	while (s < b) {
		const char tmpch = *s; *s++ = *b; *b-- = tmpch;
	}
}

void itoaNoArr (int n, char *s) /* Based on itoaEvenMIN() in 3-4,5,6.c */
{
	char *original_s = s;
    const bool isNeg = n < 0;
    do {
		*s++ = abs(n % 10) + '0';
    } while (n /= 10);
    if (isNeg)
		*s++ = '-';
    *s = '\0';
    reverseNoArr(original_s);
}
/* Next 3 lines are in aid of TEST_itoaNoArr */
typedef signed short ourSigned_t; /* The "int" we'll be using as study aid. */
#define WORD_BITS (sizeof(ourSigned_t) * 8)
#define BINSTR_BUFLEN (WORD_BITS  +  ((WORD_BITS/4)-1)  +  1)

const char *binstrNoArr (const ourSigned_t z, char *buf)
{
	const char *original_buf = buf;
    int jBit, val;
    for (jBit = 0; jBit < WORD_BITS; ++jBit) {
        if (jBit && ! (jBit % 4))
			*buf++ = '\''; /* nybble separator mark */
        val = z & (0x1 << (WORD_BITS - 1 - jBit));
		*buf++ = jBit ? (val ? '1' : '0') : (val ? '-' : '+');
    }
	*buf = '\0';
    return original_buf;
}


const char *formatForDisplay (const char *buf)
{
	int i;
	static char display[BUFSIZE+1];
	memset(display, '\0', sizeof display);
	for (i = 0; i < BUFSIZE; ++i)
		display[i] = buf[i] ? buf[i] : '!';
	return display;
}

main ()
{
	char buf[BUFSIZE];

#define TEST_strcatHandcraft(appendTo,src)   \
	strcpy(buf, (appendTo));   \
	printf("Ln%3d:   strcatHandcraft(appendTo \"%s\" , \"%s\")   ===>   \"%s\"\n",   \
		__LINE__, (appendTo), (src), strcatHandcraft(buf,(src)))
	TEST_strcatHandcraft("abc", "yz");
	TEST_strcatHandcraft("", "abc");
	TEST_strcatHandcraft("", "");
	TEST_strcatHandcraft("abc", "abc");
	TEST_strcatHandcraft("abc", "cab");
	TEST_strcatHandcraft("abc", "");
	TEST_strcatHandcraft("abc", "flarpeggio");

	printf("\e[35m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\e[0m\n");
#define TEST_strend(s,t)   \
	strcpy(buf, (s));      \
	printf("Ln%3d:   strend(cmpToEndOf \"%s\" , \"%s\")   ===>   %d\n",   \
		__LINE__, (s), (t), strend((s),(t)))
	TEST_strend("", "");
	TEST_strend("", "abc");
	TEST_strend("abc", "");
	TEST_strend("abc", "abc");
	TEST_strend("abc", "bc");
	TEST_strend("abcdef", "f");
	TEST_strend("abcdef", "a");
	TEST_strend("abcdef", "e");
	TEST_strend("abcdef", "cde");
	TEST_strend("z", "z");

	printf("\e[35m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\e[0m\n");
#define TEST_strncpyHandcraft(src,nMax)   \
	memset(buf, 'X', sizeof buf);   \
	(void) strncpyHandcraft(buf, src, nMax);   \
	printf("Ln%3d:   strncpyHandcraft(copyTo, src \"%s\" , %d)   ===>   [%s]\n",   \
		__LINE__, (src), (nMax), formatForDisplay(buf))
	TEST_strncpyHandcraft("", 0);
	TEST_strncpyHandcraft("", 1);
	TEST_strncpyHandcraft("", 2);
	TEST_strncpyHandcraft("", BUFSIZE);
	TEST_strncpyHandcraft("abcdef", 0);
	TEST_strncpyHandcraft("abcdef", 1);
	TEST_strncpyHandcraft("abcdef", 2);
	TEST_strncpyHandcraft("abcdef", 9);
	TEST_strncpyHandcraft("abcdef", BUFSIZE);
	TEST_strncpyHandcraft("abcdefghijkmnop", BUFSIZE);  /* len("abcdefghijkmnop")=15 = BUFSIZE-1 */
	TEST_strncpyHandcraft("abcdefghijkmnopq", BUFSIZE); /* len("abcdefghijkmnopq")=16 = BUFSIZE */

	printf("\e[35m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\e[0m\n");
#define TEST_strncatHandcraft(appendTo,src,nMax)   \
	memset(buf, 'X', sizeof buf);   \
	(void) strcpy(buf, (appendTo));  \
	(void) strncatHandcraft(buf, (src), (nMax));   \
	printf("Ln%3d:   strncpyHandcraft(appendTo \"%s\", src \"%s\" , %d)   ===>   [%s]\n",   \
		__LINE__, (appendTo), (src), (nMax), formatForDisplay(buf))
	TEST_strncatHandcraft("abcde", "fgh", 0);
	TEST_strncatHandcraft("abcde", "fgh", 2);
	TEST_strncatHandcraft("abcde", "fgh", 3);
	TEST_strncatHandcraft("abcde", "fgh", 4);
	TEST_strncatHandcraft("abcde", "fgh", 10);
	TEST_strncatHandcraft("abcde", "", 0);
	TEST_strncatHandcraft("abcde", "", 1);
	TEST_strncatHandcraft("abcde", "", 3);
	TEST_strncatHandcraft("abcde", "", 9);
	TEST_strncatHandcraft("", "fgh", 0);
	TEST_strncatHandcraft("", "fgh", 2);
	TEST_strncatHandcraft("", "fgh", 3);
	TEST_strncatHandcraft("", "fgh", 7);

	printf("\e[35m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\e[0m\n");
	int answ;
#define TEST_strncmpHandcraft(cs,ct,nMax,wanted)   \
	answ = strncmpHandcraft(cs,ct,(nMax));   \
	printf("Ln%3d:   strncmpHandcraft(\"%s\", \"%s\", %d)   ===>   %d; wanted \e[1m%s\e[0m\n",   \
		__LINE__, (cs), (ct), (nMax), answ, wanted)
	TEST_strncmpHandcraft("zdef", "zfed", 0,  "0");
	TEST_strncmpHandcraft("zdef", "zfed", 1,  "0");
	TEST_strncmpHandcraft("zdef", "zfed", 2, "< 0");
	TEST_strncmpHandcraft("y", "flarp",   1,  "> 0");
	TEST_strncmpHandcraft("y", "flarp",   5,  "> 0");
	TEST_strncmpHandcraft("y", "flarp",   9,  "> 0");
	TEST_strncmpHandcraft("", "aardvark",12, "< 0");
	TEST_strncmpHandcraft("flarp", "",    1,  "> 0");
	TEST_strncmpHandcraft("", "",    0,  "0");
	TEST_strncmpHandcraft("", "",    1,  "0");
	TEST_strncmpHandcraft("", "",    7,  "0");

	printf("\e[35m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\e[0m\n");
#define TEST_strindexNoArr(s,t)      \
	printf("Ln%3d:   strindexNoArr(|%2zu|\"%s\" , \"%s\"|%zu|)   ===>   %d\n",   \
	       __LINE__, strlen(s), s, t, strlen(t), strindexNoArr(s,t))
	TEST_strindexNoArr("abcdefghij", "abcdefghij");
	TEST_strindexNoArr("abcdefghij", "abc");
	TEST_strindexNoArr("abcdefghij", "efg");
	TEST_strindexNoArr("abcdefghij", "gfe");
	TEST_strindexNoArr("abcdefghij", "hij");
	TEST_strindexNoArr("abcdefghij", "a");
	TEST_strindexNoArr("abcdefghij", "e");
	TEST_strindexNoArr("abcdefghij", "j");
	TEST_strindexNoArr("abcdefghij", "xyz");
	TEST_strindexNoArr("abcdefghij", "");
	TEST_strindexNoArr("", "");
	TEST_strindexNoArr("", "xyz");

    char bufA[BINSTR_BUFLEN], bufito[66];
    short x;
    printf("\e[35m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\e[0m\n");
#define TEST_itoaNoArr(n)                                            \
    x = (n);   binstrNoArr(x,bufA);   itoaNoArr(x,bufito);                \
    printf("Ln%3d:   itoaNoArr(%10s  %6hd  %s)   ===>   \"%s\"\n",   \
           __LINE__, #n, x, bufA, bufito)
    TEST_itoaNoArr(19);
    TEST_itoaNoArr(-19);
    TEST_itoaNoArr(SHRT_MAX);
    TEST_itoaNoArr(SHRT_MIN);
    TEST_itoaNoArr(SHRT_MIN+1);
    TEST_itoaNoArr(-SHRT_MAX);
    TEST_itoaNoArr(-SHRT_MIN);
}
