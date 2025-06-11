#include "get_next_line.h"

char    *get_next_line(int fd)
{
        static char     buffer[BUFFER_SIZE + 1];
        ssize_t         bytes_read;
        char            *nxt_line;
        int             full_line;

        full_line = 0;
        if (fd < 0 || BUFFER_SIZE <= 0 || BUFFER_SIZE > MAX_BUFFER_SIZE)
                return (NULL);
        while (!full_line)
        {
                bytes_read = read(fd, buffer, BUFFER_SIZE);
                if (!bytes_read)
                        return (NULL);
                else if (!bytes_read == -1)
                        return (NULL);
                else
                {
                        ft_build_line(buffer, nxt_line);
                }
        }
        return (nxt_line);
}
