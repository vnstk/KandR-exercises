/* Vainstein K 2025mar20 */
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

unsigned htoi (const char* s, bool *const wasError)
{
	unsigned result = 0U;
	int slen, i;
	*wasError = true;
	if (!strncmp(s, "0x", 2) || !strncmp(s, "0X", 2))
		s += 2;
	slen = strlen(s);
	if (! slen) {
		printf("Input '%s' empty\n",s);
		goto done;
	}
	for (i = 1; i <= slen; ++i) {
		const char ch = tolower(s[slen - i]);
		unsigned nybble = 0;
		if (ch >= '0' && ch <= '9') {
			nybble = ch - '0';
		} else if (ch >= 'a' && ch <= 'f') {
			nybble = 10 + (ch - 'a');
		} else {
			printf("Illegal character '%c' in input '%s'\n",ch,s);
			goto done;
		}
		result |= nybble << ((i-1) * 4);
	}
	*wasError = false;
done:
	return result;
}

main ()
{
	const char *inputs[] = {
		"0xFA", /*250*/
		"0xBEEF",
		"Beef",
		"0",
		"x035", /*Illegal char*/
		"035", /*Octal, actually.*/
		"", /*empty*/
		"0X", /*empty*/
		"FFFF", /*USHRT_MAX*/
	};
	int i = 0;
	for (; i < sizeof inputs / sizeof(char*); ++i) {
		bool wasError;
		const unsigned result = htoi(inputs[i], &wasError);
		if (wasError)
			printf("%2d|  '%s'   ERR\n", i, inputs[i]);
		else
			printf("%2d|  '%s'   0x%X or %u\n", i, inputs[i], result, result);
	}
}
