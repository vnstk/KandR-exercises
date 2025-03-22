/* Vainstein K 2025mar19 */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#define BUFLEN 1024

bool getline (char *const buf, const int buflen)
{
	memset(buf, '\0', buflen);
	int c=EOF, i;
	for (i=0; (c=getchar())!=EOF; ++i) {
		if (i >= buflen-2) {
			fprintf(stderr, "Buffer too small.\n");
			return false;
		}
		buf[i] = c;
		if (c == '\n')
			break;
	}
	return c != EOF;
}

/* conv char to graphable escape seq */
void escape (char *const bufOutp, int *const iOutp, const char chCurr)
{
	switch (chCurr) {
		case '\0':
			bufOutp[(*iOutp)++] = '\\';   bufOutp[(*iOutp)++] = '0';   return;
		case '\b':
			bufOutp[(*iOutp)++] = '\\';   bufOutp[(*iOutp)++] = 'b';   return;
		case '\n':
			bufOutp[(*iOutp)++] = '\\';   bufOutp[(*iOutp)++] = 'n';   return;
		case '\t':
			bufOutp[(*iOutp)++] = '\\';   bufOutp[(*iOutp)++] = 't';   return;
		default:
			bufOutp[(*iOutp)++] = chCurr;                              return;
	}
}

/* conv escape seq to char */
void unescape (char *const bufOutp, int *const iOutp, const char chCurr,
                                               const bool chCurr_escaped)
{
	if (chCurr_escaped) {
		switch (chCurr) {
			case '0':
				 /*Note that we back up, and overwrite the "\" stored just prior. */
				bufOutp[--*iOutp] = '\0';   ++*iOutp;   return;
			case 'b':
				bufOutp[--*iOutp] = '\b';   ++*iOutp;   return;
			case 'n':
				bufOutp[--*iOutp] = '\n';   ++*iOutp;   return;
			case 't':
				bufOutp[--*iOutp] = '\t';   ++*iOutp;   return;
		}
	}
	bufOutp[(*iOutp)++] = chCurr;
}

main (int argc, char *argv[])
{
	bool insideComment=false, insideStrLiteral=false, insideCharLiteral=false;
	char lineInp[BUFLEN], lineOutp[BUFLEN]; /* We must buffer output, instead
		of just printing it to stdout right away, because of unescape logic. */
	bool modeEscape=false, modeUnescape=false;
	if (argc > 1 && ! strcmp(argv[1],"esc"))
		modeEscape=true;
	else if (argc > 1 && ! strcmp(argv[1],"unesc"))
		modeUnescape=true;
	else {
		fprintf(stderr, "Must give cmdline arg, esc|unesc\n");
		return 1;
	}
	while (getline(lineInp, sizeof lineInp)) {
		const int origLen = strlen(lineInp);
		char chPrev='\0'         , chCurr;
		bool chPrev_escaped=false, chCurr_escaped=false;
		int iInp=0, iOutp=0;
		memset(lineOutp, '\0', sizeof lineOutp);
		for (; iInp < origLen; ++iInp) {
			chCurr = lineInp[iInp];
			chCurr_escaped = (chPrev == '\\' && ! chPrev_escaped);
			if (modeEscape)
				escape(lineOutp, &iOutp, chCurr);
			else
				unescape(lineOutp, &iOutp, chCurr, chCurr_escaped);
			chPrev         = chCurr;
			chPrev_escaped = chCurr_escaped;
		}
		if (modeUnescape && lineOutp[--iOutp] == '\n')
			lineOutp[iOutp] = '\0';
		fputs(lineOutp, stdout);
		if (modeEscape)
			putchar('\n');
	}
}
