/* Vainstein K 2025apr22 */
/*Test steps:
	cat -n sampleDocument.txt | sed -n '24,26p'
			...So you know what those lines should be.
	./a.exe sampleDocument.txt ; echo $?
	./a.exe sampleDocument.txt sampleDocument.txt ; echo $?
	sed '25 c xxx' sampleDocument.txt >modif.txt
	./a.exe sampleDocument.txt modif.txt ; echo $?
	./a.exe modif.txt sampleDocument.txt ; echo $?
	rm modif.txt
	sed '25 d' sampleDocument.txt >delet.txt
	./a.exe sampleDocument.txt delet.txt ; echo $?
	./a.exe delet.txt sampleDocument.txt ; echo $?
	rm delet.txt
	sed '25 a yyy' sampleDocument.txt >added.txt
	./a.exe sampleDocument.txt added.txt ; echo $?
	./a.exe added.txt sampleDocument.txt ; echo $?
	rm added.txt
*/
/*Exit code shall be:
	1, if 1st filepath invalid (could not be opened for read).
	2, if 2nd filepath invalid (could not be opened for read).
	3, if two filepaths hadn't been supplied on cmdline.
	4, if the files differ at some point.
	0, if the files do not differ at any point.
*/

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#define LINEBUFLEN 1024

main (int argc, char const *argv[])
{
	int exitCode=5; /*meaning, bug */
	char linebufA[LINEBUFLEN], linebufB[LINEBUFLEN];
	FILE *fhA=NULL, *fhB=NULL;
	
	if (argc!=3 || !strlen(argv[1]) || !strlen(argv[2])) {
		fprintf(stderr, "\e[35;3;1mUSAGE:  <filepathA>  <filepathB>\e[0m\n");
		exitCode=3;
		goto done;
	}
	if (!(fhA = fopen(argv[1],"r"))) {
		fprintf(stderr, "Fail read-open \"%s\": %s\n", argv[1], strerror(errno));
		exitCode=1;
		goto done;
	}
	if (!(fhB = fopen(argv[2],"r"))) {
		fprintf(stderr, "Fail read-open \"%s\": %s\n", argv[2], strerror(errno));
		exitCode=2;
		goto done;
	}
	exitCode=4; /*meaning, some difference in files' contents*/

	for (;;) {
		bool const eofA = ! fgets(linebufA,sizeof linebufA,fhA);
		bool const eofB = ! fgets(linebufB,sizeof linebufB,fhB);
		if (eofA && eofB)
			break;
		if (eofA && !eofB) {
			printf("\e[31;1m<\e[0m  %s",linebufA);
			goto done;
		}
		if (!eofA && eofB) {
			printf("\e[32;1m>\e[0m  %s",linebufB);
			goto done;
		}
		if (strlen(linebufA)!=strlen(linebufB) || strcmp(linebufA,linebufB)) {
			printf("\e[31;1m<\e[0m  %s",linebufA);
			printf("\e[32;1m>\e[0m  %s",linebufB);
			goto done;
		}
	}
	exitCode=0; /*meaning, no difference in files' contents*/
done:
	if (fhA)
		fclose(fhA);
	if (fhB)
		fclose(fhB);
	return exitCode;
}
