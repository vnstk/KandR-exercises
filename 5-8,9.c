/* Vainstein K 2025apr09 */


/*=================================== copied from pg 111: BEGIN ===*/
static char daytab[2][13] = {
	{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
	{0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};
/* day_of_year:  set day of year from month & day */
int day_of_yearBook(int year, int month, int day)
{
	int i, leap;
	leap = year%4 == 0 && year%100 != 0 || year%400 == 0;
	for (i = 1; i < month; i++)
		day += daytab[leap][i];
	return day;
}
/* month_day:  set month, day from day of year */
void month_dayBook(int year, int yearday, int *pmonth, int *pday)
{
	int i, leap;
	leap = year%4 == 0 && year%100 != 0 || year%400 == 0;
	for (i = 1; yearday > daytab[leap][i]; i++)
		yearday -= daytab[leap][i];
	*pmonth = i;
	*pday = yearday;
}
/*=================================== copied from pg 111: END ===*/


#include <assert.h>

/* Ex 5-8: will report error by populating outputs with 0.
*/
int day_of_yearErrCheck(int year, int month, int day)
{
	int i, leap, yearday;
	if (year < 0 || month < 1 || month > 12 || day < 1)
		return 0;
	leap = year%4 == 0 && year%100 != 0 || year%400 == 0;
	if (day > daytab[leap][month])
		return 0;
	yearday = day;
	for (i = 1; i < month; i++)
		yearday += daytab[leap][i];
	return yearday;
}
void month_dayErrCheck(int year, int yearday, int *pmonth, int *pday)
{
	int i, leap;
	if (!pmonth || !pday) { /* If can't obey contract, at least don't segfault. */
		return;
	}
	leap = year%4 == 0 && year%100 != 0 || year%400 == 0;
	if (year < 0 || yearday < 1 || (leap && yearday > 366) || (!leap && yearday > 365)) {
		*pmonth = *pday = 0;
		return;
	}
	for (i = 1; yearday > daytab[leap][i]; i++)
		yearday -= daytab[leap][i];
	*pmonth = i;
	*pday = yearday;
}

/* Ex 5-9: use pointers instead of array indexing.
*/
int day_of_yearPtrwise(int year, int month, int day)
{
	assert(! (year < 0 || month < 1 || month > 12 || day < 1 || day > 31));
	const char (*daytabRow)[13];
	const char  *daytabCell;
	int i, leap, yearday = day;
	leap = year%4 == 0 && year%100 != 0 || year%400 == 0;
	daytabRow = daytab + leap;
	daytabCell = *daytabRow;
	for (i = 1; i < month; i++)
		yearday += *++daytabCell;
	return yearday;
}
void month_dayPtrwise(int year, int yearday, int *pmonth, int *pday)
{
	assert(! (year < 0 || yearday < 1 || yearday > 366));
	assert(pmonth && pday);
	const char *daytabCell;
	int i, leap;
	leap = year%4 == 0 && year%100 != 0 || year%400 == 0;
	daytabCell = *(daytab + leap);
	for (i = 1, ++daytabCell; yearday > *daytabCell; i++)
		yearday -= *daytabCell++;
	*pmonth = i;
	*pday = yearday;
}


#include <stdio.h>

main ()
{
	int retval_yearday;
#define TEST_day_of_year(which,y,m,d,wanted__yd) \
	retval_yearday = day_of_year ## which((y),(m),(d));   \
    printf("Ln%3d:   day_of_year%s(y %d, m %d, d %d)   ===>   "   \
	       "yd=%d \e[33m%d\e[0m:wanted\n", __LINE__,   \
	       #which, (y),(m),(d), retval_yearday, wanted__yd);   \
	if (retval_yearday != wanted__yd) printf("\t\e[31;1mERR!!!\e[0m\n")

	int outparam_month, outparam_day;
#define TEST_month_day(which,y,yd,wanted__m,wanted__d) \
	outparam_month = outparam_day = -579;  \
	month_day ## which((y),(yd),&outparam_month,&outparam_day);   \
    printf("Ln%3d:   month_day%s(y %d, yd %d)   ===>   "   \
	       "m=%d \e[33m%d\e[0m:wanted   d=%d \e[33m%d\e[0m:wanted\n", __LINE__,   \
	       #which, (y),(yd), outparam_month,wanted__m , outparam_day,wanted__d);   \
	if (outparam_month != wanted__m || outparam_day != wanted__d) printf("\t\e[31;1mERR!!!\e[0m\n")


	/* First, we test the xxxBook() variants to establish baseline behavior. */
	TEST_day_of_year(Book,1988,3,1 , 61);  /* Mar01 is 61st yearday of a yes-leap year */
	TEST_day_of_year(Book,1987,3,1 , 60);  /* Mar01 is 60th yearday of a non-leap year */
	TEST_month_day(Book,1988,60 , 2,29);
	TEST_month_day(Book,1987,60 , 3, 1);
	TEST_month_day(Book,1988,365 , 12,30); /* Dec30 */
	TEST_month_day(Book,1987,365 , 12,31); /* Dec31 */

	printf("\e[35m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\e[0m\n");
	TEST_day_of_year(Ptrwise,1988,3,1 , 61);  /* Mar01 is 61st yearday of a yes-leap year */
	TEST_day_of_year(Ptrwise,1987,3,1 , 60);  /* Mar01 is 60th yearday of a non-leap year */
	TEST_month_day(Ptrwise,1988,60 , 2,29);
	TEST_month_day(Ptrwise,1987,60 , 3, 1);
	TEST_month_day(Ptrwise,1988,365 , 12,30); /* Dec30 */
	TEST_month_day(Ptrwise,1987,365 , 12,31); /* Dec31 */

	printf("\e[35m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\e[0m\n");
	TEST_day_of_year(ErrCheck,  -1, 3, 1  , 0);
	TEST_day_of_year(ErrCheck,1988,13, 1  , 0);
	TEST_day_of_year(ErrCheck,1988, 0, 1  , 0);
	TEST_day_of_year(ErrCheck,1988, 3, 0  , 0);
	TEST_day_of_year(ErrCheck,1988, 3,32  , 0);
	TEST_day_of_year(ErrCheck,1988, 2,29  , 60);
	TEST_day_of_year(ErrCheck,1987, 2,29  , 0);  /* no Feb29th in a non-leap year */
	TEST_month_day(ErrCheck,1988,366 , 12,31);
	TEST_month_day(ErrCheck,1987,366 ,  0, 0); /* No 366 days in a non-leap year */
	TEST_month_day(ErrCheck,1988,367 ,  0, 0); /* no 367 days even in a leap year */
	TEST_month_day(ErrCheck,  -1,60 ,  0, 0);
	TEST_month_day(ErrCheck,  -1,60 ,  0, 0);
	TEST_month_day(ErrCheck,1988,0 ,  0, 0);
	TEST_month_day(ErrCheck,1987,0 ,  0, 0);
}
