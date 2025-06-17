#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 42

# endif

# ifndef MAX_BUFFER_SIZE
#  define MAX_BUFFER_SIZE 10000000

# endif
# include <unistd.h>
# include <stdlib.h>

char	*get_next_line(int fd);
char	*ft_build_line(char *buffer, char *nxt_line);
char	*ft_strchr(const char *s, int c);
char	*ft_strjoin_gnl(char *s1, char *s2);
size_t	ft_strlen_nl(const char *s, int stop_at_nl);
void	ft_cut_buffer(char *buffer);
#endif
