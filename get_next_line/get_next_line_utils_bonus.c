#include "get_next_line_bonus.h"

size_t	ft_strlen_nl(const char *s, int stop_at_nl)
{
	size_t	i;

	i = 0;
	if (!s)
		return (0);
	while (s[i])
	{
		if (stop_at_nl && s[i] == '\n')
			return (i + 1);
		i++;
	}
	return (i);
}

char	*ft_strchr(const char *s, int c)
{
	while (*s)
	{
		if (*s == (char)c)
			return ((char *)s);
		s++;
	}
	if ((char)c == 0)
		return ((char *)s);
	return (NULL);
}

char	*ft_strjoin_gnl(char *s1, char *s2)
{
	size_t	len1;
	size_t	len2;
	char	*new;
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	len1 = ft_strlen_nl(s1, 0);
	len2 = ft_strlen_nl(s2, 1);
	new = malloc(len1 + len2 + 1);
	if (!new)
		return (NULL);
	while (i < len1)
	{
		new[i] = s1[i];
		i++;
	}
	while (j < len2)
		new[i++] = s2[j++];
	new[i] = '\0';
	free(s1);
	return (new);
}

void	ft_cut_buffer(char *buffer)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (buffer[i] && buffer[i] != '\n')
		i++;
	if (buffer[i] == '\n')
		i++;
	while (buffer[i])
		buffer[j++] = buffer[i++];
	while (j < BUFFER_SIZE)
		buffer[j++] = '\0';
}

char	*ft_build_line(char **buf, char *nxt)
{
	nxt = ft_strjoin_gnl(nxt, *buf);
	ft_cut_buffer(*buf);
	return (nxt);
}
