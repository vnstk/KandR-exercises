/* Vainstein K 2025mar20 */
#include <stdio.h>
#include <string.h>

int any (const char *const s1, const char *s2)
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
	int answ;
#define TEST(s1,s2)   \
	answ = any(s1,s2);  \
	if (answ >= 0)   \
		printf("Ln=%-2d|  any(\"%s\",\"%s\")  ==>  %d  i.e. '%c'\n",__LINE__,s1,s2,answ,s1[answ]);  \
	else   \
		printf("Ln=%-2d|  any(\"%s\",\"%s\")  ==>  -1\n",__LINE__,s1,s2)

	TEST("","farce");
	TEST("antarafacial","farce");
	TEST("antarafacial","auntie");
	TEST("antarafacial","xyz");
	TEST("antarafacial","");
	TEST("mississippi","mips RISC");
	TEST("mississippi","iiii");
	TEST("mississippi","pure");
}
