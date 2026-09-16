/* 09 - a FIFO is a pipe with a name, so strangers can find it.
 *
 * Slides 29-35 of CSCE-313-6. A pipe() can only be shared by a parent and the
 * children it forks, because the only way to reach it is to inherit its
 * descriptors. mkfifo() gives the pipe a filename instead, and then ANY process
 * that can open() that name can join in - no common ancestor needed.
 *
 * This is the slide's hello-server, both halves in one file. Run each half in
 * its own terminal - two separate commands, nothing forked:
 *
 *     terminal A:  ./09-fifo server
 *     terminal B:  ./09-fifo client
 *
 * Three things the slide code does not let you see:
 *   - open() on a FIFO blocks until the other end is opened too. Whichever side
 *     you start first just sits there waiting for the other.
 *   - `ls -l my_fifo` shows type `p` and size 0, always. The bytes pass through
 *     the kernel and are never stored: read once, gone (slide 35).
 *   - read() returning 0 means every writer has closed its end. The slide's loop
 *     only checks for "not 4 bytes" and `continue`s, so a client that dies before
 *     sending its 0 leaves that server spinning at 100% CPU. This one stops.
 *
 * The slide uses /tmp/MY_FIFO. This uses ./my_fifo so it shows up in `ls -l`
 * next to everything else, and two people on one shared server cannot collide.
 */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define FIFO "my_fifo"

static int server(void)
{
    /* S_IRUSR | S_IWUSR is 0600: the owner may read and write, nobody else.
       EEXIST is fine - it is left over from a server you Ctrl-C'd last time. */
    if (mkfifo(FIFO, S_IRUSR | S_IWUSR) < 0 && errno != EEXIST) {
        perror("mkfifo " FIFO);
        return 1;
    }

    printf("server: made %s. Waiting in open() for a writer...\n", FIFO);
    fflush(stdout);                 /* say it BEFORE blocking, not after */

    int fifo = open(FIFO, O_RDONLY);
    if (fifo < 0) { perror("open " FIFO); unlink(FIFO); return 1; }
    printf("server: a writer showed up, open() returned fd %d\n", fifo);

    int req;
    ssize_t n;
    while ((n = read(fifo, &req, sizeof req)) == sizeof req) {
        if (req == 0) { printf("server: got 0 -> shutting down\n"); break; }
        printf("server: got %d -> hello\n", req);
    }
    if (n == 0)
        printf("server: read() returned 0 - every writer closed its end (EOF)\n");
    else if (n < 0)
        perror("read");

    close(fifo);
    unlink(FIFO);                   /* the name goes; the pipe goes with it */
    printf("server: deleted %s\n", FIFO);
    return 0;
}

static int client(void)
{
    printf("client: waiting in open() for a reader...\n");
    fflush(stdout);

    int fifo = open(FIFO, O_WRONLY);
    if (fifo < 0) {
        int err = errno;
        perror("open " FIFO);
        if (err == ENOENT)
            fprintf(stderr, "(nothing by that name yet - start `./09-fifo server`"
                            " first, it is the one that creates it)\n");
        return 1;
    }
    printf("client: a reader showed up, open() returned fd %d\n", fifo);

    for (int i = 5; i >= 0; i--) {
        if (write(fifo, &i, sizeof i) != sizeof i) { perror("write"); return 1; }
        printf("client: sent %d\n", i);
        if (i > 0) sleep(1);
    }
    close(fifo);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc == 2 && strcmp(argv[1], "server") == 0) return server();
    if (argc == 2 && strcmp(argv[1], "client") == 0) return client();

    fprintf(stderr, "usage: %s server    (in one terminal)\n"
                    "       %s client    (in another)\n", argv[0], argv[0]);
    return 2;
}
