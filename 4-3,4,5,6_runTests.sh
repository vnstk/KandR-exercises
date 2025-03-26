# Vainstein K 2025mar25
# vim:ts=4:ft=sh
[ -n "$1" ] && declare -r binaryName=$1 || declare -r binaryName='a.exe'
echo "1[$1]"
declare -r tmpf=`mktemp`
declare line=''
declare expected=''
while read -r line; do

	[ -z "$line" ] && {
		./${binaryName} <$tmpf | sed 's/^/\t\o33[34m  actual:\o33[0m\t/'
		:>$tmpf
		[ -n "$expected" ] && echo -e "\t\e[32mexpected:\t\t${expected}\e[0m"
		expected=''
		echo
		continue
		# Hence, input file ought to end with a blank line.
	}

	[ ${line:0:1} == '>' ] && {
		expected=${line:2}  # Skip past the tab
		continue
	}

	echo -e "\e[35m${line}\e[0m" >&2

	[ ${line:0:1} == '#' ] || echo "${line}" >>$tmpf

done < '4-3,4,5,6_tests.txt'
rm $tmpf
exit 0
