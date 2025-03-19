/* Vainstein K 2025mar17 */
#include <stdio.h>		
#include <stdbool.h>
#define LEN_THRESH 80

bool getline ()
{
	char buf[LEN_THRESH];
	int c=EOF, i; 
	bool begunEchoing=false;
	for (i=0; (c=getchar())!=EOF && c!='\n'; ++i) {
		if (i < LEN_THRESH-1) { /* save-mode*/
			buf[i] = c;
		} else if (i > LEN_THRESH-1) { /*echo-mode*/
			putchar(c);
		} else { /*inflection point*/
			buf[LEN_THRESH-1] = '\0';
			begunEchoing=true;
			printf("%s", buf);
			putchar(c);
		}
	}
	if (begunEchoing)
		putchar('\n');
	return c != EOF;
}
  
main ()
{
	while (getline())
		;
}
