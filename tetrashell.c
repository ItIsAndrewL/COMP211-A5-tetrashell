// Collaborators/teammates: Andrew Lockard, Benjamin Eldridge

// INCLUDES
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/wait.h>

#include "tetris.h"

// FORWARD DECLARATIONS
int countArgs(char*);
void runRecover(char**);
void runCheck(char**, char*);
void runModify(char**, char*);
void runRank(char**, char*, int, char*);
void runInfo(char*, TetrisGameState*);
void runSwitch(char*, TetrisGameState**, int*, ssize_t*);

// MAIN
int main(int argc, char **argv) {
  // Print intro message
  printf("Welcome to..\n\n"
      "\033[31m▄▄▄█████\033[38;5;160m▓▓\033[31m█████▄▄▄█████\033[38;5;160m▓ "
      "\033[31m██▀███   ▄▄▄        ██████  ██\033[38;5;160m░ \033[31m██ "
      "\033[38;5;160m▓\033[31m█████  ██\033[38;5;160m▓     "
      "\033[31m██\033[38;5;160m▓    \n▓  \033[31m██\033[38;5;160m▒ "
      "▓▒▓\033[31m█   ▀\033[38;5;160m▓  \033[31m██\033[38;5;160m▒ "
      "▓▒▓\033[31m██ \033[38;5;160m▒ \033[31m██\033[38;5;160m▒▒\033[31m████▄   "
      " \033[38;5;160m▒\033[31m██    \033[38;5;160m▒ "
      "▓\033[31m██\033[38;5;160m░ \033[31m██\033[38;5;160m▒▓\033[31m█   "
      "\033[31m▀ \033[38;5;160m▓\033[31m██\033[38;5;160m▒    "
      "▓\033[31m██\033[38;5;160m▒    \n▒ ▓\033[31m██\033[38;5;160m░ "
      "▒░▒\033[31m███  \033[38;5;160m▒ ▓\033[31m██\033[38;5;160m░ "
      "▒░▓\033[31m██ \033[38;5;160m░\033[31m▄█ \033[38;5;160m▒▒\033[31m██  ▀█▄ "
      " \033[38;5;160m░ ▓\033[31m██▄   "
      "\033[38;5;160m▒\033[31m██▀▀██\033[38;5;160m░▒\033[31m███   "
      "\033[38;5;160m▒\033[31m██\033[38;5;160m░    "
      "\033[38;5;160m▒\033[31m██\033[38;5;160m░    \n░ "
      "▓\033[31m██\033[38;5;160m▓ ░ ▒▓\033[31m█  ▄\033[38;5;160m░ "
      "▓\033[31m██\033[38;5;160m▓ ░ ▒\033[31m██▀▀█▄  "
      "\033[38;5;160m░\033[31m██▄▄▄▄██   \033[38;5;160m▒   "
      "\033[31m██\033[38;5;160m▒░▓\033[31m█ \033[38;5;160m░\033[31m██ "
      "\033[38;5;160m▒▓\033[31m█  ▄ \033[38;5;160m▒\033[31m██\033[38;5;160m░   "
      " ▒\033[31m██\033[38;5;160m░    \n  ▒\033[31m██\033[38;5;160m▒ ░ "
      "░▒\033[31m████\033[38;5;160m▒ ▒\033[31m██\033[38;5;160m▒ ░ "
      "░\033[31m██\033[38;5;160m▓ ▒\033[31m██\033[38;5;160m▒ ▓\033[31m█   "
      "\033[38;5;160m▓\033[31m██\033[38;5;160m▒▒\033[31m██████\033[38;5;"
      "160m▒▒░▓\033[31m█\033[38;5;160m▒░\033[31m██\033[38;5;160m▓░▒\033["
      "31m████\033[38;5;160m▒░\033[31m██████\033[38;5;160m▒░\033[31m██████\033["
      "38;5;160m▒\n  ▒ ░░   ░░ ▒░ ░ ▒ ░░   ░ ▒▓ ░▒▓░ ▒▒   "
      "▓▒\033[31m█\033[38;5;160m░▒ ▒▓▒ ▒ ░ ▒ ░░▒░▒░░ ▒░ ░░ ▒░▓  ░░ ▒░▓  ░\n    "
      "░     ░ ░  ░   ░      ░▒ ░ ▒░  ▒   ▒▒ ░░ ░▒  ░ ░ ▒ ░▒░ ░ ░ ░  ░░ ░ ▒  "
      "░░ ░ ▒  ░\n  ░         ░    ░        ░░   ░   ░   ▒   ░  ░  ░   ░  ░░ ░ "
      "  ░     ░ ░     ░ ░   \n            ░  ░           ░           ░  ░     "
      " ░   ░  ░  ░   ░  ░    ░  ░    ░  ░\n\n\033[0mThe ultimate destructive "
	  	"Tetris utility tool of DOOM!!\n");

	// Prompt for path to quicksave
	int fd;

	char *pathname = NULL;
	TetrisGameState* game;

	ssize_t pathname_length;
	size_t init_pathname_n = 0;
	printf("Enter the path to the Tetris quicksave you wish to begin hacking: ");
	if ((pathname_length = getline(&pathname, &init_pathname_n, stdin)) == -1) {
		error(EXIT_FAILURE, errno, "getline failure"); 
	}

	// Testing for newline at end
	if (pathname[pathname_length - 1] == '\n') {
		pathname[pathname_length - 1] = '\0';
	}
	
	if ((fd = open(pathname, O_RDWR)) == 0) {
		error(EXIT_FAILURE, errno, "open failure");
	}

	printf("You have entered %s as your file\n", pathname);

	if ((game = mmap(0, sizeof(TetrisGameState),
					PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED) {
		error(EXIT_FAILURE, errno, "mmap failure");
	}

	// Get info for improved prompt
	errno = 0;
	struct passwd *login_info = getpwuid(getuid());
	if (login_info == NULL) {
		error(EXIT_FAILURE, errno, "getpwuid failure");
	}

	// Basic Prompt & Exit
	char *current_line = NULL;
	char **line_tokenized;
	const char *delim = " ";
	size_t n = 0;
	ssize_t num_read;
	char *first_word;

	do {
		// Makes sure logic does not run on empty inputs (spaces, new lines, EOF)
		if (current_line != NULL && strcmp("\n", current_line) != 0
				&& strspn(current_line, delim) != (num_read - 1)) {

			char *line_tokenized[countArgs(current_line)];
			// Remove new line character
			if (current_line[num_read - 1] == '\n') {
				current_line[num_read - 1] = '\0';
			}

			line_tokenized[0] = strtok(current_line, delim);
			int i = 1;
			while ((line_tokenized[i] = strtok(NULL, delim)) != NULL) {
				i++;
			}

			// Else if ladder for commands
			if (strcmp("recover", line_tokenized[0]) == 0) {
				runRecover(line_tokenized);
			} else if (strcmp("check", line_tokenized[0]) == 0) {
				runCheck(line_tokenized, pathname);
			} else if (strcmp("modify", line_tokenized[0]) == 0) {
				runModify(line_tokenized, pathname);
			} else if (strcmp("rank", line_tokenized[0]) == 0) {
				runRank(line_tokenized, pathname, i, login_info->pw_name);
			} else if (strcmp("info", line_tokenized[0]) == 0) {
				runInfo(pathname, game);
			} else if (strcmp("switch", line_tokenized[0]) == 0) {
				runSwitch(pathname, &game, &fd, &pathname_length);
			}	else {
				printf("Command not recognized, try:\nrecover\ncheck\nrank\nmodify\ninfo\nswitch\n");
			}
		}
		// Need to abbreviate pathname, maybe use tokenizing? Some string 
		// manipulation method would also probably work
		printf("\033[38;2;123;175;212m%s\033[0m@\033[31mtetrashell\033[0m[%s][%d/%d]> ", login_info->pw_name, pathname, game->score, game->lines);
	} while ((num_read = getline(&current_line, &n, stdin)) != -1 
							&& strcmp("exit\n", current_line) != 0);
	
	close(fd);
	free(current_line);
	free(pathname);

	if (num_read == -1) {
		error(EXIT_FAILURE, errno, "getline failure");
	} else {
		return EXIT_SUCCESS;
	}
}

// HELPER FUNCTIONS
int countArgs(char *input) {
	int arg_count = 1;
	for (int i = 0; input[i] != '\0'; i++) {
		if (input[i] == ' ') {
			arg_count++;
		}
	}
	return arg_count;
}

void runRecover(char **line_tokenized) {
	pid_t fork_id = fork();
	if (fork_id == -1) {
		error(EXIT_FAILURE, errno, "fork failure");
	} else if (fork_id == 0) {
		// In child process
		if (execv("recover", line_tokenized) == -1) {
			error(EXIT_FAILURE, errno, "execv failure");
		}
		exit(0);
	}
	// In parent process
	if (wait(NULL) == -1) {
		error(EXIT_FAILURE, errno, "wait failure");
	} // Wondering if I should add a call to WIFEXITED here?
}

void runCheck(char **line_tokenized, char* pathname) {
	pid_t fork_id = fork();
	if (fork_id == -1) {
		error(EXIT_FAILURE, errno, "fork failure");
	} else if (fork_id == 0) {
		// In child process
		char* const new_args[3] = {line_tokenized[0], pathname, NULL};
		if (execv("check", new_args) == -1) {
			error(EXIT_FAILURE, errno, "execv failure");
		}
		exit(0);
	}
	// In parent process	
	if (wait(NULL) == -1) {
		error(EXIT_FAILURE, errno, "wait failure");
	} // Wondering if I should add a call to WIFEXITED here?
}

void runModify(char **line_tokenized, char *pathname) {
	pid_t fork_id = fork();
	if (fork_id == -1) {
		error(EXIT_FAILURE, errno, "fork failure");
	} else if (fork_id == 0) {
		// In child process
		char* const new_args[5] = {line_tokenized[0], line_tokenized[1],
				line_tokenized[2], pathname, NULL};
		if (execv("modify", new_args) == -1) {
			error(EXIT_FAILURE, errno, "execv failure");
		}
		exit(0);
	}
	// In parent process
	if (wait(NULL) == -1) {
		error(EXIT_FAILURE, errno, "wait failure");
	}
}

void runRank(char **line_tokenized, char *pathname, int numArgs, char *uName) {
	int pipe_in[2];
	int pipe_out[2];
	if (pipe(pipe_in) == -1)
		error(EXIT_FAILURE, errno, "pipe in failure");

	// Only setting up pipe out if the number of args is less than 3
	if (numArgs < 3 && pipe(pipe_out) == -1)
		error(EXIT_FAILURE, errno, "pipe out failure");

	pid_t pid = fork();
	if (pid == -1) {
		error(EXIT_FAILURE, errno, "fork failure");
	} else if (pid) {
		// In parent
		int res;
		// Close pipe in/out unused side
		if (close(pipe_in[0]) == -1)
			error(EXIT_FAILURE, errno, "close failure in pipe in");

		if (numArgs < 3 && close(pipe_out[1]) == -1)
			error(EXIT_FAILURE, errno, "close failure in pipe out");

		// Write to stdin & send EOF
		if (write(pipe_in[1], pathname, strlen(pathname)) == -1)
			error(EXIT_FAILURE, errno, "write failure");

		if (close(pipe_in[1]) == -1)
			error(EXIT_FAILURE, errno, "close failure for pipe in");

		if (wait(&res) == -1)
			error(EXIT_FAILURE, errno, "wait failure");

		// Read output from child function
		if (numArgs < 3) {
			const int READ_SIZE = 4096;
			size_t num_read = 0, total_read = 0, alloc_size = 0;
			char *in;
			do {
				total_read += num_read;
				if (total_read + READ_SIZE >= alloc_size) {
						alloc_size = alloc_size == 0 ? READ_SIZE : alloc_size * 2;
						in = realloc(in, alloc_size + 1);
						if (in == NULL)
							error(EXIT_FAILURE, errno, "realloc failure");
				}
			} while((num_read = read(pipe_out[0], in + total_read, READ_SIZE)) > 0);

			// Error check fread
			if (num_read == -1)
				free(in);
				error(EXIT_FAILURE, errno, "error in reading rank output");

			// Null terminate the input
			in[total_read] = '\0';

			// Count number of output (since there may not be 1000 saves)
			unsigned int num_lines = 0;
			int i;
			for (i = 0; i < total_read; i++) {
				if (in[i] == '\n')
					num_lines++;
			}
			// Don't miss last one
			if (in[i-1] != '\n')
				num_lines++;

			char **leaderboard = malloc(sizeof(char*) * num_lines);
			if (leaderboard == NULL) {
				free(in);
				error(EXIT_FAILURE, errno, "error in creating leaderboard malloc");
			}

			// Create our inputted str we will look for
			size_t len = strlen(pathname) + strlen(uName) + 2;// +2 for \n & "/"
			char *my_file = malloc(sizeof(char) * len);
			if (snprintf(my_file, len, "%s/%s", uName, pathname) < 0) {
				free(in);
				free(my_file);
				free(leaderboard);
				error(EXIT_FAILURE, errno, "error in snprintf");
			}

			// Break input up & store pointers in leaderboard
			i = 0;
			int my_save_i = 0;
			leaderboard[i] = in;
			for (int c = 0; c < total_read; c++) {
				if (in[c] == '\n') {
					in[c] = '\0';
					if (++i < num_lines) {
						leaderboard[i] = in + c + 1;
					}
					// Save the index of my save
					if (strcmp(leaderboard[i-1], my_file) == 0) {
						my_save_i = i-1;
					}
				}
			}

			// Check if file was not found on the server
			if (my_save_i == 0) {
				free(in);
				free(leaderboard);
				free(my_file);
				printf("Your file was not found on the server, make sure it passes "
					 "check and try again\n");
				return;
			}

			// Print out
			int end = my_save_i + 5 <= num_lines ? my_save_i + 5 : num_lines;
			for (int d = my_save_i - 5 < 0 ? 0 : my_save_i; d <= end; d++) {
				if (d == my_save_i) {
					printf("\x1b[1m>>> %i %s <<<\x1b[22m\n", d + 1, leaderboard[d]);
				} else {
					printf("%i %s\n", d + 1, leaderboard[d]);
				}
			}

			// Free prior to exit
			free(in);
			free(leaderboard);
			free(my_file);

			if (close(pipe_out[0]) == -1)
				error(EXIT_FAILURE, errno, "error closing pipe out");
		}
	} else {
		// In child
		if (close(pipe_in[1]) == -1)
			error(EXIT_FAILURE, errno, "close failure for pipe in");

		if (dup2(pipe_in[0], 0) == -1)
			error(EXIT_FAILURE, errno, "dup2 failure for pipe in");

		if (numArgs < 3 && close(pipe_out[0]) == -1)
			error(EXIT_FAILURE, errno, "close pipe out failure");

		if (numArgs < 3 && dup2(pipe_out[1], 1) == -1)
			error(EXIT_FAILURE, errno, "dup2 failure for pipe out");

		// Update Args & Quickrank
		char* updated_args[5];
		if (numArgs >= 3) {
			updated_args[0] = "rank";
			updated_args[1] = line_tokenized[1];
			updated_args[2] = line_tokenized[2];
			updated_args[3] = "uplink";
			updated_args[4] = NULL;
		} else if (numArgs == 2) {
			updated_args[0] = "rank";
			updated_args[1] = line_tokenized[1];
			updated_args[2] = "1000"; // Defaults to 1000 lines for PrettyRank
			updated_args[3] = "uplink";
			updated_args[4] = NULL;
		} else {
			updated_args[0] = "rank";
			updated_args[1] = "score"; // Deafults to score
			updated_args[2] = "1000"; // Defaults to 1000 lines for PrettyRank
			updated_args[3] = "uplink";
			updated_args[4] = NULL;
		}

		if (execv("rank", updated_args) == -1)
			error(EXIT_FAILURE, errno, "execv failure");
	}
}

void runInfo(char *pathname, TetrisGameState *game) {
	printf("Current savefile: %s\nScore: %d\nLines: %d\n", pathname, game->score, game->lines);
}

void runSwitch(char *pathname, TetrisGameState **game, int *fd, ssize_t *pathname_length) {
	close(*fd);
		
	printf("Enter the new path to the Tetris quicksave: ");
	if ((*pathname_length = getline(&pathname, pathname_length, stdin)) == -1) {
		error(EXIT_FAILURE, errno, "getline failure"); 
	}

	// Testing for newline at end
	if (pathname[*pathname_length - 1] == '\n') {
		pathname[*pathname_length - 1] = '\0';
	}
	
	if ((*fd = open(pathname, O_RDWR)) == 0) {
		error(EXIT_FAILURE, errno, "open failure");
	}

	printf("You have entered %s as your file\n", pathname);
	
	if ((*game = mmap(0, sizeof(TetrisGameState),
					PROT_READ, MAP_PRIVATE, *fd, 0)) == MAP_FAILED) {
		error(EXIT_FAILURE, errno, "mmap failure");
	}
}
