/* 08 - put it together: cat, and therefore cp, in about twenty lines.
 *
 *     ./08-mini-cat foobar.txt              behaves like cat
 *     ./08-mini-cat foobar.txt > copy.txt   behaves like cp
 *
 * Nothing new here - open, a read loop that stops at 0, a write loop that
 * survives short counts, close. That loop is every file copy, every network
 * receive, every tool you will write this semester.
 */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

/* write() may take fewer bytes than offered, so keep offering the rest. */
static int write_all(int fd, const char *buf, size_t n)
{
    size_t done = 0;
    while (done < n) {
        ssize_t w = write(fd, buf + done, n - done);
        if (w < 0) return -1;
        done += (size_t)w;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    const char *path = (argc > 1) ? argv[1] : "foobar.txt";

    int fd = open(path, O_RDONLY);
    if (fd < 0) { perror(path); return 1; }

    char buf[4096];
    ssize_t n;
    while ((n = read(fd, buf, sizeof buf)) > 0) {
        if (write_all(1, buf, (size_t)n) < 0) { perror("write"); return 1; }
    }
    if (n < 0) { perror("read"); return 1; }

    close(fd);
    return 0;
}
