#include <fcntl.h>
#include <stdio.h>

int     main(void)
{
        /*
        FILE    *fh;
        char    c;

        fh = fopen("get_next_line_test1.txt", "r");
        if (fh != NULL)
        {
                while ((c = fgetc(fh)) != EOF)
                        putchar(c);
                fclose(fh);
        }
        else
                printf("Error opening file.\n");
        */
        int     fd;
        char    *line;

        fd = open("get_next_line_test1.txt", O_RDONLY);
        if (fd < 0)
                printf("Error opening file.\n");
        else
        {
                while ((line = get_next_line(fd)) > 0)
                        printf("%s", line);
        }
        return (0);
}
