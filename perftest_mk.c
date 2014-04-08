#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <regex.h>

#ifndef REPEATS
#define REPEATS 40000
#endif

struct Pretender {	
	char *nick;		// псевдоним
	char *fname;	// имя функции
	char *(*f)(char *,char *);	// реализованная функция
};
extern char *nop(char *,char *);
extern char *cutout(char *,char *);
extern char *cutout_orig(char *,char *);
extern char *undebug(char *,char *);
extern char *split(char *,char *);
extern char *wipeout(char *,char *);
extern char *delsubstr(char *,char *);
extern char *process_wrapper(char *,char *);
extern char *strdel_wrapper(char *,char *);
extern char *strrdel_wrapper(char *,char *);
extern char *remove_string(char *,char *);
extern char *wordstrips(char *,char *);
extern char *cut(char *,char *);
extern char *cut2(char *,char *);
extern char *strremove_wrapper(char *,char *);
extern char *remove_word_wrapper(char *,char *);
extern char *undebug_wota_wrapper(char *,char *);
extern char *strcut_wrapper(char *,char *);
extern char *anon_strcut(char *,char *);
extern char *anon_wipedebug(char *,char *);
extern char *str_drop_str(char *,char *);
extern char *str_mask_str(char *,char *);
extern char *undebugq(char *,char *);
extern char *undebugq2(char *,char *);
extern char *undebugq3(char *,char *);
extern char *undebugq_ker(char *,char *);
extern char *strcutm(char *,char *);
extern char *carb_wrapper(char *,char *);
extern char *strreplace(char *,char *);
extern char *undebugit2_wrapper(char *,char *);

struct Pretender pretenders[]={
//	{ "nop", "nop", nop },
//	{ "beastie", "cutout", cutout },
//	{ "beastie", "cutout_orig", cutout_orig },
//	{ "beastie", "undebug", undebug },
//	{ "beastie", "split", split },
//	{ "beastie", "wipeout", wipeout },
//	{ "Eddy_Em", "delsubstr", delsubstr },
	{ "Gvidon", "process", process_wrapper },
	{ "KennyMinigun", "strdel", strdel_wrapper },
	{ "nokachi", "remove", remove_string },
	{ "qulinxao", "wordstrings", wordstrips },
//	{ "true_admin", "cut", cut },
//	{ "true_admin", "cut2", cut2 },
//	{ "wota", "strremove", strremove_wrapper },
//	{ "wota", "remove_word", remove_word_wrapper },
	{ "wota", "undebug_wota", undebug_wota_wrapper },
//	{ "anonymous", "strcut", strcut_wrapper },
	{ "anonymous", "anon_strcut", anon_strcut},
	{ "anonymous", "anon_wipedebug", anon_wipedebug},
	{ "puzan", "str_drop_str", str_drop_str },
	{ "puzan", "str_mask_str", str_mask_str },
	{ "qnikst", "undebugq uspace", undebugq },
	{ "qnikst", "undebugq2", undebugq2 },
	{ "qnikst", "undebugq3", undebugq3 },
	{ "qnikst", "undebugq", undebugq_ker },
//	{ "mix-mix", "strcut", strcutm },
	{ "Carb", "debugdel", carb_wrapper },
//	{ "Carb", "strreplace", strreplace },
	{ "MKuznetsov", "undebugit2", undebugit2_wrapper },	
	{ 0 }
};

struct Test {
	char *p1;	// первый аргумент передаваемый в функцию (строка для поиска/замены)
	char *p2;	// второй аргумент передаваемый в функцию (шаблон "debug")
	int (*check)(char *p1,char *p2,char *ret,char *arg);	// функция для проверки результата
	char *check_arg;	// дополнительный аргумент для неё
};

// набор тестов 
int no_has_regex(char *p1,char *p2,char *r,char *regular);	// =1 если regular не подходит
int no_has_debug(char *p1,char *p2,char *r,char *ignore);	// =1 если не соотв. "(^| )debug( |$)"
int empty_line(char *p1,char *p2,char *r,char *ignore);		// =1 если строка пуста или содержит только пробелы
int strlen_eq(char *p1,char *p2,char *r,char *decimal);		// =1 если длина равна decimal
int cmp_words(char *p1,char *p2,char *r,char *words);		// =1 если r==words (пословное сравнение)

struct Test testcase[]={
	// empty string
	{ "","debug",empty_line,NULL},
	{ " ","debug",empty_line,NULL},
	{ "     ","debug",empty_line,NULL},
	// hit
	{ "debug", "debug", empty_line,NULL },
	// space before
	{ " debug","debug",empty_line,NULL },
	// space after
	{ "debug ","debug",empty_line,NULL },
	// arrounded in spaces
	{ " debug ","debug",empty_line,NULL },
	// multiple times
	{ "debug debug debug debug","debug",empty_line,NULL },
	// multiple times arrounded spaces
	{ " debug debug debug debug ","debug",empty_line,NULL },
	// prefixes	
    { "debu","debug",cmp_words,"debu" },
    { " debu ","debug",cmp_words,"debu" },
    { "debu ","debug",cmp_words,"debu" },
    { " debu ","debug",cmp_words,"debu" },
	// suffixes
    { "ebug","debug",cmp_words,"ebug" },
    { "ebug ","debug",cmp_words,"ebug" },
    { " ebug","debug",cmp_words,"ebug" },
    { " ebug ","debug",cmp_words,"ebug" },
	/**************************/
	/*** FROM REAL WORLD ;-) ***/	
	{ "debugfs", "debug",cmp_words,"debugfs" },
	{ "debug=1", "debug",cmp_words,"debug=1" },
	{ "systemd.debug", "debug",cmp_words,"systemd.debug"},
	{ "debug 123 debug 456","debug",cmp_words,"123 456"},
	{ "debug debugfs debug debug=1 systemd.debug debug", "debug",cmp_words,"debugfs debug=1 systemd.debug"},
    // end of the string
	{ "BOOT_IMAGE=/debug/vmlinuz-3.2.0-debug-amd64 debug=UUID=42debug5-6ee1-464c-bc41-debug42debug ro debug","debug",
		cmp_words,"BOOT_IMAGE=/debug/vmlinuz-3.2.0-debug-amd64 debug=UUID=42debug5-6ee1-464c-bc41-debug42debug ro" },
    // middle of the string
	{ "BOOT_IMAGE=/debug/vmlinuz-3.2.0-debug-amd64 debug dolvm debug=UUID=42debug5-6ee1-464c-bc41-debug42debug debug ro","debug",
		cmp_words,"BOOT_IMAGE=/debug/vmlinuz-3.2.0-debug-amd64 dolvm debug=UUID=42debug5-6ee1-464c-bc41-debug42debug ro"},
    // beg of the string
	{ "BOOT_IMAGE=debug /debug/vmlinuz-3.2.0-debug-amd64 debug dolvm debug=UUID=42debug5-6ee1-464c-bc41-debug42debug debug ro","debug",
		cmp_words,"BOOT_IMAGE=debug /debug/vmlinuz-3.2.0-debug-amd64 dolvm debug=UUID=42debug5-6ee1-464c-bc41-debug42debug ro"},
    // multiple times
	{ "BOOT_IMAGE=/debug/vmlinuz-3.2.0-debug-amd64 debug dolvm debug debug=UUID=42debug5-6ee1-464c-bc41-debug42debug debug ro","debug",
		cmp_words,"BOOT_IMAGE=/debug/vmlinuz-3.2.0-debug-amd64 dolvm debug=UUID=42debug5-6ee1-464c-bc41-debug42debug ro"},
    // long needle middle
/*	{ "BOOT_IMAGE=/debug/vmlinuz-3.2.0-debug-amd64 debug=UUID=42debug5-6ee1-464c-bc41-debug42debug ro debug", "debug=UUID=42debug5-6ee1-464c-bc41-debug42debug",no_has_debug,NULL},
    // long needle end
	{ "BOOT_IMAGE=/debug/vmlinuz-3.2.0-debug-amd64 ro debug debug=UUID=42debug5-6ee1-464c-bc41-debug42debug", "debug=UUID=42debug5-6ee1-464c-bc41-debug42debug",no_has_debug,NULL},
    // long needle beging
	{ "BOOT_IMAGE=/debug/vmlinuz-3.2.0-debug-amd64 debug=UUID=42debug5-6ee1-464c-bc41-debug42debug ro debug","BOOT_IMAGE=/debug/vmlinuz-3.2.0-debug-amd64",no_has_debug,NULL},
*/	
    // no remove short
	{ "BOOT_IMAGE=/debug/vmlinuz-3.2.0-debug-amd64 debug=UUID=42debug5-6ee1-464c-bc41-debug42debug ro nodebuq", "debug",
		cmp_words,"BOOT_IMAGE=/debug/vmlinuz-3.2.0-debug-amd64 debug=UUID=42debug5-6ee1-464c-bc41-debug42debug ro nodebuq"},
/*	
    // no remove long
	{ "BOOT_IMAGE=/debug/vmlinuz-3.2.0-debug-amd64 debug=UUID=42debug5-6ee1-464c-bc41-debug42debuq ro debug","debug=UUID=42debug5-6ee1-464c-bc41-debug42debug",no_has_debug,NULL},
*/	
	{ 0 }
};

int
diff_timeval(struct timeval *t1, struct timeval *t2)
{
    return (((t1->tv_sec - t2->tv_sec) * 1000L) + 
            (t1->tv_usec - t2->tv_usec))/1000L;
}

int
no_has_regex(char *p1,char *p2,char *r,char *regular) {
	int ret;
	regex_t expr;
	regmatch_t match[16];
	if (regcomp(&expr,regular,REG_EXTENDED)!=0) {
		fprintf(stderr,"regcomp(%s)\n",regular);
		exit(EXIT_FAILURE);
	}
	ret=regexec(&expr,r,16,match,0);
	regfree(&expr);
	return (ret==REG_NOMATCH);
}
int
no_has_debug(char *p1,char *p2,char *r,char *regular) {
	return no_has_regex(p1,p2,r,"(^| )debug( |$)");
}
int
strlen_eq(char *p1,char *p2,char *r,char *length) {
	long len;
	len=strtol(length,NULL,0);
	if (errno) {
		fprintf(stderr,"strtol(%s)\n",length);
		exit(1);
	}
	return (strlen(r)==len);
}
int empty_line(char *p1,char *p2,char *r,char *s) {
	if (r==NULL) {
		fprintf(stderr,"r=NULL\n");
		exit(1);
	}
	while(*r==' ')
		r++;
	return (*r==0);
}
/* сравнить r и s по словам разделённым пробелами */
int 
cmp_words(char *p1,char *p2,char *r,char *s) {
//	printf(" 000 p1=%s s=%s r=%s\n",p1,s,r);
	do {
		// leading sp
		while(*r==' ') r++;
		while(*s==' ') s++;
		// words
		while(*r==*s) {
			if (*r==0) return 1;
			if (*r==' ') break;
			r++,s++;
			if ((!r || *r==' ') && *s && *s!=' ') {
				return 0; //terminated r but not s
			}
			if ((!s || *s==' ') && *r && *r!=' ') {
				return 0; //terminated s but not r
			}
		}
		// trailing sp
		while(*r==' ') r++;
		while(*s==' ') s++;
	} while(*r==*s && *r);
	return (*s == *r);
}
inline int
invoke_test(
	char *(*f)(char *,char *),			// тестируемая функция
	char *p1,char *p2,					// её аргументы
	int (*check)(char *,char *,char *,char *),	// функция для проверки результата
	char *check_arg
){
	struct TestData {
		char *result;	// для сохранение результата
		uint16_t num;	// номер прогона
		char text[];	// тестовая строка
	} **data,*d;
	struct timeval start,finish;
	int delta;
	int len;	// длина строки для тестирования
	uint16_t t;
	/* готовим данные для тестирования */
	len=strlen(p1);
	data=calloc(sizeof(struct TestData *),REPEATS);
	if (data==NULL) {
		fprintf(stderr,"malloc() #1\n");
		exit(EXIT_FAILURE);
	}
	for(t=0;t<REPEATS;t++,d++) {
		d=data[t]=malloc(sizeof(struct TestData)+len+1+sizeof(uint16_t));
		if (d==NULL) {
			fprintf(stderr,"malloc() #2\n");
			exit(EXIT_FAILURE);
		}
		d->result=NULL;
		d->num=t;
		memcpy(d->text,p1,len);
		d->text[len]=0;
		*(uint16_t *)(d->text+len+1)=t;
	}
	/* исполнение тестов */
	// засекаем время начала
	if (gettimeofday(&start,NULL)!=0) {
		int err;
		err=errno;
		fprintf(stderr,"gettimeofday() %d %s\n",err,strerror(err));
		exit(1);
	}
	// выполняем прогоны
	for(t=0;t<REPEATS;t++) {
		d=data[t];
		d->result=f(d->text,p2);
	}
	// засекаем время завершения
	gettimeofday(&finish,NULL);
	delta=diff_timeval(&finish,&start);
	/* проверка тестов */
	for(t=0;t<REPEATS;t++) {
		d=data[t];
		// проверка на "битость памяти"
		if (d->num!=t) {
			fprintf(stderr,"mem crash BOL!\n"); 
			break;	// запись перед началом строки
		}
		if (*(uint16_t *)(d->text+len+1)!=t) {
			fprintf(stderr,"mem crash EOL!\n"); 
			break; // запись за концом строки
		}
		if (d->result==NULL) {
			fprintf(stderr,"NULL returned\n"); 
			break; // некорректный возврат
		}
		if (t==0) {
			// для самого первого полученного результата - использовать функцию проверки
			if (!check(p1,p2,d->result,check_arg)){
				fprintf(stderr,"   failed f(\"%s\",\"%s\") = \"%s\"\n",p1,p2,d->result); 
				break;
			}
		} else {
			// остальные - сравниваем с первым
			if (memcmp(d->text,data[0]->text,len+1)!=0) {
				fprintf(stderr,"   pass results not equal\n");
				break;
			}
		}
		if (d->result<d->text-16 || d->result>d->text+len+17) {
			// видимо строка выделенна динамически - освободить
			free(d->result);
		}
	}
	if (t!=REPEATS) {
		// какой-то прогон обломался
		// возможно бита память - чтоб не поймать segfault на free не освобождаем память
		delta=-1;
	}
	for(t=0;t<REPEATS;t++)
		free(data[t]);
	free(data);
	// вернуть кол-во микросекунд затраченных на все тесты
	return delta;
}

void perftest(struct Pretender *person,struct Test *test) {
	int **matrix;
	int t,p;
	// кол-во претендентов
	for(p=0;person[p].nick;) {
		p++;
	}
	// кол-во тестов
	for(t=0;test[t].p1;) {
		t++;
	}
	// матрица результатов
	matrix=calloc(sizeof(int *),p);
	if (matrix==NULL) {
		fprintf(stderr,"matrix=calloc()\n");
		exit(EXIT_FAILURE);
	}
	for(;p>=0;p--) {
		matrix[p]=calloc(sizeof(int),t);
		if (matrix[p]==NULL) {
			fprintf(stderr,"matrix[p]=calloc()\n");
			exit(EXIT_FAILURE);
		}
	}
	// исполнения тестов	
	for(p=0;person[p].nick;p++) {
		fprintf(stderr,"=== %s %s ===",person[p].nick,person[p].fname);
		for(t=0;test[t].p1;t++) {
			fprintf(stderr,"%d ",t+1);
			matrix[p][t]=invoke_test(person[p].f,test[t].p1,test[t].p2,test[t].check,test[t].check_arg);
		}
		fprintf(stderr,"\n");
	}
	// вывод отчёта
	for(p=0;person[p].nick;p++) {
		unsigned long complete;
		unsigned long elapsed;
		printf("%s %s ",person[p].nick,person[p].fname);
		complete=0;
		elapsed=0;
		for(t=0;test[t].p1;t++) {
			if (matrix[p][t]!=-1) {
				complete++;
				elapsed+=matrix[p][t];
			}
		}
		printf("\t %lu %.3f \t",complete,(double)elapsed/1000);
		for(t=0;test[t].p1;t++) {
			if (t!=0) putchar(' ');
			if (matrix[p][t]==-1) {
				printf("fail");
			} else {
				printf("%.3f",(double)matrix[p][t]/1000);
			}
		}
		putchar('\n');
		free(matrix[p]);
	}
	free(matrix);
}
int main(int argc,char **argv) {
	perftest(pretenders,testcase);
	return 0;
}
