/* Vainstein K 2025mar17 */
/* Shall simplify task, thus:
	a.	Recorded wordlen will top out at 100; a 137-char word will count as a 100-char one.
	b.	Each histogram will have (space for) 20 bars.  Thus, each bar groups 5 wordlen values.
	c.	Each histogram will, regardless of LINES and COLUMNS, use 40 lines and 80 columns.  (This excludes labels.)
	d.	Each bucket will be labeled with its lower bound.
*/
#include <stdio.h>
#include <string.h>

static const int WORDLEN_CEIL = 100;
static const int N_BUCKETS = 20;
static const int BUCKET_SPAN = WORDLEN_CEIL / N_BUCKETS;

main () {
	char ch;
	int currWordlen = 0;
	int i, j, k;
	int maxCountInSingleBucket;
	int max_nPixels; /* Not real pixels, but rather "*" symbols. */
	float onePixelRepresents;
	int pixelsThisBar;
	int bucketedCounts[N_BUCKETS];
	memset(bucketedCounts, '\0', sizeof bucketedCounts);

	do {
		switch ((ch = getchar())) {
			case EOF: /* Don't assume that input ends with a newline. */
			case ' ':
			case '\t':
			case '\n':
				if (currWordlen) {
					const int adjustedLen = (currWordlen > WORDLEN_CEIL) ? WORDLEN_CEIL : currWordlen;
					++bucketedCounts[adjustedLen / BUCKET_SPAN];
					currWordlen = 0;
				}
				break;
			default:
				++currWordlen;
		}
	} while (ch != EOF);

	maxCountInSingleBucket = 0;
	for (i = 0; i < N_BUCKETS; ++i) {
		if (bucketedCounts[i] > maxCountInSingleBucket)
			maxCountInSingleBucket = bucketedCounts[i];
	}


	/**** Print a horizontal-bars histogram. ****/
	max_nPixels = 80;
	onePixelRepresents = (float)maxCountInSingleBucket / (float)max_nPixels;

	for (i = 0; i < N_BUCKETS; ++i) {
		printf("%2d |", i * BUCKET_SPAN);
		pixelsThisBar = (int)((float)bucketedCounts[i] / onePixelRepresents);
		for (j = 0; j < pixelsThisBar; ++j)
			printf("*");
		printf("\n");
	}


	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

	/**** Print a vertical-bars histogram. ****/
	max_nPixels = 40;
	onePixelRepresents = (float)maxCountInSingleBucket / (float)max_nPixels;

	for (k = max_nPixels - 1; k >= 0; --k) {
		for (i = 0; i < N_BUCKETS; ++i) {
			pixelsThisBar = (int)((float)bucketedCounts[i] / onePixelRepresents);
			printf("  %c", (k < pixelsThisBar) ? '*' : ' ');
		}
		printf("\n");
	}
	for (i = 0; i < N_BUCKETS; ++i)
		printf("---");
	printf("\n");
	for (i = 0; i < N_BUCKETS; ++i)
		printf("%2d ", i * BUCKET_SPAN);
	printf("\n");
}
