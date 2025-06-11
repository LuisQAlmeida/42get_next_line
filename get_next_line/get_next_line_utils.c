#include "get_next_line.h"

int     ft_strlen_nl(char *str)
{
        int     len;

        len = 0;
        while (str[len] && str[len] != '\n')
                len++;
        return (len);
}

char    *ft_build_line(char *buf, char *nxt)
{

        nxt = malloc((ft_strlen_nl(buf) + ft_strlen_nl(nxt) + 1) * sizeof(char));
        if (!nxt)
                return (0);
        return (nxt);
}
