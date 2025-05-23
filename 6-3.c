/* Vainstein K 2025apr20 */
/*Test steps:
	./a.exe < sampleDocument.txt
*/

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h> /* for qsort */
#include <string.h>

typedef struct {
	char *_word;
	int   _line;
} occurrence_t;

occurrence_t *items;
int nItems_used=0, nItems_max = 0;
#define TABSIZE_INI 128

int cmp_byWord_thenLine (occurrence_t *const a, occurrence_t *const b)
{
	int const cmpWord = strcmp(a->_word, b->_word);
	return cmpWord ? cmpWord : (b->_line - a->_line);
}
typedef int(*cmpFunc)(void const*,void const*);

void addItem (int const line, char const *const s)
{
	int const sLen = strlen(s), idest = nItems_used++;
	if (idest == nItems_max) {
		if (nItems_max)
			items = realloc(items, sizeof(occurrence_t) * (nItems_max *= 2));
		else
			items = malloc(        sizeof(occurrence_t) * (nItems_max = TABSIZE_INI));
	}
	strcpy( (items[idest]._word = malloc(sLen+1)) , s);
	items[idest]._line = line;
}

bool isNoiseword (char const *const s)
{
	static char const *blacklist[] = {
		"the" , "a" , "an" ,
		"is", "was", "will", "are", "were",
		"and" , "or" , "not", "as", "but",
		"of", "at", "on", "in",
		"what", "it", "that" , "this" ,
	};
	int i;
	for (i = 0; i < sizeof blacklist / sizeof blacklist[0]; ++i)
		if (!strcmp(s, blacklist[i]))
			return true;
	return false;
}

#define WORD_SEPARATOR_CHARS " \t\n\".,;:()[]<>-?!/"
#define BUFLEN 1024
main ()
{
	const occurrence_t *prev, *curr;
	int i, lineOrdinal=0;
	char line[BUFLEN], *word;

	while (fgets(line,sizeof line,stdin) && ++lineOrdinal) {
		if (! (word = strtok(line,WORD_SEPARATOR_CHARS)))
			continue;
		do {
			word[0] = tolower(word[0]);
			if (! isNoiseword(word))
				addItem(lineOrdinal, word);
		} while (word = strtok(NULL,WORD_SEPARATOR_CHARS));
	}

	qsort(items, nItems_used, sizeof(occurrence_t), (cmpFunc)cmp_byWord_thenLine);

	for (prev=NULL, i = 0; i < nItems_used; ++i) {
		curr = &items[i];
		if (i==0) {
			printf("%s %d", curr->_word, curr->_line);
		} else {
			if (strcmp(prev->_word,curr->_word)) /* word changed? */
				printf("\n%s %d", curr->_word, curr->_line);
			else {
				if (prev->_line != curr->_line)
					printf(" %d", curr->_line);
			}
		}
		prev = curr;
	}
	printf("\n");

	for (i = 0; i < nItems_used; ++i)
		free(items[i]._word);
	free(items);
}
