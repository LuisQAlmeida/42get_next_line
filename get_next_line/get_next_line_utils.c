/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luis-almeida <luis-almeida@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 19:24:07 by luis-almeid       #+#    #+#             */
/*   Updated: 2025/06/30 19:24:10 by luis-almeid      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

size_t	ft_strlen_nl(const char *s)
{
	size_t	i;

	i = 0;
	if (!s)
		return (0);
	while (s[i] && s[i] != '\n')
		i++;
	if (s[i] == '\n')
		return (i + 1);
	return (i);
}

char	*ft_strchr(const char *s, int c)
{
	if (!s)
		return (NULL);
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

void	ft_strjoin_copy(char *res, char *s1, char *s2, size_t len2)
{
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	while (s1 && s1[i])
	{
		res[i] = s1[i];
		i++;
	}
	while (j < len2)
	{
		res[i++] = s2[j];
		j++;
	}
	res[i] = '\0';
}

char	*ft_strjoin_nl(char *s1, char *s2)
{
	size_t	len1;
	size_t	len2;
	char	*res;

	len1 = 0;
	while (s1 && s1[len1])
		len1++;
	len2 = 0;
	while (s2[len2] && s2[len2] != '\n')
		len2++;
	if (s2[len2] == '\n')
		len2++;
	res = malloc(len1 + len2 + 1);
	if (!res)
		return (free(s1), NULL);
	ft_strjoin_copy(res, s1, s2, len2);
	free(s1);
	return (res);
}

void	ft_remain_buf(char *buffer)
{
	size_t	i;
	size_t	j;

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
