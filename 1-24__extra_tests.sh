# Vainstein K 2025mar20
# vim:ts=4:ft=sh
declare -r tmpf=`mktemp`
declare line=''
while read -r line; do
#	echo -e "\n\e[35m${line}\e[0m" >&2
# Not that simple; what if line contains any double backslashes??
	{ echo -ne "\n\e[35m"; echo -n "${line}"; echo -e "\e[0m" ;} >&2
	echo "${line}" > $tmpf
	./a.exe <$tmpf
	echo -e "\t\e[35;1m$?\e[0m" >&2
done < 1-24__extra_tests.txt
rm $tmpf
exit 0
