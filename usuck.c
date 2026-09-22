#if defined(linux) || defined(__linux__)
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <linux/limits.h>
char *version = "Unix v1.10";
#elif defined(__FreeBSD__)
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
char *version = "Unix v1.10";
#elif defined(__APPLE__) || defined(__MACH__)
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
char *version = "Unix v1.10";
#elif defined(__CYGWIN__)
#define NAME_MAX 512
#define PATH_MAX 512
char *version = "Windows v1.10";
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#elif defined(__MINGW32__) || defined(__MINGW64__)
#define NAME_MAX 512
#define PATH_MAX 512
char *version = "Windows v1.10";
#include <ctype.h>
#else
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
char *version = "Unix v1.10";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "tinydir/tinydir.h"

int writef(const char *nomefile, const long size, const long SIZE, const char *copy);
void usage(const char *nomefile, const long SIZE, const char *BUFFER);
char *get_basename(const char *name);

int main(int argc, char *argv[]) {
  long size = 1;
  long SIZE = 1048576; /* default buffer size (1MB) */
  const char *BUFFER = "Copyright 1993-1998 (rewrote in 2022) fixed via gemini in 2026 by P.H.C. - All Rights Reserved.";
  int opt;

  printf("%s\n", BUFFER);
  printf("%s - %s\n", get_basename(argv[0]), version);

  while ((opt = getopt(argc, argv, "s:t:")) != -1) {
    switch (opt) {
      case 's':
        SIZE = atol(optarg);
        if (SIZE <= 0) {
          fprintf(stderr, "Error: Buffer size must be greater than 0.\n");
          exit(-1);
        }
        break;
      case 't':
        BUFFER = optarg;
        break;
      default:
        usage(get_basename(argv[0]), SIZE, BUFFER);
        exit(-2);
    }
  }

  if (argc - optind < 2) {
    usage(get_basename(argv[0]), SIZE, BUFFER);
    exit(-2);
  }

  const char *filename = argv[optind];
  size = atol(argv[optind + 1]);

  if (size <= 0) {
    fprintf(stderr, "Error: File write multiplier (size) must be greater than 0.\n");
    exit(-1);
  }

  int result = writef(filename, size, SIZE, BUFFER);
  if (result == 0) {
    printf(" \n");
    exit(0);
  } else {
    exit(result);
  }
}

int writef(const char *nomefile, const long size, const long SIZE, const char *copy) {
  FILE *fp;
  char *buffer;
  long num;
  size_t copy_len = strlen(copy);

  if (copy_len == 0) {
    fprintf(stderr, "Writef Error: Pattern text length cannot be 0.\n");
    return (-5);
  }

  fp = fopen(nomefile, "wb");
  if (!fp) {
    perror("Writef (fopen)");
    return (-4);
  }

  buffer = malloc(SIZE);
  if (!buffer) {
    perror("Writef (malloc)");
    fclose(fp);
    return (-3);
  }

  size_t bytes_written = 0;
  while (bytes_written + copy_len <= (size_t)SIZE) {
    memcpy(buffer + bytes_written, copy, copy_len);
    bytes_written += copy_len;
  }
  
  if (bytes_written < (size_t)SIZE) {
    memcpy(buffer + bytes_written, copy, (size_t)SIZE - bytes_written);
  }

  setvbuf(fp, buffer, _IOFBF, SIZE);

  for (num = 0; num < size; num++) {
    if (fwrite(buffer, 1, SIZE, fp) != (size_t)SIZE) {
      perror("Writef (fwrite)");
      break;
    }
    printf(".");
    fflush(stdout);
  }

  printf("\nFile writing complete.");
  fflush(stdout);
  fclose(fp);
  free(buffer);
  return (0);
}

void usage(const char *nomefile, const long SIZE, const char *BUFFER) {
  printf("\nUsage: \n");
  printf(" %s [-s Buffer size] [-t \"text to use\"] <filename> <size>\n", nomefile);
  printf(" The file is created with a multiple of %ld Kb \n", SIZE / 1024);
  printf("   -s buffer size default %ld\n", SIZE);
  printf("   -t uses the text as a pattern for nuking\n");
  printf("   default \"%s\"\n", BUFFER);
  printf(" WARNING! This may be a very dangerous program!\n");
  printf(" If you don't know what you're doing, DON'T DO IT!!\n");
}

char *get_basename(const char *name) {
  const char *base = name;
  while (*name) {
    if (*name == '/' || *name == '\\') base = name + 1;
    ++name;
  }
  return (char *)base;
}

