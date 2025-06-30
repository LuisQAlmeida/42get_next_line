/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luis-almeida <luis-almeida@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 19:24:25 by luis-almeid       #+#    #+#             */
/*   Updated: 2025/06/30 19:24:27 by luis-almeid      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static void	ft_free_ptr(char **ptr)
{
	if (ptr && *ptr)
	{
		free(*ptr);
		*ptr = NULL;
	}
}

static char	*ft_build_line(char *buffer, char *line)
{
	char	*tmp;

	tmp = ft_strjoin_nl(line, buffer);
	if (!tmp)
		return (NULL);
	ft_remain_buf(buffer);
	return (tmp);
}

char	*get_next_line(int fd)
{
	static char	buffer[BUFFER_SIZE + 1];
	char		*line;
	ssize_t		bytes;

	line = NULL;
	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	while (1)
	{
		if (!buffer[0])
		{
			bytes = read(fd, buffer, BUFFER_SIZE);
			if (bytes < 0)
				return (ft_free_ptr(&line), NULL);
			if (bytes == 0)
				break ;
			buffer[bytes] = '\0';
		}
		line = ft_build_line(buffer, line);
		if (!line || ft_strchr(line, '\n'))
			break ;
	}
	if (line && *line)
		return (line);
	return (ft_free_ptr(&line), NULL);
}
