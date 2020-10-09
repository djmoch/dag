/* See LICENSE file for copyright and license details */
#include <stdio.h>
#include <string.h>
#include "string.h"

int
test_strnswp1()
{
	char big[] = "thisandthat";
	char old[] = "and";
	char new[] = "foo";
	char exp[] = "thisfoothat";
	strnswp(big, old, new, strlen(big));
	if (strcmp(exp, big) != 0) {
		printf("test_strnswp1 failed:\n\texp: %s\n\tact: %s\n", exp, big);
		return 1;
	}

	return 0;
}

int
test_strnswp2()
{
	char big[12] = "thisandthat";
	char old[] = "and";
	char new[] = "foot";
	char exp[] = "thisfootthat";
	strnswp(big, old, new, 12);
	if (strcmp(exp, big) != 0) {
		printf("test_strnswp2 failed:\n\texp: %s\n\tact: %s\n", exp, big);
		return 1;
	}

	return 0;
}

int
test_strnswp3()
{
	char big[] = "thisandthat";
	char old[] = "and";
	char new[] = "fo";
	char exp[] = "thisfothat";
	strnswp(big, old, new, strlen(big));
	if (strcmp(exp, big) != 0) {
		printf("test_strnswp3 failed:\n\texp: %s\n\tact: %s\n", exp, big);
		return 1;
	}

	return 0;
}

int
test_strnswp4()
{
	char big[] = "thisandthat";
	char old[] = "and";
	char new[] = "";
	char exp[] = "thisthat";
	strnswp(big, old, new, strlen(big));
	if (strcmp(exp, big) != 0) {
		printf("test_strnswp4 failed:\n\texp: %s\n\tact: %s\n", exp, big);
		return 1;
	}

	return 0;
}

int
test_strnswp5()
{
	char big[] = "thisandthat";
	char old[] = "and";
	char new[] = "foot";
	char exp[] = "thisandthat";
	strnswp(big, old, new, strlen(big));
	if (strcmp(exp, big) != 0) {
		printf("test_strnswp5 failed:\n\texp: %s\n\tact: %s\n", exp, big);
		return 1;
	}

	return 0;
}

int
test_strnswp6()
{
	char big[] = "thisandthat";
	char old[] = "foot";
	char new[] = "bat";
	char exp[] = "thisandthat";
	strnswp(big, old, new, strlen(big));
	if (strcmp(exp, big) != 0) {
		printf("test_strnswp6 failed:\n\texp: %s\n\tact: %s\n", exp, big);
		return 1;
	}

	return 0;
}

int main()
{
	int retval = test_strnswp1();
	retval |= test_strnswp2();
	retval |= test_strnswp3();
	retval |= test_strnswp4();
	retval |= test_strnswp5();
	retval |= test_strnswp6();
	return retval;
}
