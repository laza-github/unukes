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

int debug = 1; /* switch to 0 for debug */
int all_file = 0;

int writef(const char *nomefile, const long size, const long SIZE, const char *BUFFER);
void usage(const char *prgfile, const long SIZE, const char *BUFFER);
int search_dir(const char *search, const long SIZE, const char *BUFFER);
char *toLower(char *s);

int main(int argc, char **argv) {
  long SIZE = 1048576; /* default buffer size (1MB) */
  const char *BUFFER = "Copyright 1993-1998 (rewrote in 2022) fixed via gemini in 2026 by P.H.C. - All Rights Reserved.";
  char nomefile[NAME_MAX];
  char cur_dir[PATH_MAX];
  const char *updir = ".";
  int done, opt;
  int dir_search = 0;
  struct stat statfile;

  memset(nomefile, 0, sizeof(nomefile));

  printf(" %s:%s ", (argc > 0) ? argv[0] : "unukes", version);
  printf("- %s\n\n", BUFFER);
  fflush(stdout);

  // Use getopt for robust, overflow-safe parsing
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
        BUFFER = optarg; // Safe direct assignment without stack corruption
        break;
      default:
        usage((argc > 0) ? argv[0] : "unukes", SIZE, BUFFER);
        exit(-1);
    }
  }

  // Expect exactly one remaining non-option argument (<filename> or `.`)
  if (argc - optind != 1) {
    usage((argc > 0) ? argv[0] : "unukes", SIZE, BUFFER);
    exit(-1);
  }

  const char *target_arg = argv[optind];

  if (strcmp(target_arg, updir) != 0) {
    // Safely copy and bounds-check target name
    strncpy(nomefile, target_arg, sizeof(nomefile) - 1);
    nomefile[sizeof(nomefile) - 1] = '\0';
    toLower(nomefile);
  } else {
    all_file = 1;
  }

  if (getcwd(cur_dir, sizeof(cur_dir)) == NULL) {
    perror("Error working directory getcwd PATH_MAX");
    return (-4);
  }

  if (debug == 0) {
    printf("search filename %s\n", nomefile);
  }

  if (all_file == 0) {
    if (stat(nomefile, &statfile) == -1) {
      printf("\"%s\" file not found, using as search filter in subdirectories.\n", nomefile);
    } else {
      if (S_ISDIR(statfile.st_mode)) {
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
  }

  done = search_dir(nomefile, SIZE, BUFFER);

  if (done == 0) {
    if (dir_search == 1) {
      if (chdir(cur_dir) != 0) {
        perror("Error chdir reverting to cur_dir");
        exit(-5);
      }
      if (rmdir(nomefile) != 0) {
        perror("rmdir root target error");
        exit(-3);
      }
    }
    printf("\nDone.\n...Have a nice day!\n");
    exit(0);
  } else {
    printf("\nERROR occurred.\n...Have a nice day!\n");
    exit(done);
  }
}

int writef(const char *nomefile, const long size, const long SIZE, const char *BUFFER) {
  FILE *fp;
  char *buffer;
  long num;
  size_t pattern_len = strlen(BUFFER);

  if (pattern_len == 0) {
    fprintf(stderr, "Writef Error: Pattern text length cannot be 0.\n");
    return (-5);
  }

  if (debug == 0) {
    printf("writef file to be wiped: %s\n", nomefile);
  }

#if defined(linux) || defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__) || defined(__MACH__)
  if (chmod(nomefile, S_IRUSR | S_IWUSR) == -1) {
    perror("Writef chmod error");
    return (-1);
  }
#endif

  fp = fopen(nomefile, "wb");
  if (!fp) {
    fprintf(stderr, "Unable To Open %s!\n", nomefile);
    perror("Writef fopen");
    return (-1);
  }

  buffer = malloc(SIZE);
  if (!buffer) {
    perror("writef unable to alloc memory");
    fclose(fp);
    return (-2);
  }

  // Safely fill buffer handling remainders and preventing out-of-bounds evaluation
  size_t bytes_written = 0;
  while (bytes_written + pattern_len <= (size_t)SIZE) {
    memcpy(buffer + bytes_written, BUFFER, pattern_len);
    bytes_written += pattern_len;
  }
  if (bytes_written < (size_t)SIZE) {
    memcpy(buffer + bytes_written, BUFFER, (size_t)SIZE - bytes_written);
  }

  setvbuf(fp, buffer, _IOFBF, SIZE);

  long iterations = (size / SIZE) + 2;
  for (num = 0; num < iterations; num++) {
    if (fwrite(buffer, 1, SIZE, fp) != (size_t)SIZE) {
      perror("Writef fwrite loop broke prematurely");
      break;
    }
  }

  fflush(fp);
  fclose(fp);
  free(buffer);

  if (unlink(nomefile) != 0) {
    perror("Unlink writef failed");
    return (-3);
  }

  return (0);
}

void usage(const char *prgfile, const long SIZE, const char *BUFFER) {
  printf("Usage: \n");
  printf(" %s [-s Buffer size] [-t \"text to use\"] <filename | .>\n", prgfile);
  printf(" This program searches the current directory and subdirectories.\n");
  printf(" The files are wiped with a multiple of %ld Kb \n", SIZE / 1024);
  printf("   -s buffer size default %ld\n", SIZE);
  printf("   -t uses the text as a pattern for wiping\n");
  printf("   default \"%s\"\n", BUFFER);
}

int search_dir(const char *search, const long SIZE, const char *BUFFER) {
  tinydir_dir dir;
  int i;
  const char *updir = ".";
  const char *curr = "..";
  char tempfile[NAME_MAX];
  char cur_dir[PATH_MAX];
  int result = 0;
  struct stat statfile;

  if (getcwd(cur_dir, sizeof(cur_dir)) == NULL) {
    perror("Error search_dir getcwd");
    return (-5);
  }

  if (tinydir_open_sorted(&dir, cur_dir) == -1) {
    perror("Error opening directory");
    return (-5);
  }

  for (i = 0; i < (int)dir.n_files; i++) {
    tinydir_file file;
    if (tinydir_readfile_n(&dir, &file, i) == -1) {
      continue;
    }

    if (file.is_dir) {
      if (strcmp(file.name, updir) == 0 || strcmp(file.name, curr) == 0) {
        continue;
      }
      if (chdir(file.name) != 0) {
        perror("Error search_dir chdir descend");
        tinydir_close(&dir);
        return (-5);
      }
      result = search_dir(search, SIZE, BUFFER);
      if (result != 0) {
        tinydir_close(&dir);
        return (-3);
      }
      if (chdir(cur_dir) != 0) {
        perror("Error search_dir chdir ascend fallback");
        tinydir_close(&dir);
        return (-5);
      }
      if (all_file == 1) {
        if (rmdir(file.name) != 0) {
          perror("rmdir subdirectory error");
          tinydir_close(&dir);
          return (-3);
        }
      }
    } else {
      if (stat(file.name, &statfile) == -1) {
        continue;
      }

      if (all_file == 0) {
        // Safe string operation with bounds limits
        strncpy(tempfile, file.name, sizeof(tempfile) - 1);
        tempfile[sizeof(tempfile) - 1] = '\0';
        toLower(tempfile);

        if (strstr(tempfile, search) != NULL) {
          result = writef(file.name, statfile.st_size + 1, SIZE, BUFFER);
        } else {
          result = 0;
        }
      } else {
        result = writef(file.name, statfile.st_size + 1, SIZE, BUFFER);
      }

      if (result != 0) {
        fprintf(stderr, "writef failed on file: %s\n", file.name);
        tinydir_close(&dir);
        return (-3);
      }
    }
  }
  
  size_t total_files = dir.n_files;
  tinydir_close(&dir);

  // Safe iteration tracking using snprintf bounds management 
  for (i = 0; i < (int)total_files; i++) {
    FILE *trash;
    char target_nuke[NAME_MAX];
    snprintf(target_nuke, sizeof(target_nuke), "nukes.%03d", i);
    trash = fopen(target_nuke, "w");
    if (trash) {
      fprintf(trash, "%s\n", BUFFER);
      fclose(trash);
    }
  }
  for (i = 0; i < (int)total_files; i++) {
    char target_nuke[NAME_MAX];
    snprintf(target_nuke, sizeof(target_nuke), "nukes.%03d", i);
    unlink(target_nuke);
  }

  return (0);
}

char *toLower(char *s) {
  if (!s) return NULL;
  for (char *p = s; *p; p++) {
    *p = (char)tolower((unsigned char)*p);
  }
  return s;
}
