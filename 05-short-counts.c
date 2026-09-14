/* 05 - read() gives you AT MOST what you asked for. Usually less.
 *
 * A short count is not an error and it is not rare: it is what you get at end
 * of file, from a pipe, from a socket, from a terminal, and from a signal
 * arriving mid-call. Code that assumes read() fills the buffer works on small
 * files and breaks on everything else.
 *
 * The only correct shape is the loop at the bottom of this file. Learn it once.
 */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define BIG 5000

static void make_files(void)
{
    int fd = open("tiny.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (write(fd, "foobar", 6) != 6) perror("write");
    close(fd);

    char blob[BIG];
    for (int i = 0; i < BIG; i++) blob[i] = 'a' + (i % 26);
    fd = open("big.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (write(fd, blob, BIG) != BIG) perror("write");
    close(fd);
}

int main(void)
{
    make_files();

    /* One read, a generous buffer, a 6-byte file. */
    char buf[512];
    int fd = open("tiny.txt", O_RDONLY);
    ssize_t n = read(fd, buf, sizeof buf);
    printf("asked for %zu bytes from a 6-byte file -> got %zd\n", sizeof buf, n);
    close(fd);

    /* The loop. Keep going until read() returns 0. */
    fd = open("big.txt", O_RDONLY);
    long total = 0;
    int calls = 0;
    while ((n = read(fd, buf, sizeof buf)) > 0) {
        total += n;
        calls++;
    }
    if (n < 0) perror("read");
    close(fd);

    printf("read a %d-byte file with a %zu-byte buffer:\n", BIG, sizeof buf);
    printf("  %d calls, %ld bytes, last call returned a partial %d\n",
           calls, total, BIG % (int)sizeof buf);

    printf("\nwrite() short-counts too, for the same reasons. If you only ever\n"
           "remember one thing from this deck: loop until read() returns 0.\n");
    return 0;
}
