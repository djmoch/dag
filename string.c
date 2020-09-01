/* See LICENSE file for copyright and license details */
#include <stdio.h>
#include <string.h>

int
strbegin(const char *big, const char *little) {
	return (strstr(big, little) == big);
}

void
strnswp(char *big, const char *old, const char *new, size_t max) {
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
dedup(char **strarray) {
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
