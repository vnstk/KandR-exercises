/* Vainstein K 2025mar20 */
/*Test steps:
	bash 1-24_runTests.sh
*/
#ifndef DBG
#  define DBG 0
#endif
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFLEN 1024
#define MAX_STACK_DEPTH 512

/* Notes:
	a.-j.	Largely an elaboration of 1-23.c; see there for comments which apply here as well.
	k.	For validation, run
	                                  bash 1-24_runTests.sh
	l.	For addit'l negative validation, just run with this very source file as input.
*/

enum ePairableDelimiter {
	PAREN = 1,
	BRACKET = 2,
	BRACE = 3,
};

bool isPairableDelimiter (const char ch, enum ePairableDelimiter *which, bool *isOpening) {
	if (ch == '(') { *which=PAREN;   *isOpening=true;  } else
	if (ch == '[') { *which=BRACKET; *isOpening=true;  } else
	if (ch == '{') { *which=BRACE;   *isOpening=true;  } else
	if (ch == ')') { *which=PAREN;   *isOpening=false; } else
	if (ch == ']') { *which=BRACKET; *isOpening=false; } else
	if (ch == '}') { *which=BRACE;   *isOpening=false; } else
		return false;
	return true;
}

float validationAid (double) {
	int i = /*xx\0x*/ 42;
	int j =  42/*multiline, or "manylined"

	, comment.*/ ;
	int k = \
		     42;
	const char ch2 = ", I presume?"['\''];
	bool b0 = true;
	bool b1 = (ch2 == '\\' && ! b0);
	const char *s0 = "No /*comment*/ */* at this time\n\0\0!!!";
	const char *s1 = "aaa \" bbb /* ccc */ ddd \\";
	const char *s2 = "\"eee"
"fff"           	 "ggg"
	"hhh\
hh";
	const char *s3 = "\"""'quatrains"/*eee*/"Z";
	const char *s4 = "Foo\\\
ba\"\""
"rr"/*flarp"qux*"*/;
	/* Sure they /*don't nest?? */
	const char *s5 = "Fnord \\   qux*/" ;
	char ch3 = '"'; /*Omar*/
/* Indeed,\
indeed,* /repentance\0 oft
"
before I swore --- but was I / * " sober\\ when
I \\
swore?""*"/"""*/
	char ch4 = '[\
';
	char ch5 = '\
"';/*Clever enough to delete this comment????*/ char ch6 = '"\
';
	const char *s6 = "quatrains'"; /*Khayyam*/
}

bool getline (char *const buf, const int buflen)
{
	memset(buf, '\0', buflen);
	int c=EOF, i;
	for (i=0; (c=getchar())!=EOF; ++i) {
		if (i >= buflen-2) {
			fprintf(stderr, "Buffer too small.\n");
			exit(1);
		}
		buf[i] = c;
		if (c == '\n')
			break;
	}
	return c != EOF;
}

main (int argc, char *argv[])
{
	char position[32];
	bool insideComment=false, insideStrLiteral=false, insideCharLiteral=false;
	char lineInp[BUFLEN];
	int iStack = 0, iLine = 0;
	enum ePairableDelimiter pdStack[MAX_STACK_DEPTH];
	memset(pdStack, '\0', sizeof pdStack); /* NB: 0 is not one of ePairableDelimiter's legal values */

	while (getline(lineInp, sizeof lineInp)) {
		const int origLen = strlen(lineInp);
		char chPrev='\0'         , chCurr;
		bool chPrev_escaped=false, chCurr_escaped=false;
		int iInp=0;
		for (; iInp < origLen; ++iInp) {
			chCurr = lineInp[iInp];
			chCurr_escaped = (chPrev == '\\' && ! chPrev_escaped);
			sprintf(position, "<Ln%d:Ch%d>", iLine+1, iInp+1);

			if (insideComment) {
 				/* check for end of comment */
				if (chCurr == '/' && chPrev == '*') {
					insideComment = false;
				}
			} else {
				if (insideStrLiteral) {
 					/* check for end of str lit */
					if (chCurr == '"' && (chPrev != '\\' || (chPrev == '\\' && chPrev_escaped))) {
						insideStrLiteral = false;
					}
				} else {
					/* check for beginning and end of char lit */
					if (chCurr == '\'' && (chPrev != '\\' || (chPrev == '\\' && chPrev_escaped))) {
						insideCharLiteral = ! insideCharLiteral;
					}
					/* check for beginning of str lit */
					if (chCurr == '"' && ! insideCharLiteral && (chPrev != '\\' || ! chPrev_escaped)) {
						insideStrLiteral = true;
					}
					/* check for beginning of comment */
					if (chCurr == '*' && chPrev == '/') {
						insideComment = true;
					}
				}

				if (! insideStrLiteral && ! insideComment && ! insideCharLiteral) {
					enum ePairableDelimiter which;
					bool isOpening;
					if (isPairableDelimiter(chCurr,&which,&isOpening)) {
						if (isOpening) {
							DBG && fprintf(stderr, "\e[33m Storg opening '%c' (e%d) in pdStack[%d]  @ %s\e[0m\n",chCurr,which,iStack,position);
							pdStack[iStack++] = which;
							if (iStack == MAX_STACK_DEPTH) {
								fprintf(stderr, "Stack too small.\n");
								exit(2);
							}
						} else {
							if (iStack == 0) {
								fprintf(stderr, "Stray '%c' at %s.\n", chCurr,position);
								exit(3);
							}
							--iStack;
							if (pdStack[iStack] != which) {
								fprintf(stderr, "Unbalanced '%c' at %s.\n", chCurr,position);
								exit(4);
							}
							DBG && fprintf(stderr, "\e[35m Clearg stored '%c' (e%d) of pdStack[%d]  @ %s\e[0m\n",chCurr,which,iStack,position);
							pdStack[iStack] = 0; /* Not strictly necessary */
						}
					}
				}
			}

			chPrev         = chCurr;
			chPrev_escaped = chCurr_escaped;
		}

		++iLine;
	}

	if (insideComment) {
		fprintf(stderr, "Input ends inside unterminated comment.\n");
		exit(5);
	} else if (insideStrLiteral) {
		fprintf(stderr, "Input ends inside unterminated string literal.\n");
		exit(6);
	} else if (insideCharLiteral) {
		fprintf(stderr, "Input ends inside unterminated char literal.\n");
		exit(7);
	} else if (iStack) {
		fprintf(stderr, "Input ends with %d unbalanced (|[|{|)|]|} delimiter%s.\n", iStack, iStack > 1 ? "s" : "");
		exit(8);
	}
	return 0;
}
