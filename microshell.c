#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>

int	ft_how_much_token(char **strs)
{
	int i = 0;

	while (strs[i])
		i++;
	return (i);
}

void	ft_putchar_err(char c)
{
	write(2, &c, 1);
}

void	ft_putstr_err(char *str)
{
	int i = 0;

	while (str && str[i])
		ft_putchar_err(str[i++]);
}

void	ft_chdir(char *path)
{
	if (chdir(path) == - 1)
	{
		ft_putstr_err("error: cd: cannot change directory to ");
		ft_putstr_err(path);
		ft_putstr_err("\n\0");
	}
}

bool	setup_next_argv(char **argv)
{
	int		i = 0;
	bool	retour = false;

	while (argv[i])
	{
		if (argv[i] && (argv[i][0] == '|' || argv[i][0] == ';') && !argv[i][1])
		{
			if (argv[i][0] == '|')
				retour = true;
			argv[i] = NULL;
			return (retour);
		}
		i++;
	}
	return (false);
}

int	main(int argc, char **argv, char **envp)
{
	int		index = 1;
	int		id = 0;
	bool	ppipe = false;
	int		fd[2];

	while (index < argc && argv[index])
	{
		ppipe = setup_next_argv(&argv[index]);
		if (ppipe)
		{
			if (pipe(fd) == -1)
			{
				ft_putstr_err("error: fatal\n\0");
				exit(1);
			}
		}
		if (strncmp(argv[index], "cd", 2) == 0)
		{
			if (argv[index + 1] && !argv[index + 2])
				ft_chdir(argv[++index]);
			else
				ft_putstr_err("error: cd: bad arguments\n\0");
		}
		else
		{
			id = fork();
			if (id == 0)
			{
				if (ppipe)
				{
					dup2(fd[1], 1);
					close(fd[1]);
					close(fd[0]);
				}
				if (execve(argv[index], &argv[index], envp) == -1)
				{
					ft_putstr_err("error: cannot execute ");
					ft_putstr_err(argv[index]);
					ft_putstr_err("\n\0");
					exit(1);
				}
			}
			else
			{
				if (ppipe)
				{
					dup2(fd[0], 0);
					close(fd[0]);
					close(fd[1]);
				}
				waitpid(id, NULL, 0);
			}
		}
		index = index + ft_how_much_token(&argv[index]) + 1;
	}
}
