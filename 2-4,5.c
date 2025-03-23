/* Vainstein K 2025mar20 */
#include <stdio.h>
#include <string.h>

void squeeze (char *const s1, const char *s2) /* Ex 2-4 */
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

int any (const char *const s1, const char *s2) /* Ex 2-5 */
{
	const int s1len = strlen(s1), s2len = strlen(s2);
	int i, j;
	for (i=0; i < s1len; ++i) {
		const char chCompare = s1[i];
		for (j=0; j < s2len; ++j)
			if (chCompare == s2[j])
				return i;
	}
	return -1;
}

main ()
{
	char buf[128];
#define TEST_squeeze(s1,s2)  \
		strcpy(buf, s1);     \
		squeeze(buf,s2);     \
		printf("Ln%3d:   squeeze(\"%s\",\"%s\")  ==>  \"%s\"\n", __LINE__,s1,s2,buf)
	TEST_squeeze("","farce");
	TEST_squeeze("antarafacial","farce");
	TEST_squeeze("antarafacial","xyz");
	TEST_squeeze("antarafacial","");
	TEST_squeeze("mississippi","mips RISC");
	TEST_squeeze("mississippi","iiii");

	printf("\e[35m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\e[0m\n");
	int answ;
#define TEST_any(s1,s2)   \
	answ = any(s1,s2);  \
	if (answ >= 0)   \
		printf("Ln%3d:   any(\"%s\",\"%s\")  ==>  %d  i.e. '%c'\n",__LINE__,s1,s2,answ,s1[answ]);  \
	else   \
		printf("Ln%3d:   any(\"%s\",\"%s\")  ==>  -1\n",__LINE__,s1,s2)
	TEST_any("","farce");
	TEST_any("antarafacial","farce");
	TEST_any("antarafacial","auntie");
	TEST_any("antarafacial","xyz");
	TEST_any("antarafacial","");
	TEST_any("mississippi","mips RISC");
	TEST_any("mississippi","iiii");
	TEST_any("mississippi","pure");
}
