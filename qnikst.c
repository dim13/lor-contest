#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char * undebugq(char *in, char *needle) {
    char *p = in;
    size_t c = 0, l = strlen(in), nl = strlen(needle);
    for (p = in; *p; p++, c++) {
        for (; *(p) == ' '; (p)++);
        if (*p == *needle) {
            if (l < c + nl) { return in; }
            if (strncmp(p, needle, nl) == 0) {
                if (p[nl] == ' ') {
                  memmove(p, p + nl + 1, l - nl + 1); // we need to copy \0
                  l -= nl;
                  continue;
                } else if (p[nl] == '\0') {
                  *p = '\0';
                  return in;
                }
            }
        }
        for (; *p != ' ' && *p; p++);
    }
    return in;
}
/*
main() {
   static char *tests[] = {
      "debug","debugfs","debug=1","debug systemd.debug","debug 123 debug 456 debug","debu",NULL
      };
   char r[]="debug",*s;
   int t;
   for(t=0;(s=tests[t])!=NULL;t++) {
     int len;
     s = strdup(s);  // make writable copy
     printf("string=\"%s\"\n",s);
     undebug(s);
     printf("result=\"%s\"\n\n",s);
     free(s);
   }
}
*/

