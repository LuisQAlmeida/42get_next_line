#include "get_next_line.h"

char	*get_next_line(int fd)
{
	static char	buffer[BUFFER_SIZE + 1];
	ssize_t		bytes_read;
	char		*nxt_line;

	nxt_line = NULL;
	if (fd < 0 || BUFFER_SIZE <= 0 || BUFFER_SIZE > MAX_BUFFER_SIZE)
		return (NULL);
	while (1)
	{
		if (!buffer[0])
		{
			bytes_read = read(fd, buffer, BUFFER_SIZE);
			if (bytes_read <= 0)
				break ;
			buffer[bytes_read] = '\0';
		}
		nxt_line = ft_build_line(buffer, nxt_line);
		if (!nxt_line || ft_strchr(nxt_line, '\n'))
			break ;
	}
	if (!nxt_line || !nxt_line[0])
		return (free(nxt_line), NULL);
	return (nxt_line);
}
