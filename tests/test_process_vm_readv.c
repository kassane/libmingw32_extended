//! source: https://gist.github.com/FergusInLondon/fec6aebabc3c9e61e284983618f40730

#define _GNU_SOURCE
#include <sys/uio.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("usage: %s <pid> <mem address> [len]\n", argv[0]);
        printf("  <pid> - PID of process to target\n");
        printf("  <mem> - Memory address to target\n");
        printf("  [len] - Length (in bytes) to dump\n");
        return -1;
    }

// PARSE CLI ARGS
    pid_t pid = strtol(argv[1], NULL, 10);
    printf(" * Launching with a target PID of: %zd\n", pid);

    void *remotePtr = (void *)strtol(argv[2], NULL, 0);
    printf(" * Launching with a target address of 0x%llx\n", remotePtr);

    size_t bufferLength = (argc > 3) ? strtol(argv[3], NULL, 10) : 20;
    printf(" * Launching with a buffer size of %d bytes.\n", bufferLength);

// Build iovec structs
    struct iovec local[1];
    local[0].iov_base = calloc(bufferLength, sizeof(char));
    local[0].iov_len = bufferLength;

    struct iovec remote[1];
    remote[0].iov_base = remotePtr;
    remote[0].iov_len = bufferLength;

// Call process_vm_readv - handle any error codes
    ssize_t nread = process_vm_readv(pid, local, 2, remote, 1, 0);
    if (nread < 0) {
        switch (errno) {
            case EINVAL:
              printf("ERROR: INVALID ARGUMENTS.\n");
              break;
            case EFAULT:
              printf("ERROR: UNABLE TO ACCESS TARGET MEMORY ADDRESS.\n");
              break;
            case ENOMEM:
              printf("ERROR: UNABLE TO ALLOCATE MEMORY.\n");
              break;
            case EPERM:
              printf("ERROR: INSUFFICIENT PRIVILEGES TO TARGET PROCESS.\n");
              break;
            case ESRCH:
              printf("ERROR: PROCESS DOES NOT EXIST.\n");
              break;
            default:
              printf("ERROR: AN UNKNOWN ERROR HAS OCCURRED.\n");
        }

        return -1;
    }

    printf(" * Executed process_vm_ready, read %zd bytes.\n", nread);
    printf("%s\n", local[0].iov_base);

    return 0;
}
