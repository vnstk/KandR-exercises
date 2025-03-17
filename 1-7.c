/* Vainstein K 2025mar17 */
/* Invoke as e.g.
					./a.exe < 1-7.c
  ; or even
					./a.exe < a.exe
  (if you want to find out location of the first 0xFF in the file).
*/
#include <stdio.h>
main () {
	unsigned nbytes = 0U;
	char ch;
	while ((ch = getchar()) != EOF)
		++nbytes;
	printf("After %u-byte input, EOF = %d\n", nbytes, (int)ch);
}
/* EOF is -1 */
