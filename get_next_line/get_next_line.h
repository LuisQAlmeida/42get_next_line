#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 10

# endif

# include <unistd.h>
# include <stdlib.h>

char    *get_next_line(int fd);
char    *ft_strchr(const char *s, int c);
void    ft_concat(char *nxt_line, char *line, char *buffer, size_t len_buffer);
char    *ft_strjoin_gnl(char *line, char *buffer);
void    ft_excess(char *buffer);
#endif
