#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

int main() {
    const int array_size = 10;
    const int array_bytes = sizeof(int) * array_size;
    const int offset = 0;
    const int protection = PROT_READ | PROT_WRITE;
    const int flags = MAP_PRIVATE | MAP_ANONYMOUS;

    int* array;
    if (asprintf((char**)&array, "%0*c", array_bytes, '\0') == -1) {
        perror("Error allocating the array");
        exit(EXIT_FAILURE);
    }

    array = (int*) mmap(NULL, array_bytes, protection, flags, -1, offset);
    if (array == MAP_FAILED) {
        perror("Error mmapping the array");
        exit(EXIT_FAILURE);
    }

    // Initialize the array contents
    for (int i = 0; i < array_size; i++) {
        array[i] = i * i;
    }

    // Print the array contents
    for (int i = 0; i < array_size; i++) {
        printf("array[%d] = %d\n", i, array[i]);
    }

    if (munmap(array, array_bytes) == -1) {
        perror("Error un-mmapping the array");
        exit(EXIT_FAILURE);
    }

    return 0;
}
