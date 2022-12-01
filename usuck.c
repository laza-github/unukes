#ifdef linux
#include <dirent.h>
#include <fcntl.h>
#include <linux/limits.h>
char *version = "Unix v1.07a";
#elif __FreeBSD__
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
char *version = "Unix v1.07a";
#endif
#ifdef __CYGWIN__
#define NAME_MAX 14
#define _POSIX_NAME_MAX 14
#define PATH_MAX 512
#define _POSIX_PATH_MAX 255
char *version = "Windows v1.07a";
#elif __MINGW32__
#define NAME_MAX 14
#define _POSIX_NAME_MAX 14
#define PATH_MAX 512
#define _POSIX_PATH_MAX 255
char *version = "Windows v1.07a";
#elif __MINGW64__
#define NAME_MAX 14
#define _POSIX_NAME_MAX 14
#define PATH_MAX 512
#define _POSIX_PATH_MAX 255
char *version = "Windows v1.07a";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int writef(const char *nomefile, const long size, const long SIZE,
           const char *copy);
void usage(const char *nomefile, const long SIZE, const char *BUFFER);
char *basename(const char *name);
int main(int argc, char *argv[]) {
  long size = 1;
#ifdef DEBUG
  int count;
#endif
  int mem_need = 0;
  /* default string */
  char *BUFFER = "Copyright 1993-98 by P.H.C. - All Rights Reserved.";
  /* default buffer size */
  /* long   SIZE=52428;  */
  long SIZE = 1048576;
  /* long   SIZE=31457280; */

  printf("%s\n", BUFFER);
  printf("%s - %s\n", basename(argv[0]), version);
#ifdef DEBUG
  printf("argc, %d\n", argc);
  for (count = 0; count <= argc; count++) {
    printf("Argc %d, argv %s\n", count, argv[count]);
  }
#endif
  if ((argc < 2)) {
    usage(basename(argv[0]), SIZE, BUFFER);
    exit(-2);
  }
  switch (argc) {
    case 3: /* all defaults read size of file */
      size = atol(argv[2]);
      break;
    case 4: /* impossible not enought elements */
    case 6: /* impossible not enought elements */
      usage(basename(argv[0]), SIZE, BUFFER);
      exit(-2);
      break;
    case 5:
      size = atol(argv[2]);
#ifdef DEBUG
      printf("argv[3] %s\n", argv[3]);
      printf("argv[4] %s\n", argv[4]);
#endif
      switch (argv[3][1]) {
        case 's':
        case 'S':
#ifdef DEBUG
          printf("size %ld\n", SIZE);
#endif
          SIZE = atol(argv[4]);
#ifdef DEBUG
          printf("size %ld\n", SIZE);
#endif
          break;
        case 't':
        case 'T':
#ifdef DEBUG
          printf("BUFFER %s\n", BUFFER);
          printf("argv[4] %s\n", argv[4]);
#endif
          mem_need = strlen(argv[4]);
          if (!(BUFFER = malloc(mem_need))) {
            perror("Main ");
            exit(-3);
          }
          memmove(&BUFFER, &argv[4], strlen(argv[4]));
#ifdef DEBUG
          printf("BUFFER %s\n", BUFFER);
#endif
          break;
      }
      break;
    case 7:
      size = atol(argv[2]);
      switch (argv[3][1]) {
        case 's':
        case 'S':
          SIZE = atol(argv[4]);
          break;
        case 't':
        case 'T':
#ifdef DEBUG
          printf("argv[4] %s\n", argv[4]);
#endif
          mem_need = strlen(argv[4]);
          if (!(BUFFER = malloc(mem_need))) {
            perror("Main ");
            exit(-3);
          }
          memmove(&BUFFER, &argv[4], strlen(argv[4]));
          break;
      }
      switch (argv[5][1]) {
        case 's':
        case 'S':
          SIZE = atol(argv[6]);
          break;
        case 't':
        case 'T':
#ifdef DEBUG
          printf("argv[5] %s\n", argv[5]);
          printf("argv[6] %s\n", argv[6]);
#endif
          mem_need = strlen(argv[4]);
          if (!(BUFFER = malloc(mem_need))) {
            perror("Main ");
            exit(-3);
          }
          memmove(&BUFFER, &argv[6], strlen(argv[6]));
          break;
      }
      break;
    default: /* impossible not enought elements */
      usage(basename(argv[0]), SIZE, BUFFER);
      exit(-2);
      break;
  }
  mem_need = writef(argv[1], size, SIZE, BUFFER);
  switch (mem_need) {
    case 0:
      printf("\n");
      exit(0);
      break;
    default:
      exit(mem_need);
      break;
  }
}

int writef(const char *nomefile, const long size, const long SIZE,
           const char *copy) {
  FILE *fp;
  char *buffer;
  int num;

  fp = fopen(nomefile, "wb");
  if (!(fp)) {
    perror("Writef ");
    return (-4);
  }
  if (!(buffer = malloc(SIZE))) {
    perror("Writef ");
    return (-3);
  }
  for (num = 0; num < SIZE / strlen(copy); num++) {
    memcpy(buffer + (num * strlen(copy)), copy, strlen(copy));
  }
  setvbuf(fp, buffer, _IOFBF, SIZE);
  for (num = 0; num < size; num++) {
    if (fwrite(buffer, 1, SIZE, fp) != SIZE) break;
    printf(".");
    fflush(NULL);
  }
  fflush(NULL);
  fclose(fp);
  free(buffer);
  return (0);
}
void usage(const char *nomefile, const long SIZE, const char *BUFFER) {
  printf("Usage: \n");
  printf(" %s <filename> <size> | [-s Buffer size] | [-t \"text to use\"]\n",
         nomefile);
  printf(" The file is created with a multiple of %ld Kb \n", SIZE / 1024);
  printf("   -s buffer size default %ld\n", SIZE);
  printf("   -t uses the text as a pattern for nuking\n");
  printf("   default \"%s\"\n", BUFFER);
  printf(" WARNING! This may be a very dangerous program!\n");
  printf(" If you don't know what you're doing, ");
  printf("DON'T DO IT!!\n");
}

char *basename(const char *name) {
  const char *base = name;

  while (*name) {
    if (*name == '/') base = name + 1;
    ++name;
  }
  return (char *)base;
}
