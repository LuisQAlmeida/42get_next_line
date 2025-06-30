/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luis-almeida <luis-almeida@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 20:15:01 by luis-almeid       #+#    #+#             */
/*   Updated: 2025/06/19 20:15:03 by luis-almeid      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 10

# endif

# ifndef MAX_BUFFER_SIZE
#  define MAX_BUFFER_SIZE 10000000

# endif
# include <unistd.h>
# include <stdlib.h>

char	*get_next_line(int fd);
size_t	ft_strlen_nl(const char *s);
char	*ft_strchr(const char *s, int c);
void	ft_strjoin_copy(char *res, char *s1, char *s2, size_t len2);
char	*ft_strjoin_nl(char *s1, char *s2);
void	ft_remain_buf(char *buffer);
#endif
