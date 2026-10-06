/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmesci <fmesci@student.42barcelona.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/05 15:24:11 by fmesci            #+#    #+#             */
/*   Updated: 2026/10/06 14:45:33 by fmesci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static char	*join_and_free(char *reserva, char *buffer)
{
	char	*tmp;

	tmp = ft_strjoin(reserva, buffer);
	free(reserva);
	return (tmp);
}

static char	*extract_line(char *reserva)
{
	char	*line;
	int		i;
	int		j;

	if (reserva[0] == '\0')
		return (NULL);
	i = 0;
	while (reserva[i] != '\0' && reserva[i] != '\n')
		i++;
	if (reserva[i] == '\n')
		i++;
	line = (char *)malloc(sizeof(char) * (i + 1));
	if (line == NULL)
		return (NULL);
	j = 0;
	while (j < i)
	{
		line[j] = reserva[j];
		j++;
	}
	line[j] = '\0';
	return (line);
}

static char	*clean_reserve(char *reserva)
{
	char	*eol;
	char	*str;

	eol = ft_strchr(reserva, '\n');
	if (eol == NULL)
	{
		free(reserva);
		return (NULL);
	}
	str = ft_strjoin(eol + 1, NULL);
	free(reserva);
	return (str);
}

static char	*read_to_reserve(char *reserva, int fd)
{
	char	*buffer;
	int		bytes;

	bytes = 1;
	buffer = malloc(BUFFER_SIZE + 1);
	if (buffer == NULL)
		bytes = -1;
	while (ft_strchr(reserva, '\n') == NULL && bytes > 0)
	{
		bytes = read(fd, buffer, BUFFER_SIZE);
		if (bytes == -1)
			break ;
		buffer[bytes] = '\0';
		reserva = join_and_free(reserva, buffer);
		if (reserva == NULL)
			break ;
	}
	free(buffer);
	if (bytes == -1)
	{
		free(reserva);
		return (NULL);
	}
	return (reserva);
}

char	*get_next_line(int fd)
{
	static char	*reserva;
	char		*line;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	reserva = read_to_reserve(reserva, fd);
	if (reserva == NULL)
		return (NULL);
	line = extract_line(reserva);
	if (line == NULL)
	{
		free(reserva);
		reserva = NULL;
		return (NULL);
	}
	reserva = clean_reserve(reserva);
	return (line);
}
