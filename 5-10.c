/* Vainstein K 2025apr12 */

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h> /* for exit(3) */
#include <string.h>

#define DIE(fmtSpec,...) do {   \
	printf("(Ln%d)\e[31m  " fmtSpec "\e[0m\n", __LINE__, __VA_ARGS__);   \
	exit(3);   \
} while (0)

typedef struct {
	char _operator; /* '\0' if operand, ASCII code of operator if operator */
	long _operand;  /* value of operand if operand, garbage if operator */
} cmdTok_t;

#define MAX_STACK_DEPTH 100
static cmdTok_t stack[MAX_STACK_DEPTH];
static int top = -1;

main (int argc, char *argv[])
{
	cmdTok_t *curr;
	long opd1st, opd2nd;

	while (*++argv) {
		const size_t len = strlen(*argv);
		assert(len);

		/* If operand, push onto stack; if operator, eval it. */

		if (len==1 && strpbrk(*argv, "+-/d*%")) { /* An operator? */
			const char opcode = *argv[0];
			if (top < 1)
				DIE("%s '%c'", "too few args for operator", opcode);
			/* read opd2nd */
			curr = &stack[top--];
			if (curr->_operator)
				DIE("%s", "penultimate elem not an operand");
			opd2nd = curr->_operand;
			/* read opd1st */
			curr = &stack[top--];
			if (curr->_operator)
				DIE("%s", "antepenultimate elem not an operand");
			opd1st = curr->_operand;
			/* eval */
			if ((opcode == '/' || opcode == '%') && 0 == opd2nd)
				DIE("%s '%c'", "operation not defined if 2nd operand == 0", opcode);
			curr = &stack[++top];
			curr->_operator = '\0';
			switch (opcode) {
				case '+':   curr->_operand = opd2nd + opd1st;   break;
				case '-':   curr->_operand = opd2nd - opd1st;   break;
				case '*':   curr->_operand = opd2nd * opd1st;   break;
				case 'd': /* MSYS2 "helpfully" converts '/' to "C:\msys64\", so.  */
				case '/':   curr->_operand = opd2nd / opd1st;   break;
				case '%':   curr->_operand = opd2nd % opd1st;   break;
			}
		} else {                                 /* An operand? */
			if (top == MAX_STACK_DEPTH-1)
				DIE("%s", "No room in token stack.");
			curr = &stack[++top];
			char *unconvertedSuffix = NULL;
			errno=0;
			curr->_operand = strtol(*argv, &unconvertedSuffix, /*radix*/10);
			if (errno)
				DIE("%s '%s': %s", "Num conv fail", *argv, strerror(errno));
			else if ((int)(unconvertedSuffix - *argv) == len)
				curr->_operator = '\0';
			else
				DIE("%s '%s'", "Num conv misc fail", *argv);
		}
	}

	/* Done processing inputs */
	if (top != 0)
		DIE("%s %d", "illegal expr --- stack has wrong # elems", top+1);
	curr = &stack[top];
	if (curr->_operator)
		DIE("%s", "illegal expr --- top not an operand");
	printf("%ld\n", curr->_operand);
}
