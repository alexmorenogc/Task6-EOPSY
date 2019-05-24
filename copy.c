#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define NAME 0
#define OPTIONS 1
#define INFILE 2
#define OUTFILE 3


int help(char const *name) {
  printf("usage:\n"
  " %s  [-h|--help] [-m] <file_name> <new_file_name>\n"
  "\n"
  "   Copy files using read() or write() or not (-m to use them)\n"
  "   if not, it will use mmap() and memcpy()\n"
  "\n"
  " %s correct syntax examples:\n"
  "   %s --help\n"
  "   %s -m filename_input.txt filename_output.txt\n"
  "   %s filename_input.txt filename_output.txt\n"
  " %s incorrect syntax examples:\n"
  "   %s --d\n", name, name, name, name, name, name, name);
  return 0;
}

int error_msg(char *message, char const *name, int error_code) {
  printf("%s, see help: %s -h\n", message, name);
  return error_code;
}

int main(int argc, char const *argv[]) {
  if (argc < 2) {
    /* Returns error code 1 which means no options given. */
    return error_msg("No options given", argv[NAME], 1);
  } else if (argc == 2) {
    if (strcmp(argv[OPTIONS],"-h") == 0 || strcmp(argv[OPTIONS],"--help") == 0) {
      return help(argv[NAME]);
    } else {
      /* Returns error code 2 which menas bad option */
      return error_msg("Bad option", argv[NAME], 2);
    }
  } else {
    if (strcmp(argv[OPTIONS],"-h") == 0 || strcmp(argv[OPTIONS],"--help") == 0) {
      /* Returns error code 2 which menas bad option */
      return error_msg("Bad option", argv[NAME], 2);
    }
    for (int i = 0; i < argc; i++) {
      printf("arguments: %s\n", argv[i]);
    }
  }
  return 0;
}
