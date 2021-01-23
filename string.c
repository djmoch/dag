/* See LICENSE file for copyright and license details */
#include <stdio.h>
#include <string.h>

int
strbegin(const char *big, const char *little)
{
	return (strstr(big, little) == big);
}

int
strend(const char *big, const char *little)
{
	return ((strstr(big, little)) == (big + strlen(big) - strlen(little)));
}

void
strnswp(char *big, const char *old, const char *new, size_t max)
{
	size_t biglen = strlen(big);
	size_t oldlen = strlen(old);
	size_t newlen = strlen(new);
	char *begin = strstr(big, old);

	if ((biglen + newlen - oldlen > max) ||
		(begin == NULL)) {
		return;
	}

	if (newlen > oldlen) {
		for(char* i=&(big[biglen + 1]); i>begin; i--) {
			*(i + (newlen  - oldlen)) = *i;
		}
	}
	else if (oldlen > newlen) {
		for(char* i=begin; i<&(big[biglen + 1]); i++) {
			*i = *(i + (oldlen - newlen));
		}
	}

	memcpy(begin, new, newlen);
}

void
dedup(char **strarray)
{
	int i = 0;
	while(strarray[i] != NULL) {
		for(int j=0; j!=i; j++) {
			if(strcmp(strarray[i], strarray[j]) == 0) {
				for(int k=i; strarray[k]!=NULL; k++) {
					strarray[k] = strarray[k+1];
				}
				break;
			}
		}
		i++;
	}
}

char *
unquote(char *string)
{
	if ((string[0] == '"' && string[strlen(string)-1] == '"') ||
			(string[0] == '\'' && string[strlen(string)-1] == '\'')) {
		string[strlen(string)-1] = '\0';
		return string + 1;
	}
	return string;
}

char *
strnesc(char *str, int maxlen)
{
	int len = strlen(str);
	for (int i=0;;i++) {
		if (str[i] == '\0')
			break;

		if (str[i] == '%') {
			if (len >= maxlen)
				return NULL;

			for (int j=len;j>=i;j--)
				str[j+1] = str[j];
			len += 1;
			i += 1;
		}
	}

	return str;
}

int
strcnt(char *str, char c)
{
	int n = 0;

	for (int i=0;;i++) {
		if (str[i] == '\0')
			break;
		if (str[i] == c)
			n += 1;
	}
	return n;
}

int
sprintf_ct(char *str, char *fmt, char *subst, int ct)
{
	switch (ct) {
	case 0:
		return sprintf(str, fmt, NULL);
	case 1:
		return sprintf(str, fmt, subst);
	case 2:
		return sprintf(str, fmt, subst, subst);
	case 3:
		return sprintf(str, fmt, subst, subst, subst);
	case 4:
		return sprintf(str, fmt, subst, subst, subst, subst);
	case 5:
		return sprintf(str, fmt, subst, subst, subst, subst, subst);
	case 6:
		return sprintf(str, fmt, subst, subst, subst, subst, subst, subst);
	case 7:
		return sprintf(str, fmt, subst, subst, subst, subst, subst, subst, subst);
	case 8:
		return sprintf(str, fmt, subst, subst, subst, subst, subst, subst, subst, subst);
	case 9:
		return sprintf(str, fmt, subst, subst, subst, subst, subst, subst, subst, subst, subst);
	default:
		return 0;
	}
}
