/* Vainstein K 2025mar17 */
#include <stdio.h>

int convert_C_to_F (int degC) {
	const int degF = (int)(((float)degC * 9.0) / 5.0) + 32;
	return degF;
}

main () {
	const int degC_min = -40, degC_max = 140, degC_step = 20;
	int degC;
	printf("\tdegC\tdegF\n");
	printf("\t----\t----\n");
	for (degC = degC_min; degC <= degC_max; degC += degC_step) {
		printf("\t%4d\t%4d\n", degC, convert_C_to_F(degC));
	}
}
