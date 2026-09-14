/* 09 - printf() and write() do not arrive in the order you wrote them.
 *
 * printf() puts bytes in a buffer inside your process. write() is a system
 * call and goes straight out. Mixing them means the buffered text can appear
 * long after the unbuffered text - at exit(), in the worst case.
 *
 * To a terminal stdout is LINE buffered, so this looks fine. To a pipe or a
 * file it is FULLY buffered, and the order collapses. Run both:
 *
 *     ./09-buffer-vs-syscall
 *     ./09-buffer-vs-syscall | cat
 *
 * Same program, same machine, different output. This is the file-I/O twin of
 * the fork() doubling bug from the process-API deck.
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static void say(const char *s)
{
    if (write(1, s, strlen(s)) < 0) perror("write");
}

int main(void)
{
    printf("1. printf - sits in the stdio buffer\n");
    say  ("2. write  - already in the kernel\n");
    printf("3. printf - still in the buffer, behind line 1\n");
    say  ("4. write  - already in the kernel\n");

    /* Uncomment and the order is restored, because the buffer is emptied
       before each write() gets to jump the queue:                          */
    /* fflush(stdout); */

    printf("\nPiped, you get 2 4 1 3: the write()s went out immediately and the\n"
           "printf()s were all flushed at exit. fflush(stdout) before any raw\n"
           "write() - and before fork() - is the fix.\n");
    return 0;
}
