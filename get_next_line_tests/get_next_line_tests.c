#include <fcntl.h>
#include <stdio.h>
#include "get_next_line.h"

int	main(int argc, char *argv[])
{
	int		fd;
	char	*nxt_line;

	if (argc != 2)
	{
		printf("Please insert name for 1 file ONLY.\n");
		return (1);
	}
	fd = open(argv[1], O_RDONLY);
	if (fd < 0)
	{
		printf("Error opening file");
		return (1);
	}
	printf("fd: %d\n", fd);
	while ((nxt_line = get_next_line(fd)))
	{
		printf("%s", nxt_line);
		free(nxt_line);
	}
	close(fd);
	return (0);
}
