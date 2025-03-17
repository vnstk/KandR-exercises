/* Vainstein K 2025mar17 */
/* Invoke as e.g.
					./a.exe < 1-7.c
*/
#include <stdio.h>
main () {
	unsigned nSP = 0U, nHT = 0U, nLF = 0U;
	char ch;
	while ((ch = getchar()) != EOF) {
		nSP += ch == ' ';
		nHT += ch == '\t';
		nLF += ch == '\n';
	}
	printf("nSP=%u nHT=%u nLF=%u\n",nSP,nHT,nLF);
}	/* nLF easily checked with `wc -l` */
