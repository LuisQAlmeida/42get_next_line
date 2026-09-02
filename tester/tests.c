#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "get_next_line.h"

#define MAX_EXPECTED_LINES 8
#define PREVIEW_LENGTH 80
#define GIANT_FIXTURE "tester/fixtures/giant_line.txt"
#define GIANT_LENGTH 20000

typedef struct s_case
{
	const char	*name;
	const char	*content;
	size_t		content_length;
	const char	*expected[MAX_EXPECTED_LINES];
	size_t		expected_count;
	int			free_content;
}	t_case;

static int	write_all(int fd, const char *content, size_t length)
{
	ssize_t	written;
	size_t	offset;

	offset = 0;
	while (offset < length)
	{
		written = write(fd, content + offset, length - offset);
		if (written <= 0)
			return (-1);
		offset += (size_t)written;
	}
	return (0);
}

static int	read_all(int fd, char *buffer, size_t length)
{
	ssize_t	bytes_read;
	size_t	offset;

	offset = 0;
	while (offset < length)
	{
		bytes_read = read(fd, buffer + offset, length - offset);
		if (bytes_read <= 0)
			return (-1);
		offset += (size_t)bytes_read;
	}
	return (0);
}

static int	create_fixture(const t_case *test)
{
	char	path[] = "/tmp/42gnl_case_XXXXXX";
	int		fd;

	fd = mkstemp(path);
	if (fd < 0)
		return (-1);
	if (unlink(path) != 0)
	{
		close(fd);
		return (-1);
	}
	if (write_all(fd, test->content, test->content_length) != 0)
	{
		close(fd);
		return (-1);
	}
	if (lseek(fd, 0, SEEK_SET) < 0)
	{
		close(fd);
		return (-1);
	}
	return (fd);
}

static size_t	line_length(const char *line)
{
	size_t	length;

	length = 0;
	while (line && line[length])
		length++;
	return (length);
}

static int	line_matches(const char *actual, const char *expected)
{
	size_t	expected_length;
	size_t	actual_length;

	if (!actual || !expected)
		return (0);
	expected_length = line_length(expected);
	actual_length = line_length(actual);
	if (actual_length != expected_length)
		return (0);
	if (memcmp(actual, expected, expected_length) != 0)
		return (0);
	return (1);
}

static void	print_preview(const char *line)
{
	size_t	i;

	if (!line)
	{
		printf("NULL\n");
		return ;
	}
	i = 0;
	while (line[i] && i < PREVIEW_LENGTH)
	{
		if (line[i] == '\n')
			printf("\\n");
		else if (line[i] == '\t')
			printf("\\t");
		else if ((unsigned char)line[i] < 32
			|| (unsigned char)line[i] > 126)
			printf("\\x%02x", (unsigned char)line[i]);
		else
			putchar(line[i]);
		i++;
	}
	if (line[i])
		printf("...");
	printf("\n");
}

static int	report_line_failure(
	const char *name,
	size_t index,
	const char *expected,
	const char *actual)
{
	printf("[FAIL] %s\n", name);
	printf("       line:            %zu\n", index + 1);
	printf("       expected length: %zu\n", line_length(expected));
	printf("       actual length:   %zu\n", line_length(actual));
	printf("       expected:        ");
	print_preview(expected);
	printf("       actual:          ");
	print_preview(actual);
	return (1);
}

static int	verify_expected_lines(int fd, const t_case *test)
{
	char	*line;
	size_t	i;

	i = 0;
	while (i < test->expected_count)
	{
		line = get_next_line(fd);
		if (!line_matches(line, test->expected[i]))
		{
			report_line_failure(test->name, i, test->expected[i], line);
			free(line);
			return (1);
		}
		free(line);
		i++;
	}
	return (0);
}

static int	verify_eof(int fd, const char *name, size_t expected_count)
{
	char	*line;

	line = get_next_line(fd);
	if (line)
	{
		printf("[FAIL] %s\n", name);
		printf("       expected EOF after %zu line(s)\n", expected_count);
		printf("       actual: ");
		print_preview(line);
		free(line);
		return (1);
	}
	line = get_next_line(fd);
	if (line)
	{
		printf("[FAIL] %s\n", name);
		printf("       repeated post-EOF call returned data\n");
		free(line);
		return (1);
	}
	return (0);
}

static int	execute_case(const t_case *test)
{
	int	fd;
	int	failed;

	fd = create_fixture(test);
	if (fd < 0)
	{
		printf("[FAIL] %s: fixture creation failed\n", test->name);
		return (1);
	}
	failed = verify_expected_lines(fd, test);
	if (!failed)
		failed = verify_eof(fd, test->name, test->expected_count);
	close(fd);
	return (failed);
}

static int	wait_for_child(pid_t pid, const char *name)
{
	int	status;

	if (waitpid(pid, &status, 0) < 0)
	{
		printf("[FAIL] %s: waitpid failed\n", name);
		return (1);
	}
	if (!WIFEXITED(status))
	{
		printf("[FAIL] %s: test process terminated abnormally\n", name);
		return (1);
	}
	if (WEXITSTATUS(status) != 0)
		return (1);
	printf("[PASS] %s\n", name);
	return (0);
}

static int	run_isolated_case(const t_case *test)
{
	pid_t	pid;
	int		failed;

	fflush(NULL);
	pid = fork();
	if (pid < 0)
	{
		printf("[FAIL] %s: fork failed\n", test->name);
		return (1);
	}
	if (pid == 0)
	{
		failed = execute_case(test);
		if (test->free_content)
			free((void *)test->content);
		fflush(NULL);
		_exit(failed != 0);
	}
	return (wait_for_child(pid, test->name));
}

static int	run_dynamic_line_case(const char *name, size_t length)
{
	t_case	test;
	char	*content;
	int		failed;

	content = malloc(length + 1);
	if (!content)
	{
		printf("[FAIL] %s: allocation failed\n", name);
		return (1);
	}
	memset(content, 'x', length);
	content[length] = '\0';
	memset(&test, 0, sizeof(test));
	test.name = name;
	test.content = content;
	test.content_length = length;
	test.expected[0] = content;
	test.expected_count = 1;
	test.free_content = 1;
	failed = run_isolated_case(&test);
	free(content);
	return (failed);
}

static char	*load_file(const char *path, size_t *length)
{
	off_t	end;
	char	*content;
	int		fd;

	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (NULL);
	end = lseek(fd, 0, SEEK_END);
	if (end < 0 || lseek(fd, 0, SEEK_SET) < 0)
	{
		close(fd);
		return (NULL);
	}
	content = malloc((size_t)end + 1);
	if (!content)
	{
		close(fd);
		return (NULL);
	}
	if (read_all(fd, content, (size_t)end) != 0)
	{
		free(content);
		close(fd);
		return (NULL);
	}
	close(fd);
	content[end] = '\0';
	*length = (size_t)end;
	return (content);
}

static int	execute_giant_line(void)
{
	char	*expected;
	char	*line;
	size_t	expected_length;
	int		fd;
	int		failed;

	expected = load_file(GIANT_FIXTURE, &expected_length);
	if (!expected)
	{
		printf("[FAIL] giant line: fixture load failed\n");
		return (1);
	}
	if (expected_length != GIANT_LENGTH)
	{
		printf("[FAIL] giant line: expected fixture length %d, got %zu\n",
			GIANT_LENGTH, expected_length);
		free(expected);
		return (1);
	}
	fd = open(GIANT_FIXTURE, O_RDONLY);
	if (fd < 0)
	{
		printf("[FAIL] giant line: fixture open failed\n");
		free(expected);
		return (1);
	}
	line = get_next_line(fd);
	failed = 0;
	if (!line_matches(line, expected))
		failed = report_line_failure("giant line", 0, expected, line);
	free(line);
	if (!failed)
		failed = verify_eof(fd, "giant line", 1);
	close(fd);
	free(expected);
	return (failed);
}

static int	execute_negative_fd(void)
{
	char	*line;

	line = get_next_line(-1);
	if (line)
	{
		printf("[FAIL] invalid negative fd: expected NULL\n");
		free(line);
		return (1);
	}
	return (0);
}

static int	execute_closed_fd(void)
{
	char	*line;
	int		pipefd[2];
	int		closed_fd;

	if (pipe(pipefd) != 0)
	{
		printf("[FAIL] closed fd read error: pipe failed\n");
		return (1);
	}
	closed_fd = pipefd[0];
	close(pipefd[0]);
	close(pipefd[1]);
	line = get_next_line(closed_fd);
	if (line)
	{
		printf("[FAIL] closed fd read error: expected NULL\n");
		free(line);
		return (1);
	}
	return (0);
}

static int	run_isolated_function(const char *name, int (*function)(void))
{
	pid_t	pid;
	int		failed;

	fflush(NULL);
	pid = fork();
	if (pid < 0)
	{
		printf("[FAIL] %s: fork failed\n", name);
		return (1);
	}
	if (pid == 0)
	{
		failed = function();
		fflush(NULL);
		_exit(failed != 0);
	}
	return (wait_for_child(pid, name));
}

static int	run_static_cases(void)
{
	static const t_case	tests[] = {
		{
			"empty file", "", 0, {NULL}, 0, 0
		},
		{
			"single byte without newline", "A", 1, {"A"}, 1, 0
		},
		{
			"newline only", "\n", 1, {"\n"}, 1, 0
		},
		{
			"multiple lines",
			"alpha\nbeta\ngamma\n",
			17,
			{"alpha\n", "beta\n", "gamma\n"},
			3,
			0
		},
		{
			"final line without newline",
			"alpha\nbeta\ngamma",
			16,
			{"alpha\n", "beta\n", "gamma"},
			3,
			0
		},
		{
			"consecutive empty lines",
			"first\n\n\nlast\n",
			13,
			{"first\n", "\n", "\n", "last\n"},
			4,
			0
		}
	};
	size_t	i;
	int		failures;

	i = 0;
	failures = 0;
	while (i < sizeof(tests) / sizeof(tests[0]))
	{
		failures += run_isolated_case(&tests[i]);
		i++;
	}
	return (failures);
}

int	main(void)
{
	size_t	short_length;
	int		failures;
	int		total;

	failures = 0;
	total = 13;
	failures += run_static_cases();
	short_length = 1;
	if (BUFFER_SIZE > 1)
		short_length = BUFFER_SIZE - 1;
	failures += run_dynamic_line_case(
		"short boundary line", short_length);
	failures += run_dynamic_line_case(
		"exact BUFFER_SIZE line", BUFFER_SIZE);
	failures += run_dynamic_line_case(
		"BUFFER_SIZE plus one line", BUFFER_SIZE + 1);
	failures += run_dynamic_line_case(
		"line spanning many reads", (BUFFER_SIZE * 4) + 3);
	failures += run_isolated_function("giant line", execute_giant_line);
	failures += run_isolated_function(
		"invalid negative fd", execute_negative_fd);
	failures += run_isolated_function(
		"closed fd read error", execute_closed_fd);
	printf("\n%d/%d tests passed (BUFFER_SIZE=%d)\n",
		total - failures, total, BUFFER_SIZE);
	if (failures != 0)
		return (1);
	return (0);
}
