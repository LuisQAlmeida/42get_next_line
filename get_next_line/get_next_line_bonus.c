#include "get_next_line_bonus.h"

static int      err_handle(int fd)
{
        if (fd < 0 || fd >= FD_MAX)
                return (1);
        if (BUFFER_SIZE <= 0 || BUFFER_SIZE > MAX_BUFFER_SIZE)
                return (1);
        return (0);
}

char    *get_next_line(int fd)
{
        static char     *buffers[FD_MAX];
        char            buffer[BUFFER_SIZE + 1];
        ssize_t         bytes_read;
        char            *nxt_line;

        nxt_line = NULL;
        if (err_handle(fd))
                return (NULL);
        while (!ft_strchr(buffers[fd], '\n'))
	{
		bytes_read = read(fd, buffer, BUFFER_SIZE);
		if (bytes_read < 0)
			return (free(buffers[fd]), buffers[fd] = NULL, free(nxt_line), NULL);
		if (bytes_read == 0)
			break ;
		buffer[bytes_read] = '\0';
		buffers[fd] = ft_strjoin_gnl(buffers[fd], buffer);
		if (!buffers[fd])
			return (free(nxt_line), NULL);
	}
	nxt_line = ft_build_line(&buffers[fd], nxt_line);
        if (!nxt_line)
                return (NULL);
        return (nxt_line);
}
