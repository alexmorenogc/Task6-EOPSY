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


int help(char *name) {
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

int error_msg(char *message, char *name, int error_code) {
  printf("%s, see help: %s -h\n", message, name);
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

int copy_mmap(char fd_from, char fd_to) {
  /*
  Not implemented yet
  */

  int sfd, dfd;
  char *src, *dest;
  size_t filesize;

  /* SOURCE */
  sfd = open("hello.c", O_RDONLY);
  filesize = lseek(sfd, 0, SEEK_END);

  src = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, sfd, 0);

  /* DESTINATION */
  dfd = open("dest", O_RDWR | O_CREAT, 0666);

  ftruncate(dfd, filesize);

  dest = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, dfd, 0);

  /* COPY */

  memcpy(dest, src, filesize);

  munmap(src, filesize);
  munmap(dest, filesize);

  close(sfd);
  close(dfd);

  return 0;
}

int main(int argc, char *argv[]) {
  /*  Using getopt()
  printf("testing getopt: %d\n", getopt(argc,argv,":hm"));
  */
  /*  Not using getopt():  */
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

    int fd_from, fd_to;
    /* Opening files */
    fd_from = open(argv[INFILE], O_RDONLY);
    fd_to = open(argv[OUTFILE], O_WRONLY|O_CREAT|O_TRUNC, 0700);

    if (strcmp(argv[OPTIONS],"-m") == 0) {
      /* use of read and write */
      copy_read_write(fd_from,fd_to);
    } else {
      /* use of nmap */
      //copy_mmap(argv[INFILE],argv[OUTFILE]);
    }

    /* Closing files */
    close(fd_from);
    close(fd_to);
  }
  return 0;
}
