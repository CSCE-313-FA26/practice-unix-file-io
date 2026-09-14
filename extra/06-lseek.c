/* 06 - lseek() moves the cursor without transferring a byte.
 *
 * read() and write() move the cursor as a side effect. lseek() lets you move
 * it on purpose - back to the start, to a record boundary, or past the end of
 * the file, which is legal and creates a hole.
 *
 * lseek() returns the new absolute offset, so lseek(fd, 0, SEEK_CUR) is the
 * standard way to ask "where am I?" and lseek(fd, 0, SEEK_END) is the standard
 * way to ask "how big is this file?".
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(void)
{
    int fd = open("seekme.txt", O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) { perror("open"); return 1; }
    if (write(fd, "foobar", 6) != 6) perror("write");

    char buf[4];
    buf[3] = '\0';

    lseek(fd, 0, SEEK_SET);
    if (read(fd, buf, 3) != 3) perror("read");
    printf("from offset 0 -> \"%s\"\n", buf);

    lseek(fd, 0, SEEK_SET);                       /* rewind */
    if (read(fd, buf, 3) != 3) perror("read");
    printf("rewound, read again -> \"%s\"   same bytes, on purpose\n", buf);

    lseek(fd, 3, SEEK_SET);                       /* jump */
    if (read(fd, buf, 3) != 3) perror("read");
    printf("from offset 3 -> \"%s\"\n", buf);

    printf("file size (lseek to SEEK_END) = %ld\n", (long)lseek(fd, 0, SEEK_END));

    /* Seek a megabyte past the end, then write one byte. The gap is a hole: it
       reads back as zeros and costs no disk blocks until something is actually
       written into it. */
    lseek(fd, 1024 * 1024, SEEK_END);
    if (write(fd, "!", 1) != 1) perror("write");
    printf("\nseeked 1 MiB past the end and wrote 1 byte -> size is now %ld\n",
           (long)lseek(fd, 0, SEEK_END));

    lseek(fd, 6, SEEK_SET);
    unsigned char hole[10];
    if (read(fd, hole, 10) != 10) perror("read");
    printf("the first 10 bytes of the gap: ");
    for (int i = 0; i < 10; i++) printf("%02x ", hole[i]);

    printf("\n\nZeros the kernel invented; nothing was written there. Check:\n\n");
    fflush(stdout);
    if (system("ls -l seekme.txt; du -h seekme.txt") == -1) perror("system");
    printf("\nA megabyte long, a couple of blocks on disk. That is a sparse file.\n");

    close(fd);
    return 0;
}
