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
void runRank(char**, char*, int);
void runInfo(char*, TetrisGameState*);
void runSwitch(char*, TetrisGameState**, int*, ssize_t*);
void runHelp(char*);
void runVisualize(TetrisGameState*);

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
				runRank(line_tokenized, pathname, i);
			} else if (strcmp("info", line_tokenized[0]) == 0) {
				runInfo(pathname, game);
			} else if (strcmp("switch", line_tokenized[0]) == 0) {
				runSwitch(pathname, &game, &fd, &pathname_length);
			} else if (strcmp("help", line_tokenized[0]) == 0) {
				runHelp(line_tokenized[1]);
			} else if (strcmp("visualize", line_tokenized[0]) == 0) {
				runVisualize(game);
			} else {
				printf("Command not recognized, try:\nrecover\ncheck\nrank\nmodify\ninfo\nswitch\nhelp\nvisualize\nexit\n");
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

void runRank(char **line_tokenized, char *pathname, int numArgs) {
	int pipe_in[2];
	if (pipe(pipe_in) == -1) {
		error(EXIT_FAILURE, errno, "pipe failure");
	}
	pid_t pid = fork();
	if (pid == -1) {
		error(EXIT_FAILURE, errno, "fork failure");
	} else if (pid) {
		// In parent
		int res;
		if (close(pipe_in[0]) == -1)
			error(EXIT_FAILURE, errno, "close failure");

		if (write(pipe_in[1], pathname, strlen(pathname)) == -1)
			error(EXIT_FAILURE, errno, "write failure");

		if (close(pipe_in[1]) == -1)
			error(EXIT_FAILURE, errno, "close failure");

		if (wait(&res) == -1)
			error(EXIT_FAILURE, errno, "wait failure");

	} else {
		// In child
		if (close(pipe_in[1]) == -1)
			error(EXIT_FAILURE, errno, "close failure");

		if (dup2(pipe_in[0], 0) == -1)
			error(EXIT_FAILURE, errno, "dup2 failure");

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
			updated_args[2] = "10"; // Defaults to 10 lines
			updated_args[3] = "uplink";
			updated_args[4] = NULL;
		} else {
			updated_args[0] = "rank";
			updated_args[1] = "score"; // Deafults to score
			updated_args[2] = "10"; // Defaults to 10 lines
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

void runHelp(char* command) {
	// Else if ladder for commands
	if (strcmp("recover", command) == 0) {
		printf("recover <path_to_disk.img file>\nScans the disk.img file for valid quicksaves, and outputs them in a new folder, recovered which is created in the current directory\n");
	} else if (strcmp("check", command) == 0) {
		printf("check\nRuns the current quicksave through the validation software to determine if the curernt quicksave is valid or not\n");
	} else if (strcmp("modify", command) == 0) {
		printf("modify <lines/score> <new_amount>\nModifies the current quicksave's lines or score to the new_amount\n");
	} else if (strcmp("rank", command) == 0) {
		printf("rank [lines/score] [amount_to_show]\nUploads your current quicksave to the server leaderboard and displays the specified amount_to_show top leaders based off the lines or score argument provided.\n");
	} else if (strcmp("info", command) == 0) {
		printf("info\nPrints current saves pathname, score, and lines\n");
	} else if (strcmp("switch", command) == 0) {
		printf("switch <path_to_new_Tetris_Quicksave.bin>\nSwitches the current quicksave to the new_Tetris_Quicksave.bin file\n");
	} else if (strcmp("help", command) == 0) {
		printf("help <command>\nPrints usage information for command\n");
	} else if (strcmp("visualize", command) == 0) {
		printf("visualize\nPrints out the current state of the game board as it would be displayed in game.\n");
	} else if (strcmp("exit", command) == 0) {
		printf("exit\nExits tetrashell\n");
	} else {
		printf("Command not recognized, try getting help with:\nrecover\ncheck\nrank\nmodify\ninfo\nswitch\nhelp\nvisualize\n");
	}
}

void runVisualize(TetrisGameState* game) {
	printf("+-----Game board-----+\n");
// 	int i = 0;
// 	while (i < BLOCKS_WIDE * BLOCKS_TALL) {
// 		printf("|%.*s|\n", BLOCKS_WIDE, (game->board)+i);
// 		i += BLOCKS_WIDE;
// 	}
	for (int i = 0; i < BLOCKS_WIDE * BLOCKS_TALL; i++) {
		if (i == 0) {
			printf("|");
		} else if (i % BLOCKS_WIDE == 0 && i != 0) {
			printf("|\n|");
		}
		printf("%c%c", *((game->board)+i), *((game->board)+i));
		if (i == (BLOCKS_WIDE * BLOCKS_TALL) - 1) {
			printf("|\n");
		}
	}
	printf("+--------------------+\n+--Next--+\n");
	int piece_array_width = 4;
	int piece_array_height = 4;
	for (int i = 0; i < piece_array_width * piece_array_height; i++) {
		if (i == 0) {
			printf("|");
		} else if (i % piece_array_width == 0 && i != 0) {
			printf("|\n|");
		}
		printf("%c%c", *((tetris_pieces[game->next_piece])+i), *((tetris_pieces[game->next_piece])+i));
		if (i == (piece_array_width * piece_array_height) - 1) {
			printf("|\n");
		}
	}
	printf("+--------+\n");
}
