/* Vainstein K 2025apr21 */
/*Test steps:
	./a.exe < sampleDocument.txt
*/

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h> /* for qsort */
#include <string.h>

char **words;
int nWords_used=0, nWords_max = 0;

typedef struct {
	char *_word;
	int   _count;
} occurrence_t;
occurrence_t *items;
int nItems_used=0, nItems_max = 0;

#define TABSIZE_INI 128

int cmp_word (char const **a, char const **b)
{
	return strcmp(*a,*b);
}
int cmp_occurrence_byCountDesc (occurrence_t const *a, occurrence_t const *b)
{
	return b->_count - a->_count;
}
typedef int(*cmpFunc)(void const*,void const*);

void addWord (char const *const s)
{
	int const sLen = strlen(s), idest = nWords_used++;
	if (idest == nWords_max) {
		if (nWords_max)
			words = realloc(words, sizeof(char*) * (nWords_max *= 2));
		else
			words = malloc(        sizeof(char*) * (nWords_max = TABSIZE_INI));
	}
	strcpy( (words[idest] = malloc(sLen+1)) , s);
}
occurrence_t *addItem (char *const sStored)
{
	int const idest = nItems_used++;
	if (idest == nItems_max) {
		if (nItems_max)
			items = realloc(items, sizeof(occurrence_t) * (nItems_max *= 2));
		else
			items = malloc(        sizeof(occurrence_t) * (nItems_max = TABSIZE_INI));
	}
	occurrence_t *const occ = &items[idest];
	occ->_word = sStored; /* No need to strcpy */
	occ->_count = 1;
	return occ;
}

#define WORD_SEPARATOR_CHARS " \t\n\".,;:()[]<>-?!/"
#define BUFLEN 1024
main ()
{
	occurrence_t *occ;
	int i;
	char line[BUFLEN];
	char *word, *curr, *prev;

	while (fgets(line,sizeof line,stdin)) {
		if (! (word = strtok(line,WORD_SEPARATOR_CHARS)))
			continue;
		do {
			word[0] = tolower(word[0]);
			addWord(word);
		} while (word = strtok(NULL,WORD_SEPARATOR_CHARS));
	}
	if (! nWords_used)
		return 1;

	qsort(words, nWords_used, sizeof(char*), (cmpFunc)cmp_word);

	for (occ=NULL, prev=NULL, i = 0; i < nWords_used; ++i) {
		curr = words[i];
		if (i == 0) {
			occ = addItem(curr);
		} else {
			if (strcmp(prev,curr)) /* word changed? */
				occ = addItem(curr);
			else {
				occ->_count++;
				free(curr);
				continue;
			}
		}
		prev = curr;
	}
#ifdef DBG
	fprintf(stderr, "\e[36m""nWords_used=%d nWords_max=%d , nItems_used=%d nItems_max=%d\e[0m\n",
	        nWords_used,nWords_max , nItems_used,nItems_max);
#endif

	qsort(items, nItems_used, sizeof(occurrence_t), (cmpFunc)cmp_occurrence_byCountDesc);

	for (i = 0; i < nItems_used; ++i) {
		occ = &items[i];
		printf("%3d %s\n", occ->_count, occ->_word);
		free(occ->_word);
	}
	free(items);
	free(words);
}
