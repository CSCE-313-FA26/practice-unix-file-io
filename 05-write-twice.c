/* 05 - writing is reading with the arrow turned around. Same cursor, same count.
 *
 * This is the slide-18 question: the same two bytes written twice give "abab",
 * for exactly the reason two reads gave "fo" then "ob" in demo 04. One cursor,
 * and write() advances it too.
 *
 * The slide also asks what changes if you write `char *buf = "ab"` instead of
 * `char buf[] = {'a','b'}`. It is not a style choice - sizeof stops measuring
 * the string and starts measuring the pointer. That number is printed below.
 */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define BIG 5000

int main(void)
{
    /* Slide 18, verbatim. */
    char buf[] = { 'a', 'b' };

    int fd = open("abab.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) { perror("open"); return 1; }

    ssize_t w1 = write(fd, buf, sizeof buf);
    printf("write #1 -> %zd byte(s), cursor is now at %ld\n",
           w1, (long)lseek(fd, 0, SEEK_CUR));
    ssize_t w2 = write(fd, buf, sizeof buf);
    printf("write #2 -> %zd byte(s), cursor is now at %ld\n",
           w2, (long)lseek(fd, 0, SEEK_CUR));
    close(fd);

    char back[16];
    fd = open("abab.txt", O_RDONLY);
    ssize_t n = read(fd, back, sizeof back - 1);
    back[n < 0 ? 0 : n] = '\0';
    close(fd);
    printf("abab.txt now contains \"%s\"  <- not \"ab\". The second write did\n"
           "not start over; it started where the first one stopped.\n", back);

    /* The sizeof trap from the same slide. No bad write here - just the two
       numbers, because the number is the whole bug. */
    const char *ptr = "ab";
    printf("\nsizeof(char buf[] = {'a','b'}) = %zu   <- the array, 2 bytes\n",
           sizeof buf);
    printf("sizeof(char *ptr  = \"ab\")      = %zu   <- the POINTER, not the text\n",
           sizeof ptr);
    printf("The second form hands write() an 8 and it will happily read 8 bytes\n"
           "out of a 3-byte object. Use strlen(), or write the array.\n");

    /* Same deal as read(): the return value is the truth, not your request. */
    char blob[BIG];
    for (int i = 0; i < BIG; i++) blob[i] = 'a' + (i % 26);
    fd = open("big.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    ssize_t w = write(fd, blob, BIG);
    close(fd);
    printf("\nasked write() for %d bytes -> it took %zd\n", BIG, w);
    printf("To a regular file it usually takes all of them. To a pipe, a socket\n"
           "or a terminal it often takes fewer, and that is not an error either.\n"
           "So a write is a loop, same as a read. Demo 08 has the loop.\n");
    return 0;
}
