/* 02 - stdin, stdout and stderr are just fds 0, 1 and 2.
 *
 * There is nothing special about printf(). It formats a string and then hands
 * the bytes to write() on fd 1. You can skip the middleman.
 *
 * The point to make in class: run it twice, the second time with stdout sent
 * somewhere else. The program does not change and does not know.
 *
 *     ./02-three-standard-files
 *     ./02-three-standard-files > /dev/null
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* write() returns how many bytes it actually took. Never ignore it.
   The fflush is because printf() buffers and write() does not, so without it
   these lines come out in the wrong order when stdout is a pipe.
   09-buffer-vs-syscall.c is entirely about that; here it would just be noise. */
static void say(int fd, const char *s)
{
    fflush(stdout);
    ssize_t n = write(fd, s, strlen(s));
    if (n < 0) perror("write");
}

int main(void)
{
    say(1, "write(1, ...) -> stdout, no stdio involved\n");
    say(2, "write(2, ...) -> stderr\n");

    printf("printf(...)   -> also fd 1, via write()\n");
    fflush(stdout);
    fprintf(stderr, "fprintf(stderr, ...) -> also fd 2\n");

    printf("\nNow run it again with:  > /dev/null\n"
           "Only the fd 2 lines survive. The shell swapped out slot 1 before\n"
           "this program started, and every printf above followed it blindly.\n"
           "Demo 07 shows how the shell does that swap.\n");
    return 0;
}
