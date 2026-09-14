/* 01 - a file descriptor is not a pointer. It is an index.
 *
 * Every process has a descriptor table. open() puts an entry in the lowest
 * free slot and hands you back the slot number. That is the whole story: the
 * kernel keeps the interesting state (cursor, flags, which file), you keep a
 * small integer.
 *
 * Slots 0, 1 and 2 are already filled in by the shell, so the first file you
 * open is almost always 3.
 */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(void)
{
    int a = open("/etc/hosts", O_RDONLY);
    if (a < 0) { perror("open"); return 1; }
    printf("first  open  -> fd %d    (0,1,2 were already taken)\n", a);

    int b = open("/etc/hosts", O_RDONLY);
    printf("second open  -> fd %d    same file, different slot\n", b);

    close(a);
    printf("close(%d)\n", a);

    int c = open("/etc/hosts", O_RDONLY);
    printf("third  open  -> fd %d    <- the freed slot came back, not %d\n",
           c, b + 1);

    /* Failure is reported the same way: -1, and errno explains it. */
    int bad = open("/no/such/file", O_RDONLY);
    printf("\nopen(\"/no/such/file\") -> %d\n", bad);
    fflush(stdout);           /* perror writes to fd 2, which is unbuffered;
                                 flush fd 1 first or the two cross. */
    perror("what went wrong");

    close(b);
    close(c);

    printf("\nfd == -1 is the only error check open() gives you. Use it every\n"
           "time; a bad fd does not announce itself until the read() fails.\n");
    return 0;
}
