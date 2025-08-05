#include <fcntl.h>
#include <stdio.h>
#include "get_next_line.h"

int     main(int argc, char *argv[])
{
        int             fd;
        char    *nxt_line;

        if (argc != 2)
        {
                printf("Please insert name for 1 file ONLY.\n");
                return (1);
        }
        if (*argv[1] == '0')
                fd = 0;
        else if (*argv[1] == '-')
                fd = 42;
        else
                fd = open(argv[1], O_RDONLY);
        while ((nxt_line = get_next_line(fd)))
        {
                printf("%s", nxt_line);
                free(nxt_line);
        //      sleep(1);
        }
        close(fd);
        return (0);
}
