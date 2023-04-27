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
char *getFirstWord(char* input);
char **tokenizeEntry(char *input, char const *delim, ssize_t *length);

// GLOBALS
TetrisGameState* game;

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
	ssize_t pathname_length;
	size_t init_pathname_n;
	printf("Enter the path to the Tetris quicksave you wish to begin hacking: ");
	if ((pathname_length = getline(&pathname, &init_pathname_n, stdin)) == -1) {
		error(EXIT_FAILURE, errno, "getline failure"); 
	}

	// Someone could redirect a file into stdin that ends with EOF, so testing for
	// newline isn't entirely unnecessary
	if (pathname[pathname_length - 1] == '\n') {
		pathname[pathname_length - 1] = '\0';
	}
	
	if ((fd = open(pathname, O_RDWR)) == 0) {
		error(EXIT_FAILURE, errno, "open failure");
	}

	printf("You have entered %s as your file\n", pathname);

	if ((game = mmap(0, sizeof(TetrisGameState), PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED) {
		error(EXIT_FAILURE, errno, "mmap failure");
	}

	// Get info for improved prompt
	// man page for getpwuid says to set errno to 0 before the call to be able to check it after
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
		// Current_line will be null in the case of just an EOF character being put
		// into stdin, the first strcmp checks for just hitting enter into the shell
		// and the strspn call checks for any number of spaces before hitting enter
		// which would become null after strtok
		if (current_line != NULL && strcmp("\n", current_line) != 0 && strspn(current_line, delim) != (num_read - 1)) {
			int arg_count = 1;
			for (int i = 0; current_line[i] != '\0'; i++) {
				if (current_line[i] == ' ') {
					arg_count++;
				}
			}
			char *line_tokenized[arg_count];
			if (current_line[num_read - 1] == '\n') {
				current_line[num_read - 1] = '\0';
				num_read--;
			}
			
			line_tokenized[0] = strtok(current_line, delim);
			int i = 1;
			while ((line_tokenized[i] = strtok(NULL, delim)) != NULL) {
				i++;
			}

			// Else if ladder for commands
			if (strcmp("recover", line_tokenized[0]) == 0) {
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
				
			} else if (strcmp("check", line_tokenized[0]) == 0) {
				pid_t fork_id = fork();
				if (fork_id == -1) {
					error(EXIT_FAILURE, errno, "fork failure");
				} else if (fork_id == 0) {
					// In child process
					char *const new_args[3] = {line_tokenized[0], pathname, NULL};
					if (execv("check", new_args) == -1) {
						error(EXIT_FAILURE, errno, "execv failure");
					}
					exit(0);
				}
				// In parent process	
				if (wait(NULL) == -1) {
					error(EXIT_FAILURE, errno, "wait failure");
				} // Wondering if I should add a call to WIFEXITED here?
			} else if (strcmp("modify", line_tokenized[0]) == 0) {
				pid_t fork_id = fork();
				if (fork_id == -1) {
					error(EXIT_FAILURE, errno, "fork failure");
				} else if (fork_id == 0) {
					// In child process
					char *const new_args[3] = {line_tokenized[0], pathname,     NULL};
					exit(0);
				}
				// In parent process
				if (wait(NULL) == -1) {
					error(EXIT_FAILURE, errno, "wait failure");
				}
			}
		}
		// Need to abbreviate pathname, maybe use tokenizing? Some string 
		// manipulation method would also probably work
		printf("\033[38;2;123;175;212m%s\033[0m@\033[31mtetrashell\033[0m[%s][%d/%d]> ", login_info->pw_name, pathname, game->score, game->lines);
	} while ((num_read = getline(&current_line, &n, stdin)) != -1 
							&& strcmp("exit\n", current_line) != 0);

	free(current_line);
	free(pathname);

	if (num_read == -1) {
		error(EXIT_FAILURE, errno, "getline failure");
	} else {
		return EXIT_SUCCESS;
	}
}

// HELPER FUNCTIONS
char *getFirstWord(char *input) {
	char delim = ' ';
	// May want to add newline removal here (if it exists)
	return strsep(&input, &delim);
}

char **tokenizeEntry(char *input, const char *delim, ssize_t *length) {
// 	char **token_array = (char **) malloc((sizeof(char) * (size_t) *length) * 100000);
	static char *token_array[40];
// 	if (token_array == NULL) {
// 		error(EXIT_FAILURE, errno, "malloc failure");
// 	}
	if (input[(*length) - 1] == '\n') {
		input[(*length) - 1] = '\0';
		(*length)--;
	}
// 	char *buf = strtok(input, delim);
// 	int i = 0;
// 	while (buf != NULL) {
// 		token_array[i++] = buf;
// 		buf = NULL;
// 		buf = strtok(NULL, delim);
// 	}
	token_array[0] = strtok(input, delim);
	int i = 1;
	while ((token_array[i] = strtok(NULL, delim)) != NULL) {
		i++;
	}
	return token_array;
}


