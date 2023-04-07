#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

int main() {
   const int page_size = getpagesize();
    const int file_size = page_size * 2;
    const int offset = 0;
    const int protection = PROT_READ | PROT_WRITE;
    const int flags = MAP_PRIVATE | MAP_ANONYMOUS;

    char* file_contents = mmap(NULL, file_size, protection, flags, -1, offset);
    if (file_contents == MAP_FAILED) {
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }

    // Allocate the string dynamically using asprintf
    if (asprintf(&file_contents, "Hello, world!") == -1) {
        perror("Error allocating the string");
        exit(EXIT_FAILURE);
    }

    printf("\n%s\n", file_contents);

    if (munmap(file_contents, file_size) == -1) {
        perror("Error un-mmapping the file");
        exit(EXIT_FAILURE);
    }


    return 0;
}
