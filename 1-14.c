/* Vainstein K 2025mar17 */
/* Shall simplify task, thus:
	a.	Assuming ASCII encoding, natch.
	b.	Each "pixel" will represent 5 occurrences.
	c.	Shall track occurrences only of bytes 0x09-0x7E; of those, 0x21-0x7E are isgraph()able.
	d.	For simplicity, shall alloc 128 buckets and simply not use the 0x00-0x08 ones and 0x7F.
*/
#include <stdio.h>
#include <string.h>

static const int N_BUCKETS = 128;
static const char MIN_CH_TRACKED = 0x09;
static const char MAX_CH_TRACKED = 0x7E;
static const char MIN_ISGRAPHABLE = 0x21;
static const int COUNTS_PER_PIXEL = 5;

main () {
	char ch;
	int k;
	int counts[N_BUCKETS];
	memset(counts, '\0', sizeof counts);

	while ((ch = getchar()) != EOF) {
		if (ch >= 0x7F)
			continue;
		++counts[ch];
	}

	for (ch = MIN_CH_TRACKED; ch < MAX_CH_TRACKED; ++ch) {
		if (ch >= MIN_ISGRAPHABLE)
			printf("  %c  ", ch);
		else
			printf("0x%02X ", ch);
		printf("|");
		for (k = 0; k < counts[ch] / COUNTS_PER_PIXEL; ++k)
			printf("*");
		printf("\n");
	}
}
