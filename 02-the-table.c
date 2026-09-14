/* 02 - the descriptor table, printed.
 *
 * /proc/self/fd is a directory the kernel invents on demand: one symlink per
 * open descriptor, named after the slot number, pointing at whatever that slot
 * currently refers to. It is the left-hand box from the slides, and you can
 * just look at it.
 *
 * Two things to notice while it is on screen:
 *
 *   - 0, 1 and 2 point at your terminal (/dev/pts/N). They are not special.
 *     They are slots the shell filled in before this program started. Run this
 *     with `| cat` and they turn into pipes.
 *   - /proc itself is not on any disk. The kernel is exposing its own data
 *     structures as files, because on Unix everything is a file.
 *
 * Demos 06 and 07 change this table. Come back here after each of them.
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

static void show_table(const char *when)
{
    printf("\n--- descriptor table %s ---\n", when);
    fflush(stdout);
    /* ls runs in a forked child, which INHERITS this table - that is why it
       can see our descriptors at all. It also opens the directory it is
       reading, which lands in whatever slot is free and would look like one
       of ours, so that row is filtered out below. */
    if (system("ls -l /proc/self/fd"
               " | grep -v '^total'"
               " | grep -v '/proc/[0-9]*/fd'") == -1) perror("system");
}

int main(void)
{
    show_table("at startup");

    int a = open("/etc/hosts", O_RDONLY);
    int b = open("/etc/hosts", O_RDONLY);
    if (a < 0 || b < 0) { perror("open"); return 1; }
    printf("\nopened /etc/hosts twice -> fds %d and %d\n", a, b);
    show_table("after two open()s");

    close(a);
    printf("\nclose(%d)\n", a);
    show_table("after one close()");

    close(b);
    printf("\nA descriptor is a row in that table. open() adds one, close()\n"
           "removes one, and the integer you hold is just the row number.\n");
    return 0;
}
