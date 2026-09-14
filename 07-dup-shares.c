/* 08 - dup() copies the ARROW, not the session. And that is how `>` works.
 *
 * Demo 06: two open()s -> two file-table entries -> two cursors.
 * Demo 07: one open() and a dup() -> two descriptor slots pointing at ONE
 * file-table entry -> one shared cursor, and refcnt goes to 2.
 *
 *   fd 3  --\
 *            >--> pos, refcnt=2  --->  foobar.txt
 *   fd 4  --/
 *
 * dup2(fd, 1) is the same trick aimed at slot 1, and it is literally what your
 * shell does between fork() and exec() when it sees `command > file`.
 */
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int main(void)
{
    int fd = open("foobar.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (write(fd, "foobar", 6) != 6) perror("write");
    close(fd);

    /* Part 1: one session, two descriptors. */
    int fd1 = open("foobar.txt", O_RDONLY);
    int fd2 = dup(fd1);
    printf("fd1 = %d, fd2 = dup(fd1) = %d\n\n", fd1, fd2);

    char c;
    if (read(fd1, &c, 1) != 1) perror("read");
    printf("read(fd1) -> '%c'\n", c);
    if (read(fd2, &c, 1) != 1) perror("read");
    printf("read(fd2) -> '%c'   <- 'o', not 'f'. Compare demo 06.\n", c);
    printf("both report cursor %ld - it is the same cursor\n",
           (long)lseek(fd2, 0, SEEK_CUR));
    close(fd1);
    close(fd2);

    /* Part 2: redirection, by hand. */
    printf("\n--- about to redirect stdout ---\n");
    fflush(stdout);

    int saved = dup(1);                                  /* keep a way back */
    int log = open("redirected.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    dup2(log, 1);                                        /* slot 1 now points at the file */
    close(log);

    printf("this printf never reached the terminal\n");
    fflush(stdout);                                      /* flush BEFORE restoring */

    dup2(saved, 1);                                      /* put slot 1 back */
    close(saved);
    printf("--- stdout restored ---\n");

    int back = open("redirected.txt", O_RDONLY);
    char buf[128];
    ssize_t n = read(back, buf, sizeof buf - 1);
    buf[n < 0 ? 0 : n] = '\0';
    close(back);
    printf("redirected.txt contains: %s", buf);

    printf("\nprintf() never knew. It writes to fd 1 and the kernel decides what\n"
           "fd 1 means. That indirection is the whole point of a descriptor.\n");
    return 0;
}
