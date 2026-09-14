/* 03 - the third argument of open() is OCTAL. The missing zero is a real bug.
 *
 * open(path, flags, mode) only uses `mode` when it actually creates the file.
 * 0700 means rwx------. 700 without the leading zero is a decimal number, and
 * decimal 700 is octal 1274 - which sets the sticky bit and gives the group
 * and others permissions you never meant to hand out.
 *
 * Both files below are created by the same line of code with the same digits.
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

static void show(const char *path)
{
    struct stat st;
    if (stat(path, &st) < 0) { perror("stat"); return; }
    printf("  %-10s  mode = %04o\n", path, (unsigned)(st.st_mode & 07777));
}

int main(void)
{
    /* mode is only applied at creation time, so start from nothing. */
    unlink("right.txt");
    unlink("wrong.txt");
    umask(0);                 /* stop the shell's mask from masking the lesson */

    int a = open("right.txt", O_WRONLY | O_CREAT | O_TRUNC, 0700);
    int b = open("wrong.txt", O_WRONLY | O_CREAT | O_TRUNC, 700);
    if (a < 0 || b < 0) { perror("open"); return 1; }
    close(a);
    close(b);

    printf("asked for 0700 and for 700:\n");
    show("right.txt");
    show("wrong.txt");

    printf("\n700 decimal == 01274 octal. The bits land in the wrong columns.\n"
           "Look at the difference:\n\n");
    fflush(stdout);
    if (system("ls -l right.txt wrong.txt") == -1) perror("system");

    printf("\nPermissions are always octal. Write the leading zero, or use the\n"
           "named constants: S_IRWXU, S_IRUSR | S_IWUSR, and so on.\n");
    return 0;
}
