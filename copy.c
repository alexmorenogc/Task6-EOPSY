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

#define BUFFERSIZE 1024


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

int m_option (char const *infile, char const *outfile) {
  int descriptorIn, descriptorOut;
  char buffer[BUFFERSIZE];
  int numbytes;

  /* Opening files */
  descriptorIn = open(infile, O_RDONLY);
  descriptorOut = open(outfile, O_WRONLY|O_CREAT|O_TRUNC, 0700);

  /* Copying files */
  while ((numbytes = read(descriptorIn, &buffer, sizeof(char))) > 0){
    write(descriptorOut, &buffer, numbytes);
  }

  /* Closing files */
  close(descriptorIn);
  close(descriptorOut);

  return 0;
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
    /* It is possible to do -h and more arguments, so we checked again */
    if (strcmp(argv[OPTIONS],"-h") == 0 || strcmp(argv[OPTIONS],"--help") == 0) {
      /* Returns error code 2 which means bad option */
      return error_msg("Bad option", argv[NAME], 2);
      /* Another way could be, showing help and not error_msg:
      return help(argv[NAME]);
      */
    }
    /* Checking too many arguments (no more than 2 files) */
    if (argc > 4 && strcmp(argv[OPTIONS],"-m") == 0 ||
        argc > 3 && strcmp(argv[OPTIONS],"-m") != 0) {
      /* Returns error code 3 which means too many arguments */
      return error_msg("Too many arguments winyo", argv[NAME], 3);
    }

    if (strcmp(argv[OPTIONS],"-m") == 0) {
      /* use of read and write */
      m_option(argv[INFILE],argv[OUTFILE]);
    } else {
      /*

      Not implemented yet

      */
      for (int i = 0; i < argc; i++) {
        printf("arguments: %s\n", argv[i]);
      }
    }
  }
  return 0;
}
