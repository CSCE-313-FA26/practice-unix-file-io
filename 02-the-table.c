/* 02 - the descriptor table, printed.
 *
 * Demo 01 printed the slot numbers. This prints the table they index into.
 *
 * /proc/self/fd is a directory the kernel invents on demand: one symlink per
 * open descriptor, named after the slot number, pointing at whatever that slot
 * currently refers to. Watch a row appear when open() runs and vanish when
 * close() runs, leaving the gap that open() will reuse next.
 *
 * 0, 1 and 2 are ordinary rows in it. They point at your terminal (/dev/pts/N)
 * only because the shell filled them in before this program started - run this
 * with `| cat` and watch slot 1 turn into a pipe.
 *
 * (/proc is not on any disk. It is the kernel showing you its own data
 * structures as files, which is the "even the kernel is a file" line from
 * earlier in the deck, made literal.)
 */
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

static int ascending(const void *a, const void *b)
{
    return *(const int *)a - *(const int *)b;
}

static void show_table(const char *when)
{
    printf("\n--- descriptor table %s ---\n", when);

    DIR *d = opendir("/proc/self/fd");
    if (!d) { perror("opendir"); return; }

    /* Reading the table needs a descriptor of its own, and it lands in a free
       slot like anything else. It is an artifact of looking, not part of what
       we are looking at, so leave it out. */
    int looking = dirfd(d);

    int fds[64];
    int n = 0;
    struct dirent *e;
    while ((e = readdir(d)) != NULL && n < (int)(sizeof fds / sizeof fds[0])) {
        if (e->d_name[0] == '.') continue;       /* "." and ".." */
        int fd = atoi(e->d_name);
        if (fd != looking) fds[n++] = fd;
    }
    qsort(fds, (size_t)n, sizeof fds[0], ascending);

    for (int i = 0; i < n; i++) {
        char link[64], target[256];
        snprintf(link, sizeof link, "/proc/self/fd/%d", fds[i]);
        ssize_t len = readlink(link, target, sizeof target - 1);
        if (len < 0) { perror("readlink"); continue; }
        target[len] = '\0';
        printf("   %d -> %s\n", fds[i], target);
    }
    closedir(d);
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
