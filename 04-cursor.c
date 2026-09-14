/* 04 - reading twice does not give you the same bytes twice.
 *
 * This is the "foobar" example from the slides. Each open file has a cursor,
 * the kernel starts it at 0, and every read() moves it forward by however many
 * bytes it handed over. You never pass an offset to read(); the offset is
 * state the kernel is holding for you.
 *
 * Two reads of 2 bytes give "fo" then "ob". Not "fo" then "foob".
 */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

static void make_foobar(void)
{
    int fd = open("foobar.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) { perror("open"); _exit(1); }
    if (write(fd, "foobar", 6) != 6) perror("write");
    close(fd);
}

int main(void)
{
    make_foobar();

    int fd = open("foobar.txt", O_RDONLY);
    if (fd < 0) { perror("open"); return 1; }

    char buf[3];              /* 2 bytes plus room for a terminator */
    for (int i = 1; i <= 4; i++) {
        ssize_t n = read(fd, buf, 2);
        if (n < 0) { perror("read"); return 1; }
        buf[n] = '\0';
        printf("read #%d -> %zd byte(s), buf = \"%s\", cursor is now at %ld\n",
               i, n, buf, (long)lseek(fd, 0, SEEK_CUR));
    }

    close(fd);
    printf("\nThe last read returned 0. That is end of file, and it is not an\n"
           "error - errors are negative. 0 is how every read loop knows to stop.\n");
    return 0;
}
