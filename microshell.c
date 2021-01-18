
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int		ft_strlen(char *str)
{
	int i = 0;

	while (str[i])
		i++;
	return (i);
}

int		ft_error(char *err_message, char *path)
{
	write(2, err_message, ft_strlen(err_message));
	if (path)
	{
		write(2, path, ft_strlen(path));
		write(2, "\n", 1);
	}
	return (1);
}

void	ft_substr(char **str1, char **str2, int i, int j)
{
	int k = 0;

	while (i < j)
	{
		str1[k] = str2[i];
		k++;
		i++;
	}
	str1[k] = 0;
}

int		cd(char **argv, int len)
{
	if (len != 2)
		return (ft_error("error: cd: bad arguments\n", NULL));
	if (chdir(argv[1]))
		return (ft_error("error: cd: cannot change directory to ", argv[1]));
	return (0);
}

int		main(int argc, char **argv, char **env)
{
	int i, j, k, l;
	int p[2];
	pid_t pid;
	int fd;
	char **tmp;

	i = 1;
	while (i < argc)
	{
		j = i, k = i, l = i;
		while (j < argc && strcmp(argv[j], ";")) // loop untill next semicolon (j) is hit
			j++;
		fd = 0; // set fd
		while (k < j) // loop through all commands untill semicolon (j) is hit
		{
			l = k; // set k to the right index
			while (l < j && strcmp(argv[l], "|")) // loop untill next pipe (l) is hit
				l++;
			tmp = malloc(sizeof(char*) * (l - k + 1)); // malloc tmp sizeof args untill pipe (l)
			if (!tmp)
				return (0);
			ft_substr(tmp, argv, k, l); // tmp is 2d array of all args untill next pipe (l)
			if (!strcmp(tmp[0], "cd")) //  check for cd as first args
				cd(tmp, l - k);
			else
			{
				pipe(p); // calling pipe
				if ((pid = fork()) == -1) // create fork and protect
					return (ft_error("error: fatal\n", NULL));
				else if (pid == 0) // child
				{
					dup2(fd, 0); // get input from pipe (tmp fd)
					if (l < j)
						dup2(p[1], 1);  // make output go to pipe (tmp fd)
					close(p[0]); // close unused read-end of pipe
					if (execve(tmp[0], tmp, env))
						ft_error("error: cannot execute ", tmp[0]);
					exit(EXIT_FAILURE); // unsuccesfull termination
				}
				else
				{
					waitpid(pid, NULL, 0); // waits for child to be ended or stopped
					close(p[1]); // close unused write-end of pipe
					fd = p[0]; // input from pipe
				}
			}
			k = l + 1;
		}
		i = j + 1;
	}
}
