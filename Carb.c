/*
 * https://www.linux.org.ru/forum/development/10349962?cid=10355425
 */

#include <stdint.h>
#include <string.h>

static inline uint8_t
_strcmp(char *str)
{
	return (*(str + 0) == 'd') && (*(str + 1) == 'e')
	    && (*(str + 2) == 'b') && (*(str + 3) == 'u')
	    && (*(str + 4) == 'g');
}

static char *
debugdel(char *str)
{
	char *ret = str;
	if (_strcmp(str + 0) && (*(str + 5) == ' ' || *(str + 5) == '\0'))
		memset(str + 0, ' ', 5), str += 5;
	while ((str = strchr(++str, ' ')))
		if ((*(str + 0) == ' ') && _strcmp(str + 1)
		    && (*(str + 6) == ' ' || *(str + 6) == '\0'))
			memset(str + 1, ' ', 5), str += 5;
	return ret;
}

char *
carb_wrapper(char *hey, char *needle)
{
	return debugdel(hey);
}

char *
strreplace(char *str, char *needl)
{
	char *ret = str;
	typeof(strlen(needl)) needl_len = strlen(needl);
	while ((str = strstr(str, needl))) {
		if ((str == ret) || (*(str - 1) == ' ')) {
			char end = *(str + needl_len);
			if ((end == ' ') || (end == '\0'))
				memset(str, ' ', needl_len);
		}
		str += needl_len;
	}
	return ret;
}
