#ifdef linux
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <linux/limits.h>
char *version = "Unix v1.09a";
#elif __FreeBSD__
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
char *version = "Unix v1.09a";
#endif
#ifdef __CYGWIN__
#define NAME_MAX 512
#define PATH_MAX 512
char *version = "Windows v1.09a";
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#elif __MINGW32__
#define NAME_MAX 512
#define PATH_MAX 512
char *version = "Windows v1.09a";
#elif __MINGW64__
#define NAME_MAX 512
#define PATH_MAX 512
char *version = "Windows v1.09a";
#endif

int debug = 1; /* switch to 0 for debug */
int all_file = 0;
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "tinydir/tinydir.h"

int writef(const char *nomefile, const long size, const long SIZE,
           const char *BUFFER);
void usage(const char *prgfile, const long SIZE, const char *BUFFER);
int search_dir(char *search, const long SIZE, const char *BUFFER);
char *toLower(char *s);

int main(int argc, char **argv) {
  long int SIZE = 52428;
  char *BUFFER =
      "Copyright 1993-1998 (rewrote in 2022) by P.H.C. - All Rights Reserved.";
  char nomefile[NAME_MAX];
  char cur_dir[PATH_MAX];
  char *updir = ".";
  int done, num, dir_search = 0;
  struct stat statfile;

  printf(" %s:%s ", basename(argv[0]), version);
  printf("- %s\n\n", BUFFER);
  fflush(NULL);
  if (argc < 2) {
    usage(basename(argv[0]), SIZE, BUFFER);
    exit(-1);
  }
  num = 0;
  /* getting parameters */

  switch (argc) {
    case 2: /* all defaults read size of file */
      break;
    case 3: /* impossible not enought elements */
    case 5: /* impossible not enought elements */
      usage(basename(argv[0]), SIZE, BUFFER);
      exit(-1);
      break;
    case 4:
      switch (argv[2][1]) {
        case 's':
        case 'S':
          SIZE = atol(argv[3]);
          break;
        case 't':
        case 'T':
          num = strlen(argv[3]);
          if (!(BUFFER = malloc(num))) {
            printf("Not Enought Memory!\n");
            exit(-2);
          }
          memmove(&BUFFER, &argv[3], strlen(argv[3]));
          break;
      }
      break;
    case 6:
      switch (argv[2][1]) {
        case 's':
        case 'S':
          SIZE = atol(argv[3]);
          break;
        case 't':
        case 'T':
          num = strlen(argv[3]);
          if (!(BUFFER = malloc(num))) {
            printf("Not Enought Memory!\n");
            exit(-2);
          }
          memmove(&BUFFER, &argv[3], strlen(argv[3]));
          break;
      }
      switch (argv[4][1]) {
        case 's':
        case 'S':
          SIZE = atol(argv[5]);
          break;
        case 't':
        case 'T':
          num = strlen(argv[5]);
          if (!(BUFFER = malloc(num))) {
            printf("Not Enought Memory!\n");
            exit(-2);
          }
          memmove(&BUFFER, &argv[5], strlen(argv[5]));
          break;
      }
      break;
    default: /* impossible not enought elements */
      usage(basename(argv[0]), SIZE, BUFFER);
      exit(-1);
      break;
  }

  /* END PARAMATERS */
  /* compare argv[1] with . switch all_file to 1 */
  if (strcmp(argv[1], updir) != 0) {
    strcpy(nomefile, toLower(basename(argv[1])));
  } else {
    all_file = 1;
  }

  if (getcwd(cur_dir, sizeof(cur_dir)) == NULL) {
    perror("Error wdir getcwd PATH_MAX");
    return (-4);
  }

  if (debug == 0) {
    printf("search filename %s\n", nomefile);
  }
  /* check if nomefile is a directory or not, if it's a directory cd to it and
   * do full erase */
  if (all_file == 0) {
    if (stat(nomefile, &statfile) == -1) {
      perror("lstat");
      exit(-1);
    }
    if ((statfile.st_mode & S_IFMT) == S_IFDIR) {
      if (debug == 0) {
        printf("search filename %s is a directory\n", nomefile);
      }
      if (chdir(nomefile) != 0) {
        perror("Error chdir filename is a directory");
        exit(-5);
      }
      all_file = 1;
      dir_search = 1;
    }
  }
  done = search_dir(nomefile, SIZE, BUFFER);
  if (done == 0) {
    if (dir_search == 1) {
      if (chdir(cur_dir) != 0) {
        perror("Error chdir cur_dir is a directory");
        exit(-5);
      }
      if (rmdir(nomefile) != 0) {
        if (debug == 0) {
          char tmp_dir[PATH_MAX];
          if (getcwd(tmp_dir, sizeof(tmp_dir)) == NULL) {
            perror("Error dir_search getcwd");
            return (-5);
          }
          printf("CWD %s\n", tmp_dir);
        }
        printf("rmdir %s Error\n", nomefile);
        perror("rmdir ");
        exit(-3);
      }
    }
    printf("\nDone. \n");
    printf("...Have a nice day!\n");
    exit(0);
  } else {
    printf("\nERROR. \n");
    printf("...Have a nice day!\n");
    exit(done);
  }
}

int writef(const char *nomefile, const long size, const long SIZE,
           const char *BUFFER) {
  FILE *fp;
  char *buffer;
  int num;

  if (debug == 0) {
    printf("writef file to be wiped %s\n", nomefile);
  }
#if defined(linux) || defined(__FreeBSD__)
  /* on windows we dont have a working chmod */
  if (chmod(nomefile, S_IRUSR | S_IWUSR) == -1) {
    printf("Unable To change %s\n", nomefile);
    perror("Writef ");
    return (-1);
  }
#endif
  fp = fopen(nomefile, "wb");
  if (!(fp)) {
    printf("Unable To Open %s!\n", nomefile);
    perror("Writef ");
    return (-1);
  }
  if (!(buffer = malloc(SIZE))) {
    perror("writef unable to alloc memory");
    return (-2);
  }
  for (num = 0; num < SIZE / strlen(BUFFER); num++) {
    memcpy(buffer + (num * strlen(BUFFER)), BUFFER, strlen(BUFFER));
  }
  setvbuf(fp, buffer, _IOFBF, SIZE);
  for (num = 0; num < (size / SIZE + 2); num++) {
    if (fwrite(buffer, 1, SIZE, fp) != SIZE) break;
  }
  fflush(NULL);
  fclose(fp);
  free(buffer);
  if (unlink(nomefile) != 0) {
    perror("Unlink writef");
    return (-3);
  }

  return (0);
}

void usage(const char *prgfile, const long SIZE, const char *BUFFER) {
  printf("Usage: \n");
  printf(" %s <filename>| [-s Buffer size] | [-t \"text to use\"]\n", prgfile);
  printf(" this programm will search the current directory and subdirectory\n");
  printf(" for the filename doing a string compare IS NOT using regexp \n");
  printf(" The files are nuked with a multiple of %ld Kb \n", SIZE / 1024);
  printf("   -s buffer size default %ld\n", SIZE);
  printf("   -t uses the text as a pattern for nuking\n");
  printf("   default \"%s\"\n", BUFFER);
  printf(" for example: %s .\n", prgfile);
  printf(
      " this will destroy ALL files in the current directory and subdirectory. "
      "\n");
  printf(" for example: %s .txt \n", prgfile);
  printf(
      " this will destroy ALL .txt files in the current directory and "
      "subdirectory. \n");
  printf(" WARNING! This may be a very dangerous program!\n");
  printf(" If you don't know what you're doing, ");
  printf("DON'T DO IT!!\n");
}

int search_dir(char *search, const long SIZE, const char *BUFFER) {
  tinydir_dir dir;
  int i;
  char *updir = ".";
  char *curr = "..";
  char *foundfile;
  char tempfile[NAME_MAX];
  char cur_dir[PATH_MAX];
  int result = 0;
  struct stat statfile;

  if (getcwd(cur_dir, sizeof(cur_dir)) == NULL) {
    perror("Error search_dir getcwd");
    return (-5);
  }

  if (debug == 0) {
    printf("begin search: %s\n", search);
  }
  tinydir_open_sorted(&dir, cur_dir);

  if (debug == 0) {
    printf("dir %s, dir.n_files %zu\n", search, dir.n_files);
  }
  for (i = 0; i < dir.n_files; i++) {
    tinydir_file file;
    tinydir_readfile_n(&dir, &file, i);
    if (file.is_dir) {
      /* skip . and .. */
      if (((strcmp(file.name, updir)) * (strcmp(file.name, curr))) == 0) {
        continue;
      }
      if (debug == 0) {
        printf("directory found %s CWD %s\n", file.name, cur_dir);
      }
      if (chdir(file.name) != 0) {
        perror("Error search_dir chdir");
        return (-5);
      }
      result = search_dir(search, SIZE, BUFFER);
      if (result != 0) {
        printf("search_dir search %s cur_dir %s Error\n", search, cur_dir);
        perror("search_dir ");
        return (-3);
      }
      if (chdir(cur_dir) != 0) {
        perror("Error search_dir chdir cur_dir");
        return (-5);
      }
      if (all_file == 1) {
        if (rmdir(file.name) != 0) {
          if (debug == 0) {
            char tmp_dir[PATH_MAX];
            if (getcwd(tmp_dir, sizeof(tmp_dir)) == NULL) {
              perror("Error search_dir getcwd");
              return (-5);
            }
            printf("CWD %s\n", tmp_dir);
          }
          printf("rmdir %s Error\n", file.name);
          perror("rmdir ");
          return (-3);
        }
      }
    } else {
      /* it's not a directory */
      if (debug == 0) {
        printf("file %s\n", file.name);
      }
      stat(file.name, &statfile);
      if (all_file == 0) {
        /* compare file.name with search */
        strcpy(tempfile, toLower(file.name));
        foundfile = strstr(tempfile, search);
        if (foundfile) {
          if (debug == 0) {
            printf("file found %s \n", file.name);
          }
          result = writef(file.name, statfile.st_size + 1, SIZE, BUFFER);
        } else {
          if (debug == 0) {
            printf("%s is not search %s \n", file.name, search);
          }
          result = 0;
        }
      } else {
        result = writef(file.name, statfile.st_size + 1, SIZE, BUFFER);
      }
      if (result != 0) {
        printf("writef in search_dir %s Error\n", file.name);
        perror("writef ");
        return (-3);
      }
    }
  }
  tinydir_close(&dir);
  /* create temporary file to wipe the directory listing */
  for (i = 0; i < dir.n_files; i++) {
    FILE *trash;
    char nomefile[NAME_MAX];

    sprintf(nomefile, "%s%03u", "nukes.", i);
    trash = fopen(nomefile, "w");
    fprintf(trash, "%s\n", BUFFER);
    fclose(trash);
  }
  for (i = 0; i < dir.n_files; i++) {
    char nomefile[NAME_MAX];

    sprintf(nomefile, "%s%03u", "nukes.", i);
    unlink(nomefile);
  }

  return (0);
}
char *toLower(char *s) {
  for (char *p = s; *p; p++) *p = tolower(*p);
  return s;
}
