/* Vainstein K 2025mar19 */
#ifndef DBG
#	define DBG 0
#endif
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#define BUFLEN 1024

/* Notes:
	a.	\"   is not a string literal delimiter; but   \\"  is.
	b.	'"'  is not a string literal delimiter; but   "' and  '"  could be (the " symbols).
	c.	comment inside a string literal doesn't count as a comment, for us.
	d.	string literal inside a comment doesn't count as a string literal, for us.
	e.	comment can span multiple lines.
	f.	string literal cannot span multiple lines, _unless_ a line ends with a line continuation char (i.e. backslash).
	g.	double-backslash is not a line continuation.
	h.	Shall not handle elision due to preprocessor directives, like #if 0 ... #endif.
	i.	Shall not handle strings or comments composed with aid of macros, like
#define FOO "aaa
#define BAR /
	j.	Shall not handle strngification due to the   ##    preprocessor facility.
	k.	Shall not handle illegal C code.
	l.	What about   backslash-fwdslash-asterisk  ?
*/

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

/*NB: Unlike getline() funcs in previous exercises' solutions,
       this one does not remove EOL from the read-in line. */
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

main (int argc, char *argv[])
{
	bool insideComment=false, insideStrLiteral=false, insideCharLiteral=false;
	char lineInp[BUFLEN], lineOutp[BUFLEN]; /* We must buffer output, instead
		of just printing it to stdout right away, because of the case of starting
		a comment: when we see *, we've already printed the / directly preceding! */
	while (getline(lineInp, sizeof lineInp)) {
		const int origLen = strlen(lineInp);
		char chPrev='\0'         , chCurr;
		bool chPrev_escaped=false, chCurr_escaped=false;
		int iInp=0, iOutp=0;
		memset(lineOutp, '\0', sizeof lineOutp);
		DBG && fprintf(stderr,"\n\e[36;3morigLen=%d inComnt=%c inStrLit=%c inCharLit=%c\e[0m   \e[33m%s\e[0m",origLen,insideComment?'Y':'N',insideStrLiteral?'Y':'N',insideCharLiteral?'Y':'N' ,lineInp);
		for (; iInp < origLen; ++iInp) {
			chCurr = lineInp[iInp];
			chCurr_escaped = (chPrev == '\\' && ! chPrev_escaped);

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
						DBG && fprintf(stderr,"\t\e[35mNow insideStrLiteral=%c.  iInp=%d\e[0m\n",insideStrLiteral?'Y':'N',iInp);
					}
				} else {
					/* check for beginning and end of char lit */
					if (chCurr == '\'' && (chPrev != '\\' || (chPrev == '\\' && chPrev_escaped))) {
						insideCharLiteral = ! insideCharLiteral;
						DBG && fprintf(stderr,"\t\e[35mNow insideChaLiteral=%c.  iInp=%d\e[0m\n",insideCharLiteral?'Y':'N',iInp);
					}
					/* check for beginning of str lit */
					if (chCurr == '"' && ! insideCharLiteral && (chPrev != '\\' || ! chPrev_escaped)) {
						insideStrLiteral = true;
						DBG && fprintf(stderr,"\t\e[35mNow insideStrLiteral=%c.  iInp=%d\e[0m\n",insideStrLiteral?'Y':'N',iInp);
					}
					/* check for beginning of comment */
					if (chCurr == '*' && chPrev == '/') {
						insideComment = true;
						lineOutp[iOutp-1] = '\0';
						--iOutp;
					}
				}
				if (! insideComment) {
					lineOutp[iOutp++] = chCurr;
				}
			}

			chPrev         = chCurr;
			chPrev_escaped = chCurr_escaped;
		}
		fputs(lineOutp, stdout);
	}
}
