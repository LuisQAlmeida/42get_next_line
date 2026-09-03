#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

/**
 * @file get_next_line.h
 * @brief Interface and support functions for the maintained get_next_line
 * implementation.
 *
 * The maintained implementation returns one dynamically allocated line per
 * call while preserving unread input between consecutive calls.
 *
 * It uses one persistent static buffer internally. This matches the maintained
 * mandatory single-file-descriptor scope and does not provide independent
 * buffered state for interleaved reads from multiple file descriptors.
 *
 * The historical bonus multi-FD implementation is not part of the maintained
 * source.
 */

# ifndef BUFFER_SIZE

/**
 * @def BUFFER_SIZE
 * @brief Maximum number of bytes requested from read() per input operation.
 *
 * The build may override BUFFER_SIZE at compile time. When no override is
 * provided, the maintained default is 10.
 */
#  define BUFFER_SIZE 10

# endif

# include <unistd.h>
# include <stdlib.h>

/**
 * @brief Returns the next line read from a file descriptor.
 *
 * The returned allocation contains the next available line. When a newline is
 * encountered, that newline is included in the returned string. A final line
 * without a terminating newline is still returned before EOF.
 *
 * The caller owns every non-NULL returned allocation and must release it with
 * free().
 *
 * The implementation preserves unread bytes between consecutive calls using
 * one internal persistent buffer:
 *
 * @code
 * static char buffer[BUFFER_SIZE + 1];
 * @endcode
 *
 * Because that buffer is shared across calls rather than maintained per file
 * descriptor, independent interleaved reads from multiple descriptors are
 * outside the maintained scope.
 *
 * @param fd File descriptor from which input is read.
 *
 * @return Newly allocated next line when data is available.
 * @retval NULL If fd is invalid, BUFFER_SIZE is not positive, read() fails,
 * allocation fails, or EOF is reached with no remaining line content.
 */
char	*get_next_line(int fd);

/**
 * @brief Finds the first occurrence of a character in a string.
 *
 * The terminating null character can also be searched for.
 *
 * @param s Null-terminated string to inspect.
 * @param c Character value to locate.
 *
 * @return Pointer to the first matching character.
 * @retval NULL If the character is not present.
 */
char	*ft_strchr(const char *s, int c);

/**
 * @brief Concatenates an existing partial line with bytes from the input
 * buffer.
 *
 * Copies the existing line, when non-NULL, followed by exactly len_buffer
 * bytes from buffer into nxt_line, then appends a null terminator.
 *
 * Memory allocation and ownership are handled by ft_strjoin_gnl(); this
 * function only performs the copy into an already allocated destination.
 *
 * @param nxt_line Destination allocation.
 * @param line Existing partial line, or NULL.
 * @param buffer Input buffer containing bytes to append.
 * @param len_buffer Number of bytes to copy from buffer.
 */
void	ft_concat(char *nxt_line, char *line, char *buffer, size_t len_buffer);

/**
 * @brief Extends a partial line with the next consumable segment of the
 * persistent buffer.
 *
 * The consumed segment ends at the first newline, including that newline when
 * present, or at the end of the buffered string otherwise.
 *
 * A new allocation is created for the combined line. The previous line
 * allocation is freed before returning, including when the new allocation
 * fails.
 *
 * @param line Existing dynamically allocated partial line, or NULL.
 * @param buffer Persistent input buffer.
 *
 * @return Newly allocated combined line.
 * @retval NULL If allocation fails.
 */
char	*ft_strjoin_gnl(char *line, char *buffer);

/**
 * @brief Removes the already consumed segment from the persistent buffer.
 *
 * Bytes through the first newline are discarded. Any unread bytes after that
 * newline are shifted to the beginning of the buffer, and the remaining
 * buffer capacity is cleared.
 *
 * If no newline is present, the buffered content has been fully consumed and
 * the buffer is cleared for the next read().
 *
 * @param buffer Persistent input buffer to update in place.
 */
void	ft_excess(char *buffer);

#endif
