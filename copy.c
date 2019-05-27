#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NAME 0
#define OPTIONS 1
#define INFILE 2
#define OUTFILE 3

#define BUFFERSIZE 1024


int help(char *name) {
  printf("usage:\n"
  " %s  [-h] [-m] <file_name> <new_file_name>\n"
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

int error_msg(char *message, char *name, int error_code) {
  if (name != NULL) {
    printf("%s, see help: %s -h\n", message, name);
  } else {
    printf("%s\n", message);
  }
  return error_code;
}

int copy_read_write (int fd_from, int fd_to) {
  char buffer[BUFFERSIZE];
  int numbytes;

  while ((numbytes = read(fd_from, &buffer, sizeof(char))) > 0){
    write(fd_to, &buffer, numbytes);
  }

  return 0;
}

int copy_mmap(int fd_from, int fd_to) {
  void *src, *dest;
  struct stat s;
  int filesize;

  int status = fstat (fd_from, &s);
  filesize = s.st_size;

  if ((src = mmap(NULL, filesize, PROT_READ, MAP_SHARED, fd_from, 0)) == (void *) -1) {
    return error_msg("Error mapping in file",NULL,4);
  }
  ftruncate(fd_to, filesize);

  if ((dest = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd_to, 0)) == (void *) -1) {
    return error_msg("Error mapping destination file",NULL,4);
  }

  memcpy(dest, src, filesize);

  munmap(src, filesize);
  munmap(dest, filesize);


  return 0;
}

void check_arguments(int arguments, int how_many, char *name) {
  if (arguments > how_many) {
    // Returns error code 3 which means too many arguments
    exit (error_msg("Too many arguments winyo", name, 3));
  } else if (arguments < how_many) {
    //  Returns error code 4 which means not enough arguments
    exit (error_msg("Not enough arguments winyo", name, 4));
  }
}

int main(int argc, char *argv[]) {
  //  Using getopt()
  int opt = getopt(argc, argv, "mh");

  if (opt == 'h') {
    return help(argv[NAME]);
  }

  // Checking 2 filenames
  if (opt != -1) {
    check_arguments(argc,4,argv[NAME]);
  } else {
    check_arguments(argc,3,argv[NAME]);
  }

  // Both file descriptors
  int fd_from, fd_to;
  fd_from = open(argv[optind], O_RDONLY);
  fd_to = open(argv[optind + 1], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

  // if file doesn't exists
  if (fd_from == -1) {
    return error_msg("Error opening the file to be copied", argv[NAME], 6);
  }

  if (opt != -1) {
    switch (opt) {
      case 'm':
        // use of copy_mmap
        if (copy_mmap(fd_from,fd_to) == 0) {
            printf("File %s copied as %s successfully\n", argv[optind], argv[optind + 1]);
        } else {
          return error_msg("Error copying the files", argv[NAME], 5);
        }
          break;
      case '?':
        return error_msg("Bad option", argv[NAME], 1);
    }
  } else {
    if (copy_read_write(fd_from,fd_to) == 0) {
      printf("File %s copied as %s successfully\n", argv[optind], argv[optind + 1]);
    } else {
      return error_msg("Error copying the files", argv[NAME], 5);
    }
  }

  // Closing files
  close(fd_from);
  close(fd_to);

  /*
  // Not using getopt() working propertly:

  if (argc < 2) {
    // Returns error code 1 which means no options given.
    return error_msg("No options given", argv[NAME], 1);
  } else if (argc == 2) {
    if (strcmp(argv[OPTIONS],"-h") == 0 || strcmp(argv[OPTIONS],"--help") == 0) {
      return help(argv[NAME]);
    } else {
      // Returns error code 2 which menas bad option
      return error_msg("Bad option", argv[NAME], 2);
    }
  } else {
    // It is possible to do -h and more arguments, so we checked again
    if (strcmp(argv[OPTIONS],"-h") == 0 || strcmp(argv[OPTIONS],"--help") == 0) {
      // Returns error code 2 which means bad option
      return error_msg("Bad option", argv[NAME], 2);
      // Another way could be, showing help and not error_msg:
      //return help(argv[NAME]);
    }
    // Checking too many arguments (no more than 2 files)
    if (argc > 4 && strcmp(argv[OPTIONS],"-m") == 0 ||
        argc > 3 && strcmp(argv[OPTIONS],"-m") != 0) {
      // Returns error code 3 which means too many arguments
      return error_msg("Too many arguments winyo", argv[NAME], 3);
    }
    // Both file descriptors
    int fd_from, fd_to;

    // Opening files
    if (strcmp(argv[OPTIONS],"-m") == 0) {
      fd_from = open(argv[INFILE], O_RDONLY);
      fd_to = open(argv[OUTFILE], O_WRONLY|O_CREAT|O_TRUNC, 0700);
    } else {
      fd_from = open(argv[INFILE - 1], O_RDONLY);
      fd_to = open(argv[OUTFILE - 1 ], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    }

    // if file doesn't exists
    if (fd_from == -1) {
      return error_msg("Error opening the file to be copied", argv[NAME], 6);
    }

    // Calling functions
    if (strcmp(argv[OPTIONS],"-m") == 0) {
      // use of read and write
      if (copy_read_write(fd_from,fd_to) == 0) {
        printf("File %s copied as %s successfully\n", argv[INFILE], argv[OUTFILE]);
      } else {
        return error_msg("Error copying the files", argv[NAME], 5);
      }
    } else {
      // use of nmap
      if (copy_mmap(fd_from,fd_to) == 0) {
        if (strcmp(argv[OPTIONS],"-m") == 0) {
          printf("File %s copied as %s successfully\n", argv[INFILE], argv[OUTFILE]);
        } else {
          printf("File %s copied as %s successfully\n", argv[INFILE -1], argv[OUTFILE -1]);
        }
      } else {
        return error_msg("Error copying the files", argv[NAME], 5);
      }
    }

    // Closing files
    close(fd_from);
    close(fd_to);
  }
  */
  return 0;
}
