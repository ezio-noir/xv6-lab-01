#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void compareAndPrint(char* path, const char* name) {
    char *p;
    for (p = path + strlen(path); p >= path && *p != '/'; --p);
    ++p;
    if (strlen(p) <= DIRSIZ) {
        if (strcmp(p, name) == 0) {
            printf("%s\n", path);
        }
    }
}

void find(char *path, const char *name) {
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, O_RDONLY)) < 0) {
        fprintf(2, "Error: cannot open %s\n", path);
        return;
    }
    
    if (fstat(fd, &st) < 0) {
        fprintf(2, "Error: cannot stat %s\n", path);
        close(fd);
        return;
    }

    char buf[DIRSIZ + 1];
    strcpy(buf, path);
    char *p = buf + strlen(buf);
    *p = '/';
    ++p;

    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0 || !strcmp(de.name, ".") || !strcmp(de.name, ".."))
            continue;

        strcpy(p, de.name);
        stat(buf, &st);
        switch (st.type) {
            case T_DEVICE:
            case T_FILE:
                compareAndPrint(buf, name);
                break;
            case T_DIR:
                find(buf, name);
                break;
        }
    }

    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(2, "Usage: find [file name]\n");
        exit(1);
    }
    else {
        find(".", argv[1]);
        exit(0);
    }
}