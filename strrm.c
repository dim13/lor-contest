#include <string.h>

#define SPACE_CH			' '

char *
strrm(register char *haystack, char *needle)
{
	char *const h = haystack;
	const size_t needle_size = strlen(needle);
	const size_t haystack_size = strlen(haystack);

	if (needle_size <= haystack_size) {
		register char *haystack_end = haystack + haystack_size;

		do {
			char *const space_start = haystack;

			while (*haystack == SPACE_CH) {
				haystack++;
			}

			char *const word_start = haystack;
			register char *word_end = strchr(word_start, SPACE_CH);

			if (word_end == NULL) {
				word_end = haystack_end;
			}

			haystack = word_end;

			if (word_end - word_start == needle_size &&
				memcmp(word_start, needle, needle_size) == 0)
			{
				const register size_t cut_size = word_end - space_start;

				memmove(space_start, word_end, haystack_end - space_start - cut_size + 1);
				haystack = space_start;
				haystack_end -= cut_size;
			}
		} while (*haystack != '\0');
	}

	return h;
}
