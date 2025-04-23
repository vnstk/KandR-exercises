/* Vainstein K 2025apr21 */
/*Test steps:
	diff -b <(./a.exe < 6-6_testInput.c) <(gcc -E 6-6_testInput.c|grep -v '^#')
*/
/* Constraints and simplifications:
	o	Assume there will be no whtspc between "#" and the preprocessor
		directive.
	o	Macros are expanded even within string literals and comments.
	o	Not supporting line continuations.
	o	Say macroA's definition involves macroB.  This program expands macroA's definition only once, when it sees macroA defined; the real cpp [C pre-processor] expands macroA's definition every time macroA is used.
*/

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h> /*For malloc */
#include <string.h>


/*=================================== copied from pp 144-145: BEGIN ===*/
struct nlist {	   /* table entry: */
	struct nlist *next;   /* next entry in chain */
	char *name;		   /* defined name */
	char *defn;		   /* replacement text */
};

#define HASHSIZE 101
static struct nlist *hashtab[HASHSIZE];  /* pointer table */

/* hash:  form hash value for string s */
unsigned hash(char *s)
{
	unsigned hashval;
	for (hashval = 0; *s != '\0'; s++)
		hashval = *s + 31 * hashval;
	return hashval % HASHSIZE;
}

/* lookup:  look for s in hashtab */
struct nlist *lookup(char *s)
{
	struct nlist *np;
	for (np = hashtab[hash(s)];  np != NULL; np = np->next)
		if (strcmp(s, np->name) == 0)
			return np;	 /* found */
	return NULL;		   /* not found */
}

/* install:  put (name, defn) in hashtab */
struct nlist *install(char *name, char *defn)
{
	struct nlist *np;
	unsigned hashval;
	if ((np = lookup(name)) == NULL) { /* not found */
		np = (struct nlist *) malloc(sizeof(*np));
		if (np == NULL || (np->name = strdup(name)) == NULL)
			return NULL;
		hashval = hash(name);
		np->next = hashtab[hashval];
		hashtab[hashval] = np;
	} else	   /* already there */
		free((void *) np->defn);   /*free previous defn */
	if ((np->defn = strdup(defn)) == NULL)
		return NULL;
	return np;
}
/*=================================== copied from pp 144-145: END ===*/


void chomp (char *s) /* like Perl5's chomp() */
{
	const size_t len = strlen(s);
	if (len && s[len-1]=='\n')
		s[len-1]='\0';
}

bool startsWith (char const *const s, char const *const maybePrefix)
{
	return ! strncmp(s, maybePrefix, strlen(maybePrefix));
}

#if 0
/* It's a GNU extension to provide strdup in libc */
char *strdup(const char *s)
{
	const size_t len = strlen(s);
	char *const t = malloc(len+1);
	if (t)
		strcpy(t, s);
	return t;
}
#endif


#define LINEBUFLEN 1024
#define MUCHBUFLEN 10240
#define NAMEBUFLEN 128

void destroyOneEntry (struct nlist *p)
{
	if (p->name)
		free(p->name);
	if (p->defn)
		free(p->defn);
	free(p);
}
void destroyEntireTable (void)
{
	int i;
	for (i = 0; i < HASHSIZE; ++i) {
		struct nlist *p = hashtab[i];
		while (p) {
			struct nlist *pnext = p->next;
			destroyOneEntry(p);
			p = pnext;
		}
	}
}

void undef (char *s, bool *found) /* This func def is answ to 6-5 */
{
	struct nlist **chainStart = &hashtab[hash(s)];
	struct nlist *npPrev=NULL, *npCurr;
	for (npCurr = *chainStart; npCurr; npCurr = npCurr->next) {
		if (! strcmp(s, npCurr->name)) {
			*found = true;
			if (npPrev)
				npPrev->next = npCurr->next;
			else
				*chainStart = npCurr->next;
			destroyOneEntry(npCurr);
			return;
		}
	}
	*found = false;
}

char const *skipWhtspc (char const *s)
{
	return s + strspn(s," \t");
}

void expandMacros (char const *inp, char *outp, int outpLen)
{
	struct nlist *expansion;
	int j, fragmentLen;
	char const *inpsav = inp;
	char ch, name[NAMEBUFLEN];
	memset(outp,'\0',outpLen);
#ifdef DBG
	printf("\t\traw= \e[32m%s\e[0m\n", inp);
#endif
	for (j = 0; j < outpLen; ) {

		/* scan until start of a name */
		inpsav = inp;
		while ((ch = *inp) && ! (isalpha(ch) || ch=='_'))
			inp++;
		fragmentLen = inp - inpsav;
		strncat(outp+j, inpsav, fragmentLen);
		j += fragmentLen;
		if (! *inp)
			break;

		/* scan to end of name */
		inpsav = inp;
		while ((ch = *inp) && (isalpha(ch) || isdigit(ch) || ch=='_'))
			inp++;
		fragmentLen = inp - inpsav;
		memset(name,'\0',sizeof name); /*strncpy may not write NUL!*/
		strncpy(name, inpsav, fragmentLen);
		if (! *inp)
			break;

#ifdef DBG
		printf("\t\t\t\e[3mUplook  \e[23;7m%s\e[0m\n", name);
#endif
		if (expansion = lookup(name)) {
			strcat(outp+j, expansion->defn);
			j += strlen(expansion->defn);
		} else {
			strcat(outp+j, name);
			j += strlen(name);
		}
	}
#ifdef DBG
	printf("\t\texp= \e[36m%s\e[0m\n", outp);
#endif
}


bool handleMacro_define (char const *buf)
{
	char expanded[MUCHBUFLEN];
	char name[NAMEBUFLEN] = {'\0',};
	size_t nameLen;
	static const char *const lookFor = "#define ";
	if (! startsWith(buf, lookFor))
		return false;
	buf += strlen(lookFor);
	buf = skipWhtspc(buf);
	nameLen = strcspn(buf," \t("); /* Note the '(' ! */
	assert(nameLen < sizeof name);
	strncpy(name, buf, nameLen);
	buf += nameLen;
	if (*buf == '(')
		return false;
	buf = skipWhtspc(buf);
	/* NB: is perfectly legal for definition to be empty string. */
	expandMacros(buf, expanded, sizeof expanded);
	if (! install(name, expanded))
		assert(false);
#ifdef DBG
	printf("\t\t\t\e[35mOK define \e[1m%s\e[22m ==> \e[7m%s\e[0m\n",
	       name,expanded);
#endif
	putchar('\n');
	return true;
}

bool handleMacro_undef (char const *buf)
{
	bool found;
	char name[NAMEBUFLEN] = {'\0',};
	size_t nameLen;
	static const char *const lookFor = "#undef ";
	if (! startsWith(buf, lookFor))
		return false;
	buf += strlen(lookFor);
	buf = skipWhtspc(buf);
	nameLen = strcspn(buf," \t");
	assert(nameLen < sizeof name);
	strncpy(name, buf, nameLen);
	undef(name, &found);
#ifdef DBG
	printf("\t\t\t\e[35m%s undef \e[1m%s\e[22m%s\e[0m\n",
	       found ? "OK" : "Fail", name, found ? "" : " --- not found");
#endif
	putchar('\n');
	return true;
}


main()
{
	int lineOrdinal = 0;
	char line[LINEBUFLEN], expanded[MUCHBUFLEN];
	while (++lineOrdinal && fgets(line,sizeof line,stdin)) {
		chomp(line);
		if (! *line) {
			putchar('\n');
			continue;
		}
#ifdef DBG
		printf("\n\e[33;7m%s\e[27;3m    inp line #%d\e[0m\n", line,lineOrdinal);
#endif
		if (! handleMacro_define(line) && ! handleMacro_undef(line)) {
			expandMacros(line, expanded, sizeof expanded);
			puts(expanded);
		}
	}
	destroyEntireTable();
}
