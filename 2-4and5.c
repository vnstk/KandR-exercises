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
#define TESTsqueeze(s1,s2)  \
		strcpy(buf, s1);    \
		squeeze(buf,s2);    \
		printf("Ln%3d:   squeeze(\"%s\",\"%s\")  ==>  \"%s\"\n", __LINE__,s1,s2,buf)
	TESTsqueeze("","farce");
	TESTsqueeze("antarafacial","farce");
	TESTsqueeze("antarafacial","xyz");
	TESTsqueeze("antarafacial","");
	TESTsqueeze("mississippi","mips RISC");
	TESTsqueeze("mississippi","iiii");

	printf("\n-----------------------------------------------------\n\n");

	int answ;
#define TESTany(s1,s2)   \
	answ = any(s1,s2);  \
	if (answ >= 0)   \
		printf("Ln%3d:   any(\"%s\",\"%s\")  ==>  %d  i.e. '%c'\n",__LINE__,s1,s2,answ,s1[answ]);  \
	else   \
		printf("Ln%3d:   any(\"%s\",\"%s\")  ==>  -1\n",__LINE__,s1,s2)
	TESTany("","farce");
	TESTany("antarafacial","farce");
	TESTany("antarafacial","auntie");
	TESTany("antarafacial","xyz");
	TESTany("antarafacial","");
	TESTany("mississippi","mips RISC");
	TESTany("mississippi","iiii");
	TESTany("mississippi","pure");
}
