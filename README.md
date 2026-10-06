*This project has been created as part of the 42 curriculum by fmesci.*

# get_next_line

## Description

`get_next_line` is a C function that returns, on each call, the next line read
from a file descriptor (a file, `stdin`, a pipe...).

```c
char	*get_next_line(int fd);
```

- Returns the line that was read, **including the trailing `\n`** (except for
  the last line of a file that does not end with `\n`).
- Returns `NULL` when there is nothing left to read or if an error occurs.
- The returned line is allocated with `malloc`: **the caller must `free` it**.

The goal of the project is to learn how to use **static variables** in C and
to handle dynamic memory and `read()` correctly.

Only the allowed functions are used: `read`, `malloc` and `free`.

## Instructions

### Compilation

This is not a standalone program: it is compiled together with your own `main`.
The size of each read is set with `BUFFER_SIZE` (default: `42`):

```bash
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 \
	get_next_line.c get_next_line_utils.c main.c
```

Bonus (multiple file descriptors at once):

```bash
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 \
	get_next_line_bonus.c get_next_line_utils_bonus.c main.c
```

### Usage example

```c
#include <fcntl.h>
#include <stdio.h>
#include "get_next_line.h"

int	main(void)
{
	int		fd;
	char	*line;

	fd = open("file.txt", O_RDONLY);
	line = get_next_line(fd);
	while (line != NULL)
	{
		printf("%s", line);
		free(line);
		line = get_next_line(fd);
	}
	close(fd);
	return (0);
}
```

### Files

| File | Contents |
|---|---|
| `get_next_line.c` | Main logic and helper functions (`static`) |
| `get_next_line_utils.c` | `ft_strlen`, `ft_strchr`, `ft_strjoin` |
| `get_next_line.h` | Prototypes and default `BUFFER_SIZE` |
| `*_bonus.[ch]` | Bonus version supporting multiple `fd`s |

## Algorithm

### The problem

`read()` knows nothing about lines: it reads blocks of `BUFFER_SIZE` bytes. A
block may contain half a line, a whole line or several lines. Whatever is read
past a `\n` **cannot be read again** from the file (`lseek` is forbidden and
does not work on `stdin` or pipes), so it must be kept for the next call.

### The solution: a static stash

A `static char *reserva` variable lives inside `get_next_line`. Being `static`,
it keeps its value between calls (it lives in static memory, not on the stack),
but it is only visible inside the function. Global variables are forbidden by
the subject.

Each call goes through three steps, each in its own function:

| Function | What it does with the stash |
|---|---|
| `read_to_reserve` | **Fills** it: reads blocks and appends them until there is a `\n` or EOF |
| `extract_line` | **Reads** it: copies the first line (with its `\n`) into a new string |
| `clean_reserve` | **Trims** it: keeps only what comes after the `\n` |

Example with the file `"Hola\nmundo\nfin"` and `BUFFER_SIZE = 7`:

| Call | Stash after reading | Returned line | Stash after trimming |
|---|---|---|---|
| 1 | `"Hola\nmu"` | `"Hola\n"` | `"mu"` |
| 2 | `"mundo\nfin"` | `"mundo\n"` | `"fin"` |
| 3 | `"fin"` (EOF) | `"fin"` | `NULL` |
| 4 | `""` (EOF) | `NULL` | `NULL` |

### Rationale

- **Read as little as possible**: reading stops as soon as the stash contains
  a `\n`. If the stash already holds a full line, `read()` is not called at all.
  The whole file is never read at once.
- **Small, single-purpose functions**: they comply with the Norm (max. 25 lines,
  max. 5 functions per file) and are easy to reason about.
- **Helper functions take the stash as a parameter** instead of using the
  static variable directly. Thanks to this, the bonus only required changing
  *where* the stash is stored, not the logic.

### Memory and error handling

- `join_and_free` concatenates and frees the old stash so it is not lost.
- The read buffer is temporary: it is **always** freed before leaving
  `read_to_reserve`. The stash is only freed there on error.
- Every error (`malloc` or `read` failure) ends in a single cleanup point that
  frees the stash and returns `NULL`.
- After the static stash is freed it is set to `NULL`, so a later call does not
  use freed memory (*use-after-free* / *double free*).
- A negative `fd` or a `BUFFER_SIZE <= 0` returns `NULL` immediately.

## Bonus

- **Multiple file descriptors at once** with **a single static variable**: an
  array `static char *reserva[FD_MAX]`, where each `fd` uses its own slot
  `reserva[fd]`. This allows alternating between several files without mixing
  lines.
- `FD_MAX` defaults to `1024`, the usual per-process open file limit on Linux
  (`ulimit -n`). It can be changed with `-D FD_MAX=n`.
- If `fd >= FD_MAX`, the function returns `NULL` without accessing the array
  out of bounds.
- All slots start as `NULL` automatically, since the array is `static`.

## Resources

- `man 2 read`, `man 3 malloc`, `man 3 free`
- [Static variables in C — GeeksforGeeks](https://www.geeksforgeeks.org/static-variables-in-c/)
- [File descriptor — Wikipedia](https://en.wikipedia.org/wiki/File_descriptor)
- [Norminette — 42 School](https://github.com/42School/norminette)

### AI usage

AI was used for didactic purposes: to reinforce knowledge of the C language and
of the functions and concepts used in this project, and as support for writing
this documentation.
