// Copyright: 2014 (C) Alexander Vershilov
// License:   MIT / GPL-2
//
// Add #define _GNU_SOURCE in order to use GNU implementation
// of the strchrnull.

#if __linux__
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char * strmatch(char *p, const char *n) {
	for (;*n != '\0';n++,p++) {
		if (*p != *n) return NULL;
	}
	return p;
}

long int backcmp(void *x,void *y, size_t size) {
    if (*((char*)x - 1) != ' ') return 1;
    long int t = 0;
	for (; size >= sizeof(long int); 
		size -= sizeof(long int),
		x += sizeof(long int),
		y += sizeof(long int)) {
        t |= *(long int *)x ^ *(long int*)y;
    }
    if (t) return t;
	for (; size >= sizeof(int); 
		size -= sizeof(int),
		x += sizeof(int),
		y += sizeof(int)) {
        t |= *(int *)x ^ *(int*)y;
	}
    if (t) return t;
	for (; size > 0; size--, x++, y++) {
        t |= *(char *)x ^ *(char *)y;
	}
    if (t) return t;
    return t;
}


void user_memmove(void *dest, void *source, size_t size) {
	for (; size >= sizeof(long int); 
		size -= sizeof(long int),
		dest += sizeof(long int),
		source += sizeof(long int)) {
	*(long int *)dest = *(long int *)source;
    }
	for (; size >= sizeof(int); 
		size -= sizeof(int),
		dest += sizeof(int),
		source += sizeof(int)) {
	*(int *)dest = *(int *)source;
	}
	for (; size > 0; size--, dest++, source++) {
		*(char *)dest = *(char *)source;
	}
}

#ifndef _GNU_SOURCE 
char * strchrnul(char *x, char needle) {
    for (;*x != '\0'; x++) {
        if (*x == needle) return x;
    }
    return x;
}
#endif

char *
undebugq(char *in, char *needle)
{
	char *p = in;
	char *eoi = NULL, // pointer to the end of input
		*pt = NULL;  // temporary pointer
	do {
		if ((pt = strmatch(p, needle)) != NULL) {
			if (*pt == ' ') {
				if (!eoi)
					eoi = pt + strlen(pt);
				user_memmove(p, pt + 1, eoi - pt + 1);	// we need to copy \0
				eoi -= pt - p;
				continue;
			} 
			if (*pt == '\0') {
				*p = '\0';
				return in;
			}
			p = pt;
		}
		p = strchr(p, ' ');
	} while (p && p++);
	return in;
}

char *
undebugq2(char *in, char *needle)
{
	char *p = in;
	char *pt = NULL;  // temporary pointer
	do {
		if ((pt = strmatch(p, needle)) != NULL) {
			if (*pt == ' ') {
                char *ip   = pt + 1;
                char *ipt  = NULL;
                do {
                  if ((ipt = strmatch(ip, needle)) != NULL) {
                    if (*ipt == ' ') {
                        user_memmove(p, pt+1, ip-pt-1);
                        p += ip-pt-1;
                        ip = ipt+1;
                        pt = ipt;
                    }
                    if (*ipt == '\0') {
                        if (ip - pt - 1) {
                          user_memmove(p, pt+1, ip-pt-1);
                          p += ip-pt-2;
                        }
                        *p = '\0';
                        return in;
                    }
                    ip = ipt;
                  }
                  ip   = strchrnul(ip, ' ');
                  if (!(*ip)) {
                      user_memmove(p, pt+1, ip-pt);
                      return in;
                  } 
                  ip++;
                } while (*ip);
                return in;
			} 
			if (*pt == '\0') {
				*p = '\0';
				return in;
			}
			p = pt;
		}
		p = strchr(p, ' ');
	} while (p && p++);
	return in;
}

char *
undebugq3(char *in, char *needle)
{
	char *p = in;
	char *pt = NULL;// temporary pointer
	do {
		if ((pt = strmatch(p, needle)) != NULL) {
			if (*pt == ' ') {
                char *ip   = pt + 1;
                char *ipt  = NULL;
                size_t nsize = pt - p;
                do {
                  ipt = strchrnul(ip,' ');
                  if (*ipt == ' ') {
                    if ( backcmp(ipt - nsize, needle, nsize) ) {
                      ip = ipt;
                      continue;
                    } else {
                      user_memmove(p, pt+1, ipt-pt-1-nsize);
                      p += ipt-pt-1-nsize;
                      ip = ipt+1;
                      pt = ipt;
                      continue;
                    }
                  }
                  if (*ipt == '\0') {
                     if ( backcmp(ipt - nsize, needle, nsize) ) {
                        user_memmove(p, pt+1, ipt-pt);
                     } else {
                        user_memmove(p, pt+1, ipt - pt - nsize - 1);
                        p += ipt - pt - nsize - 1;
                        *p = '\0';
                     }
                     return in;
                  }
                } while(*(ip++));
                return in;
			} 
			if (*pt == '\0') {
				*p = '\0';
				return in;
			}
			p = pt;
		}
		p = strchr(p, ' ');
	} while (p && p++);
	return in;
}

char *
undebugq_ker(char *in, char *needle)
{
	char *p = in;
	char *eoi = NULL, // pointer to the end of input
		*pt = NULL;  // temporary pointer
	do {
		if ((pt = strmatch(p, needle)) != NULL) {
			if (*pt == ' ') {
				if (!eoi)
					eoi = pt + strlen(pt);
				memmove(p, pt + 1, eoi - pt + 1);	// we need to copy \0
				eoi -= pt - p;
				continue;
			} 
			if (*pt == '\0') {
				*p = '\0';
				return in;
			}
			p = pt;
		}
		p = strchr(p, ' ');
	} while (p && p++);
	return in;
}


/*
main() {
    char *s = strdup("debug debug");
//    char *s = strdup("debug 123 debug 456");
//    char *s = strdup("debug debugfs debug debug=1 systemd.debug debug");
    printf(":%s",undebugq3(s,"debug"));
}
*/
