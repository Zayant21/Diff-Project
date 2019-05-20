//
//  main.c diff
//  Created by William McCarthy on 4/29/19.
//  Copyright © 2019 William McCarthy. All rights reserved.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include "diff.h"

int main(int argc, const char* argv[]) {
  int foundmatch = 0;

  init_options_files(--argc, ++argv);
  para* p = para_first(strings1, count1);
  para* q = para_first(strings2, count2);

  para* qlast = q;
  while (p != NULL) {
    qlast = q;
    foundmatch = 0;
    while (q != NULL && (foundmatch = para_equal(p, q)) == 0) {
      q = para_next(q);
    }
    q = qlast;

    if (foundmatch) {
      while ((foundmatch = para_equal(p, q)) == 0) {
        optionsright(p, q);
        q = para_next(q);
        qlast = q;
      }
      optionssame(p, q);
      p = para_next(p);
      q = para_next(q);
    } else {
      optionsleft(p, q);
      p = para_next(p);
    }
  }
  while (q != NULL) {
    optionsright(p, q);
    q = para_next(q);
  }
  printf("\n");
  return 0;
}

para* para_make(char* base[], int filesize, int start,
                int stop) {  // Func to make a new paragraph
  para* p = (para*)malloc(sizeof(para));
  p->base = base;          // start of para
  p->filesize = filesize;  // size of para
  p->start = start;
  p->stop = stop;  // end of para
  return p;
}

para* para_first(char* base[],
                 int size) {  // func to mark the begining of paragraph
  para* p = para_make(base, size, 0, -1);
  return para_next(p);
}

void para_destroy(para* p) { free(p); }  // func to delete a para

para* para_next(para* p) {  // func to put the next para into buffer
  if (p == NULL || p->stop == p->filesize) {
    return NULL;
  }
  int i;
  para* pnew = para_make(p->base, p->filesize, p->stop + 1, p->stop + 1);
  for (i = pnew->start; i < p->filesize && strcmp(p->base[i], "\n") != 0; ++i) {
  }
  pnew->stop = i;

  if (pnew->start >= p->filesize) {
    free(pnew);
    pnew = NULL;
  }
  return pnew;
}
size_t para_filesize(para* p) {
  return p == NULL ? 0 : p->filesize;
}  // fun to return the size of para

size_t para_size(para* p) {  // func to get the size of para
  return p == NULL || p->stop < p->start ? 0 : p->stop - p->start + 1;
}

char** para_base(para* p) {
  return p->base;
}  // func to return the base of para

char* para_info(para* p) {  // para info
  static char buf[BUFLEN];  // static for a reason
  snprintf(buf, sizeof(buf), "base: %p, filesize: %d, start: %d, stop: %d\n",
           p->base, p->filesize, p->start, p->stop);
  return buf;  // buf MUST be static
}

int para_equal(para* p, para* q) {  // func to see if the p and q para are
                                    // equal, if yes return 1 else return 0
  if (p == NULL || q == NULL) {
    return 0;
  }
  if (para_size(p) != para_size(q)) {
    return 0;
  }
  if (p->start >= p->filesize || q->start >= q->filesize) {
    return 0;
  }
  int i = p->start, j = q->start, equal = 0;
  while ((equal = strcmp(p->base[i], q->base[i])) == 0) {
    ++i;
    ++j;
  }
  return 1;
}

void para_print(para* p, void (*fp)(const char*)) {  // func to print a para
  if (p == NULL) {
    return;
  }
  for (int i = p->start; i <= p->stop && i != p->filesize; ++i) {
    fp(p->base[i]);
  }
}

void para_printboth(
    para* p, para* q,
    void (*fp)(
        const char*,
        const char*)) {  // func to print two different para's side by side
  int i, j;
  if (p == NULL || q == NULL) {
    return;
  }
  for (i = p->start, j = q->start;
       (i <= p->stop && i != p->filesize) && (j <= q->stop && j != q->filesize);
       ++i, j++) {
    fp(p->base[i], q->base[j]);
  }
}

void para_printfile(char* base[], int count,
                    void (*fp)(const char*)) {  // func to print the whole file
  para* p = para_first(base, count);
  while (p != NULL) {
    para_print(p, fp);
    p = para_next(p);
  }
}

char* yesorno(int condition) { return condition == 0 ? "no" : "YES"; }

FILE* openfile(const char* filename,
               const char* openflags) {  // func to open a file of strings
  FILE* f;
  if ((f = fopen(filename, openflags)) == NULL) {
    printf("can't open '%s'\n", filename);
    exit(1);
  }
  return f;
}

void printleft(const char* left) {  // fun to print the left side of diff ,
                                    // meaning the para's in the left file.
  char buf[BUFLEN];
  strcpy(buf, left);
  int j = 0, len = (int)strlen(buf) - 1;
  for (j = 0; j <= 48 - len; ++j) {
    buf[len + j] = ' ';
  }
  buf[len + j++] = '<';
  buf[len + j++] = '\0';
  printf("%s\n", buf);
}

void printright(const char* right) {  // funcs to print the right side of diff,
                                      // meaning para's in right file.
  if (right == NULL) {
    return;
  }
  printf("%50s %s", ">", right);
}

void printboth(const char* left_right) {                                        // func to print both sides left right of the diff command, meaning printing left and rights file para's side by side.
  char buf[BUFLEN];
  size_t len = strlen(left_right);
  if (len > 0) {
    strncpy(buf, left_right, len);
  }
  buf[len - 1] = '\0';  // stripping the \n cause there are 2 of "\n".
  printf("%-50s %s", buf, left_right);
}

void printsidebyside(const char* left, const char* right) {
  char buf[BUFLEN];
  size_t len = strlen(left);
  if (len > 0) {
    strncpy(buf, left, len);
  }
  buf[len - 1] = '\0';
  if (strcmp(left, right) ==
      0) {  // to see if the side by side paras has some uncommon lines
    printf("%-48s %s %s", buf, " ", right);
  }  // if no print nothing
  else {
    printf("%-48s %s %s", buf, "|", right);  // if yes print | between them
  }
}

void printleftcolumn(const char* left, const char* right) {
  char buf[BUFLEN];
  size_t len = strlen(left);
  if (len > 0) {
    strncpy(buf, left, len);
  }
  buf[len - 1] = '\0';
  if (strcmp(left, right) == 0) {
    printf("%-48s %s", buf, "(\n");
  } else {
    printf("%-48s %s %s", buf, "|", right);
  }
}

void printsuppresscommon(const char* left, const char* right) {
  char buf[BUFLEN];
  size_t len = strlen(left);
  if (len > 0) {
    strncpy(buf, left, len);
  }
  buf[len - 1] = '\0';

  if (strcmp(left, right) != 0) {
    printf("%-48s %s %s", buf, "|", right);
  }
}

void printnormalleft(const char* left) {
  if (left == NULL) {
    return;
  }
  printf("< %s", left);
}

void printnormalsame(const char* left, const char* right) {
  if (strcmp(left, right) != 0) {
    printf("< %s", right);
    if (strchr(right, '\n') && strchr(left, '\n')) {
    } else {
      printf("/ No newlines at the end of file\n");
    }
    printf("---\n");
    printf("> %s\n", left);
  }
}

void printnormalright(const char* right) {
  if (right == NULL) {
    return;
  }
  printf("> %s", right);
}

void printcontextright(const char* right) {
  if (right == NULL) {
    return;
  }
  printf("+ %s", right);
}

void printcontextsame(const char* left, const char* right) {
  if (strcmp(left, right) ==
      0) {  // to see if the side by side paras has some uncommon lines
    printf("%s", left);
  } else {
    printf("! %s", left);
    if (strchr(right, '\n') && strchr(left, '\n')) {
    } else {
      printf("\n/ No newlines at the end of file\n");
    }
  }
}  // if no print nothing

void printunifiedsame(const char* left, const char* right){
  if (strcmp(left,right) == 0)
  {
    printf("%s",left);
  } else { printf("- %s", left);
  if (strchr(right, '\n') && strchr(left, '\n')) {
  } else {
    printf("\n/ No newlines at the end of file\n"); }
   printf("+ %s", right);
}
}

void printconextleft(const char* left) {
  if (left == NULL) {
    return;
  }
  printf("- %s", left);
}

int filelenght(FILE* fp) {
  int filesize = 0;
  char c;
  if (fp == NULL) return 0;
  for (c = getc(fp); c != EOF; c = getc(fp))
    if (c == '\n') filesize = filesize + 1;

  return filesize;
}

void setoption(const char* arg, const char* s, const char* t, int* value) {
  if ((strcmp(arg, s) == 0) || ((t != NULL && strcmp(arg, t) == 0))) {
    *value = 1;
  }
}

void diff_output_conflict_error(void) {
  fprintf(stderr, "diff: conflicting output style options\n");
  fprintf(stderr, "diff: Try `diff --help' for more information.)\n");
  exit(CONFLICTING_OUTPUT_OPTIONS);
}

void version(void) {
  printf("\n\n\ndiff (CSUF diffutils) 1.0.0\n");
  printf("Copyright (C) 2014 CSUF\n");
  printf(
      "This program comes with NO WARRANTY, to the extent permitted by law.\n");
  printf("You may redistribute copies of this program\n");
  printf("under the terms of the GNU General Public License.\n");
  printf(
      "For more information about these matters, see the file named "
      "COPYING.\n");
  printf("Written by William McCarthy, Tony Stark, and Dr. Steven Strange\n");
}


int filecheck(FILE* fp1, FILE* fp2) {
  char file1, file2;

  if (fp1 == NULL) {
  } else if (fp2 == NULL) {
  } else {
    do {
      file1 = getc(fp1);
      file2 = getc(fp2);
    } while ((file1 != EOF) && (file2 != EOF) && (file1 == file2));
    if (file1 == file2)
      match = 0;
    else if (file1 != file2)
      match = 1;
  }
  return (0);
}


void loadfiles(const char* filename1, const char* filename2) {
  memset(buf, 0, sizeof(buf));
  memset(strings1, 0, sizeof(strings1));
  memset(strings2, 0, sizeof(strings2));

  FILE* fin1 = openfile(filename1, "r");  // to open and read file 1
  FILE* fin2 = openfile(filename2, "r");

  FILE* fin3 = openfile(
      filename1, "r");  // only to compare files cause they were being changed
  FILE* fin4 = openfile(filename2, "r");
  // to open and read file 2
  filecheck(fin3, fin4);
  // number of lines in 2nd file

  while (!feof(fin1) && fgets(buf, BUFLEN, fin1) != NULL) {
    strings1[count1++] = strdup(buf);
  }
  fclose(fin1);
  while (!feof(fin2) && fgets(buf, BUFLEN, fin2) != NULL) {
    strings2[count2++] = strdup(buf);
  }
  fclose(fin2);
}

void init_options_files(int argc, const char* argv[]) {
  struct stat time;
  int cnt = 0;
  int i = 0;
  const char* files[2] = {NULL, NULL};
  while (argc-- > 0) {
    const char* arg = *argv;
    setoption(arg, "-v", "--version", &showversion);
    setoption(arg, "-q", "--brief", &showbrief);
    setoption(arg, "-i", "--ignore-case", &ignorecase);
    setoption(arg, "-s", "--report-identical-files", &report_identical);
    setoption(arg, "--normal", NULL, &diffnormal);
    setoption(arg, "-y", "--side-by-side", &showsidebyside);
    setoption(arg, "--left-column", NULL, &showleftcolumn);
    setoption(arg, "--suppress-common-lines", NULL, &suppresscommon);
    setoption(arg, "-c", "--context", &showcontext);
    setoption(arg, "-u", "showunified", &showunified);
    if (arg[0] != '-') {
      if (cnt == 2) {
        fprintf(stderr,
                "apologies, this version of diff only handles two files\n");
        fprintf(stderr, "Usage: ./diff [options] file1 file2\n");
        exit(TOOMANYFILES_ERROR);
      } else {
        files[cnt++] = arg;
      }
    }

    ++argv;  // DEBUG only;  move increment up to top of switch at release
  }

  if (!showcontext && !showunified && !showsidebyside && !showleftcolumn &&
      !showbrief && !report_identical && !suppresscommon) {
    diffnormal = 1;
  }

  if (showversion) {
    version();
    exit(0);
  }

  while (i < 2) {
    stat(files[i], &time);
    struct tm* ti = localtime(&time.st_mtime);
    if ((showcontext) || (showunified)) {
      if (i ==0 && (showunified)) printf("--- ");
      if (i ==0 && (showcontext)) printf("*** ");
      if (i == 1) printf("+++ ");
      printf("%-10s %4d-%02d-%02d %02d:%02d:%012.9f\n", files[i],
             ti->tm_year + 1900, ti->tm_mon + 1, ti->tm_mday, ti->tm_hour,
             ti->tm_min, (float)ti->tm_sec);

      i++;
    }
  }

  if (report_identical) {
    if (match == 1) {
      printf("Files %s and %s are same", files[0], files[1]);
    }
  }

  if (showbrief) {
    if (match == 0) {
      printf("Files %s and %s differ", files[0], files[1]);
    }
  }

  if (((showsidebyside || showleftcolumn) &&
       (diffnormal || showcontext || showunified)) ||
      (showcontext && showunified) ||
      (diffnormal && (showcontext || showunified))) {
    diff_output_conflict_error();
  }

  loadfiles(files[0], files[1]);
}

void optionsleft(para* p, para* q)  // ALL THE CASES FOR THE DIFFERENT PRINTINGS
{
  if (diffnormal) {
    para_print(p, printnormalleft);
  }
  if ((showsidebyside) && ((!showleftcolumn) && (!suppresscommon))) {
    para_print(p, printleft);
  }
  if ((showsidebyside) && (showleftcolumn)) {
    para_print(p, printleft);
  }
  if ((showsidebyside) && (suppresscommon)) {
    para_print(p, printleft);
  }
  if (showcontext) {
    para_print(p, printconextleft);
  }
  if (showunified) {
    para_print(p,printconextleft);
  }
}
void optionssame(para* p, para* q) {
  if (diffnormal) {
    para_printboth(p, q, printnormalsame);
  }
  if ((showsidebyside) && ((!showleftcolumn) && (!suppresscommon))) {
    para_printboth(p, q, printsidebyside);
  }
  if ((showsidebyside) && (showleftcolumn)) {
    para_printboth(p, q, printleftcolumn);
  }
  if ((showsidebyside) && (suppresscommon)) {
    para_printboth(p, q, printsuppresscommon);
  }
  if (showcontext) {
    para_printboth(p, q, printcontextsame);
  }
  if (showunified) {
  para_printboth(p,q,printunifiedsame); }
}
void optionsright(para* p, para* q) {
  if (diffnormal) {
    para_print(q, printnormalright);
  }
  if ((showsidebyside) && ((!showleftcolumn) && (!suppresscommon))) {
    para_print(q, printright);
  }
  if ((showsidebyside) && (showleftcolumn)) {
    para_print(q, printright);
  }
  if ((showsidebyside) && (suppresscommon)) {
    para_print(q, printright);
  }
  if (showcontext) {
    para_print(q, printcontextright);
  }
  if (showunified) {
    para_print(q,printcontextright);
  }
}
