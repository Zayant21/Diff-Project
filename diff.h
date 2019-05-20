#define STAT_ERROR 5
#define ARGC_ERROR 1
#define BUFLEN 256
#define MAXSTRINGS 1024
#define ARGC_ERROR 1
#define TOOMANYFILES_ERROR 2
#define CONFLICTING_OUTPUT_OPTIONS 3
#define MAXPARAS 4096

typedef struct para para;
struct para {
  char** base;
  int filesize;
  int start;
  int stop;
};

para* para_make(char* base[], int size, int start, int stop);
para* para_first(char* base[], int size);
para* para_next(para* p);
size_t para_filesize(para* p);
size_t para_size(para* p);
char** para_base(para* p);
char* para_info(para* p);
int   para_equal(para* p, para* q);
void para_print(para* p, void (*fp)(const char*));
char* yesorno(int condition);
void printleft(const char* left);
void printright(const char* right);
void printboth(const char* left_right);
void printsidebyside(const char* left, const char* right);
void printleftcolumn(const char* left, const char* right);
void printsuppresscommon(const char* left, const char* right);
void printnormalleft(const char* left);
void printnormalsame(const char* left, const char* right);
void printnormalright(const char* right);
void printcontextright(const char *right);
void printcontextsame(const char *left, const char *right);
void printconextleft(const char *left);
void printunifiedsame(const char* left, const char* right);
int filelenght(FILE *fp);
void setoption(const char* arg, const char* s, const char* t, int* value);
void diff_output_conflict_error(void);
void version(void);
int filecheck( FILE *fp1, FILE *fp2);
void loadfiles(const char *filename1, const char *filename2);
void init_options_files(int argc, const char* argv[]);
void optionsleft(para* p, para* q);
void optionssame(para* p, para* q);
void optionsright(para* p, para* q);




char buf[BUFLEN];
char *strings1[MAXSTRINGS], *strings2[MAXSTRINGS];
int showversion = 0, showbrief = 0, ignorecase = 0, report_identical = 0, showsidebyside = 0;
int showleftcolumn = 0, showunified = 0, showcontext = 0, suppresscommon = 0, diffnormal = 0;
int count1 = 0, count2 = 0, value =0, file1lenght =0, file2lenght =0,match;
