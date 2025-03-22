/* Vainstein K 2025mar21 */
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#define BUFLEN 2048

/* Decided to treat the "-" range operator as L-associative.
*/
void expand (const char *const s1, char *s2)
{
	const int s1Len = strlen(s1);
	char chCurr, chPrev='\0', fromBound='\0', untoBound='\0';
	int i=0, iRmost_fromBound=-1, iRmost_untoBound=0;
	while ((chCurr = s1[i])) {
		const bool isFirst = i == 0, isLast = i == s1Len-1;
		if (chCurr == '-') {
			if (! isFirst && ! isLast) {
				if (fromBound && iRmost_fromBound == i-1) {
					goto doneWith_chCurr; /* We might have a range */
				}
			}
		} else {
			assert(isalnum(chCurr));
			if (fromBound) {
				if (chPrev == '-' && iRmost_fromBound == i-2) { /* Yes, have a range. */
					untoBound = chCurr;
					iRmost_untoBound = i;
					if ((fromBound <= untoBound) && ( /* But is it a valid range? */
					    (isdigit(fromBound) && isdigit(untoBound)) ||
					    (islower(fromBound) == islower(untoBound))))
					{
						char ch;
						for (ch = fromBound; ch <= untoBound; ++ch)
							*s2++ = ch;
					} else { /* Range invalid, so just treat it as literals. */
						*s2++ = fromBound;
						*s2++ = '-';
						*s2++ = untoBound;
					}
					/* Valid or not, we're done with this range */
					fromBound = untoBound = '\0';
					goto doneWith_chCurr;
				}
			} else {
				fromBound = chCurr;
				iRmost_fromBound = i;
				goto doneWith_chCurr;
			}
		}
		if (fromBound) {
			*s2++ = fromBound;
			fromBound = '\0';
		}
		if (chPrev == '-') {
			*s2++ = '-';
		}
		*s2++ = chCurr;
doneWith_chCurr:
		chPrev = chCurr;
		++i;
	}
	if (fromBound) {
		*s2++ = fromBound;
	}
}

main ()
{
	char answ[BUFLEN];
#define TEST(s)                         \
	memset(answ, '\0', sizeof answ);    \
	expand(s,answ);                     \
	printf("Ln%3d:   expand(\"%s\")  ===>  \"%s\"\n", __LINE__, s, answ)

	TEST("a-fA-F0-9");
	TEST("p");
	TEST("p-p");
	TEST("p-v");
	TEST("p-q");
	TEST("v-p");
	TEST("3-7v-p");
	TEST("v-p3-7");
	TEST("3-p-v-7");
	TEST("p-V3-7");
	TEST("P-v3-7");
	TEST("3-7v-pF-H");
	TEST("p--v");
	TEST("p-");
	TEST("-p");
	TEST("-");
/*				The next 6 cases are seriously pathological.  Alternatives:
				[a]		Go the lex&yacc route.
				[b]		Add special logic to deal with multiple consecutive dashes.
				[c]		Move on to the next exercise.
	TEST("--");
	TEST("---");
	TEST("----");
	TEST("--p");
	TEST("---p");
	TEST("p---");
*/
	TEST("p--");
	TEST("-p-");
	TEST("a-b-c");
	TEST("a-d-h");
	TEST("a-z0-9");
	TEST("-a-z");
	TEST("a-z-");
}
