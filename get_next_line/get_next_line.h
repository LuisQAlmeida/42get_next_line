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

char    *get_next_line(int fd);
size_t  ft_strlen_gnl(const char *s);
char    *ft_strchr(const char *s, int c);
void    ft_concat(char *nxt_line, char *line, char *buffer, size_t len_buffer);
char    *ft_strjoin_gnl(char *line, char *buffer);
void    ft_excess(char *buffer);
#endif
