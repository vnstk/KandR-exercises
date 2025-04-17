# Vainstein K 2025apr13
# vim:ts=4:ft=sh
[ -n "$1" ] && declare -r binaryName=$1 || declare -r binaryName='a.exe'

dcl() {
	local -r inp=$1 outpExpected=$2
	printf "\n\e[35;7m(Ln%3d)\e[27;1m   %s\e[0m\n" ${BASH_LINENO[0]} "$inp"
	[ -n "$outpExpected" ] && {
		printf "\e[33m%s   \e[7;3m(Exp)\e[0m\n" "$outpExpected"
	}
	./$binaryName <<<"$inp" || printf "\e[31mExited %d\e[0m\n" $?
}

dcl		'float foo(char ch, unsigned un)'

exit 0

dcl		'qux[5]'
dcl		'char*const* (*foonction)()'
dcl		'char (*(*x[3])( ))[5]'

dcl		'float foo(char   , unsigned   )'
dcl		'float foo()'

exit 0
dcl		'static const int **static_ptr_to_ptr_to_const_int'
dcl		'int (*daytab)[13]'
dcl		'unsigned int u'
dcl		'float ar[5]'
dcl		'int argc'
dcl		'extern register int argc'

# Addit'l test --- BEGIN
dcl		'int argc'
# Addit'l test --- END

# Test cases which dcl could not handle before my changes --- BEGIN
dcl		'extern register unsigned long int argc'
dcl		'extern register int argc'
dcl		'char (*(*x[3])( ))[5]'
# Test cases which dcl could not handle before my changes --- END

# Test cases from the book (p 122), here as regression test cases --- BEGIN
dcl		'char **argv'   'argv:  pointer to char'
dcl		'int *daytab[13]'   'daytab:  array[13] of pointer to int'
dcl		'void *comp()'   'comp: function returning pointer to void'
dcl		'void (*comp)()'   'comp: pointer to function returning void'
dcl		'char (*(*x())[])()'   'x: function returning pointer to array[] of pointer to function returning char'
dcl		'char (*(*x[3])())[5]'   'x: array[3] of pointer to function returning pointer to array[5] of char'
# Test cases from the book (p 122), here as regression test cases --- END


exit 0
