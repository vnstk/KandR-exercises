/* Vainstein K 2025mar20 */
#include <limits.h>  /* Limits of integral types */
#include <float.h>   /* Limits of floating-pt types */
#include <stdio.h>
#include <string.h>

/* NB: Since coding this in MSYS2, and MSYS2 creates _Win_ binaries, sizeof long == sizeof int.  So,
	   using "long long" instead of "long".
*/

main ()
{
	printf("Per hdr, ULLONG_MAX =  %20llu\n", (unsigned long long)ULLONG_MAX);
	printf("Compute, ULLONG_MAX =  %20llu\n", (unsigned long long)(~ 0ULL));
	/* Could have, alternatively,
	printf("Compute, ULLONG_MAX =  %20llu\n", (unsigned long long)(unsigned long long)-1LL);
	*/

	printf("Per hdr,   UINT_MAX =  %20llu\n", (unsigned long long)UINT_MAX);
	printf("Compute,   UINT_MAX =  %20llu\n", (unsigned long long)(~ 0U));

	/* No way to explicitly create a _short_ literal, so need a temp local. */
{	printf("Per hdr,  USHRT_MAX =  %20llu\n", (unsigned long long)USHRT_MAX);
	unsigned short x = 0;
	printf("Compute,  USHRT_MAX =  %20llu\n", (unsigned long long)((unsigned short)(~ x)));
}

	/* No way to explicitly create a _char_ literal, so need a temp local. */
{	printf("Per hdr,  UCHAR_MAX =  %20llu\n", (unsigned long long)UCHAR_MAX);
	unsigned char x = 0;
	printf("Compute,  UCHAR_MAX =  %20llu\n", (unsigned long long)((unsigned char)(~ x)));
}

	/* Max signed would, obviously, have 0 in sign bit and 1 in all other
	   bits. Since x86_64 big-endian, sign bit will be in the Rmost byte. */

{	printf("Per hdr,  LLONG_MAX =  %+20lld\n", (long long)LLONG_MAX);
	long long x;
	char buf[sizeof(long long)];
	memset(buf, 0xFF, sizeof buf - 1);
	buf[sizeof buf - 1] = 0x7F;
	memcpy(&x, buf, sizeof buf);
	printf("Compute,  LLONG_MAX =  %+20lld\n", (long long)x);
	printf("Per hdr,  LLONG_MIN =  %+20lld\n", (long long)LLONG_MIN);
	printf("Compute,  LLONG_MIN =  %+20lld\n", (long long)(x + 1));
}

{	printf("Per hdr,    INT_MAX =  %+20lld\n", (long long)INT_MAX);
	int x;
	char buf[sizeof(int)];
	memset(buf, 0xFF, sizeof buf - 1);
	buf[sizeof buf - 1] = 0x7F;
	memcpy(&x, buf, sizeof buf);
	printf("Compute,    INT_MAX =  %+20lld\n", (long long)x);
	printf("Per hdr,    INT_MIN =  %+20lld\n", (long long)INT_MIN);
	printf("Compute,    INT_MIN =  %+20lld\n", (long long)(x + 1));
}

{	printf("Per hdr,   SHRT_MAX =  %+20lld\n", (long long)SHRT_MAX);
	short x;
	char buf[sizeof(short)];
	memset(buf, 0xFF, sizeof buf - 1);
	buf[sizeof buf - 1] = 0x7F;
	memcpy(&x, buf, sizeof buf);
	printf("Compute,   SHRT_MAX =  %+20lld\n", (long long)x);
	printf("Per hdr,   SHRT_MIN =  %+20lld\n", (long long)SHRT_MIN);
	printf("Compute,   SHRT_MIN =  %+20lld\n", (long long)(x + 1));
}

{	printf("Per hdr,   CHAR_MAX =  %+20lld\n", (long long)CHAR_MAX);
	char x;
	char buf[sizeof(char)];
	memset(buf, 0xFF, sizeof buf - 1);
	buf[sizeof buf - 1] = 0x7F;
	memcpy(&x, buf, sizeof buf);
	printf("Compute,   CHAR_MAX =  %+20lld\n", (long long)x);
	printf("Per hdr,   CHAR_MIN =  %+20lld\n", (long long)CHAR_MIN);
	printf("Compute,   CHAR_MIN =  %+20lld\n", (long long)(x + 1));
}

{	printf("Per hdr,    FLT_MAX =  %20.3e\n", FLT_MAX);
	/* C99 has facilities to tell me when I've reached infinity (i.e. exceeded range of a FP type); but C89 doesn't. */

	/* Alternative A: works, but only for double. */
/*
	double f = 12345678.9;
	for (;;) {
		errno = 0;
		if (((double)pow(f, 2.0) == HUGE_VAL) || (errno == ERANGE) || (errno == EDOM)) {
			break;
		}
		f = pow(f, 2.0);
	}
*/

	/* Alternative B: works: calculates FLT_MAX = 3.000E38, for example.  (3.403E38 is the
	   correct answer.)  Easily extended to give a more precise answer, if desired. */
	float finf = 1.0F / 0.0F;
	float f = 1.0F;
	int i;
	for (;;) {
		float g = f * 10.0F;
		if (g == finf) { /*Whoah, back up! */
			break;
		}
		f = g;
	}
	for (i = 1; i < 10; ++i) {
		float g = f * (float)i;
		if (g == finf) { /*Back up...*/
			f = f * (float)(i-1);
			break;
		}
	}
	printf("Compute,    FLT_MAX =  %20.3e roughly.\n", f);
}

}
