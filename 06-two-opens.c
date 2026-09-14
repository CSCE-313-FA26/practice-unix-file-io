/* 07 - two open()s of the same file are two independent sessions.
 *
 * This is the three-table picture from the slides, made visible. Each open()
 * creates its own entry in the shared file table, and the cursor lives in that
 * entry - not in the file itself. So the two descriptors below both start at
 * offset 0 and both read 'f'.
 *
 *   descriptor table        file table          v-node table
 *   (per process)           (shared)            (shared)
 *      fd 3  ------------>  pos=0, refcnt=1 --\
 *      fd 4  ------------>  pos=0, refcnt=1 --+--> foobar.txt  (one entry)
 *
 * Demo 07 is the same experiment with the arrows drawn differently.
 */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(void)
{
    int fd = open("foobar.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) { perror("open"); return 1; }
    if (write(fd, "foobar", 6) != 6) perror("write");
    close(fd);

    int fd1 = open("foobar.txt", O_RDONLY);
    int fd2 = open("foobar.txt", O_RDONLY);
    if (fd1 < 0 || fd2 < 0) { perror("open"); return 1; }
    printf("opened the same file twice: fd1 = %d, fd2 = %d\n\n", fd1, fd2);

    char c;
    if (read(fd1, &c, 1) != 1) perror("read");
    printf("read(fd1) -> '%c'   fd1 cursor %ld, fd2 cursor %ld\n",
           c, (long)lseek(fd1, 0, SEEK_CUR), (long)lseek(fd2, 0, SEEK_CUR));

    if (read(fd2, &c, 1) != 1) perror("read");
    printf("read(fd2) -> '%c'   <- 'f' again. fd2 never moved.\n", c);

    if (read(fd1, &c, 1) != 1) perror("read");
    printf("read(fd1) -> '%c'   fd1 is at 2 while fd2 is at 1\n", c);

    close(fd1);
    close(fd2);
    printf("\nOne file on disk, one v-node, two file-table entries, two cursors.\n"
           "Every open() gets its own place in the file.\n");
    return 0;
}
