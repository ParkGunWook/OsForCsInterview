#include "protocol.h"

#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

int main() {
  int fd = shm_open(NAME, O_CREAT | O_EXCL | O_RDWR, 0600);
  if (fd < 0) {
    perror("shm_open()");
    return EXIT_FAILURE;
  }
  char *message_0 = "Hello World!";
  ftruncate(fd, SIZE);

  char *data =
      (char *)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  printf("sender mapped address: %p\n", data);

  sprintf(data, "%s", message_0);

  munmap(data, SIZE);

  close(fd);

  return EXIT_SUCCESS;
}