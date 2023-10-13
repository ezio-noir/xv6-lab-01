#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void prime(int pipeLeft[]) {
    close(pipeLeft[1]);
    int p;
    if (read(pipeLeft[0], &p, sizeof(p)) == 0) {
        fprintf(2, "Error: cannot read from pipe\n");
        exit(1);
    }
    else {
        printf("prime %d\n", p);
        int pipeRight[2];
        pipe(pipeRight);
        if (fork() > 0) {
            close(pipeRight[0]);
            int n;
            while (read(pipeLeft[0], &n, sizeof(n)) == sizeof(n)) {
                if (n % p != 0) {
                    write(pipeRight[1], &n, sizeof(n));
                }
            }
            close(pipeLeft[0]);
            close(pipeRight[1]);
            wait((int*)0);
        }
        else {
            prime(pipeRight);
        }
    }
    exit(0);
}


int main(int argc, char *argv[]) {
    int pipeRight[2];
    pipe(pipeRight);

    if (fork() > 0) {
        close(pipeRight[0]);
        for (int i = 2; i <= 35; ++i) {
            write(pipeRight[1], &i, sizeof(i));
        }
        close(pipeRight[1]);
        wait((int*)0);
        exit(0);
    }
    else {
        prime(pipeRight);
    }

    return 0;
}