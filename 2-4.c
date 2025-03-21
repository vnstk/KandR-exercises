/* Vainstein K 2025mar20 */
#include <stdio.h>
#include <string.h>

void squeeze (char *const s1, const char *s2)
{
	while (*s1 && *s2) {
		const char chDel = *s2++;
		const int s1len = strlen(s1);
		int i, j;
		for (i=0, j=0; i < s1len; ++i)
			if (s1[i] != chDel)
				s1[j++] = s1[i];
		s1[j] = '\0';
	}
}

main ()
{
	char buf[128];
#define TEST(s1,s2)         \
		strcpy(buf, s1);    \
		squeeze(buf,s2);    \
		printf("Ln%-2d/  squeeze(\"%s\",\"%s\")  ==>  \"%s\"\n", __LINE__,s1,s2,buf)
	TEST("","farce");
	TEST("antarafacial","farce");
	TEST("antarafacial","xyz");
	TEST("antarafacial","");
	TEST("mississippi","mips RISC");
	TEST("mississippi","iiii");
}
