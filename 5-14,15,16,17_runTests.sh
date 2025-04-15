# Vainstein K 2025apr13
# vim:ts=4:ft=sh
[ -n "$1" ] && declare -r binaryName=$1 || declare -r binaryName='a.exe'

# To remind: -f ==> case-insense ; -d ==> only consider alphanum and blanks.

# This precursor is easy to edit (add or remove lines), but not an ideal test...
declare -r precursor_to_inputLinesA=\
$'10000.4\n'\
$'bunnies\n'\
$'9.7\n'\
$'aardvark\n'\
''
# ...but shave off the single trailing newline, and our input is ready!
declare -r inputLinesA="${precursor_to_inputLinesA::-1}"

# Now similarly prepare a few more input linesets.

declare -r precursor_to_inputLinesB=\
$'co-pay\n'\
$'Bunnies\n'\
$'A\'ardvaq the Martian\n'\
$'C:\\obra\n'\
$'aardvark\n'\
''
declare -r inputLinesB="${precursor_to_inputLinesB::-1}"

declare -r precursor_to_inputLinesC=\
$'aaa	100		flarp\n'\
$'bbb	99		fnord\n'\
$'aaa	123		foo		flarpeggio\n'\
$'ccc	207		qux\n'\
$'aaa	133\n'\
''
declare -r inputLinesC="${precursor_to_inputLinesC::-1}"


declare flags=''
announceTest () {
	flags=$2
	printf "\n\e[35m%-35s     \e[0m\e[33m%s\e[0m\n" "$1" "$flags"
}

announceTest '(baseline) Plain alpha'   ''
./$binaryName $flags <<<"$inputLinesA"

announceTest '(baseline) Numeric'   '-n'
./$binaryName $flags <<<"$inputLinesA"

announceTest 'Alpha reverse'   '-r'
./$binaryName $flags <<<"$inputLinesA"

announceTest 'Numeric reverse'   '-nr'
./$binaryName $flags <<<"$inputLinesA"

announceTest 'Numeric reverse'   '-rn'
./$binaryName $flags <<<"$inputLinesA"

announceTest 'Alpha'   ''
./$binaryName $flags <<<"$inputLinesB"

announceTest 'Alpha case-insens'   '-f'
./$binaryName $flags <<<"$inputLinesB"

announceTest 'Alpha case-insens reverse dirstyle'   '-fdr'
./$binaryName $flags <<<"$inputLinesB"

announceTest 'Alpha case-insens dirstyle'   '-df'
./$binaryName $flags <<<"$inputLinesB"

announceTest 'Alpha case-insens reverse'   '-fr'
./$binaryName $flags <<<"$inputLinesB"

announceTest 'Alpha-of-1 Numer-of-2'   '-1 -n2'
./$binaryName $flags <<<"$inputLinesC"

announceTest 'Alpha-of-1 Numer-Rev-of-2'   '-1 -n2r'
./$binaryName $flags <<<"$inputLinesC"

exit 0
