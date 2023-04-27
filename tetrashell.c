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
void openRecover(char**);
char **tokenizeEntry(char *input, char const *delim, ssize_t *length);

// GLOABLS
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
		pathname_length--;
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
		if (current_line != NULL) {
			// Else if ladder for commands
			line_tokenized = tokenizeEntry(current_line, delim, &num_read);

			if (strcmp("recover", line_tokenized[0]) == 0) {
				openRecover(line_tokenized);
			}
// 			free(line_tokenized);
		}
		// Need to abbreviate pathname, maybe use tokenizing?
		printf("\033[38;2;123;175;212m%s\033[0m@\033[31mtetrashell"
						"\033[0m[%s][%d/%d]> ", login_info->pw_name,
						pathname, game->score, game->lines);
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

void openRecover(char **line_tokenized) {
	pid_t fork_id = fork();
	if (fork_id == -1) {
		error(EXIT_FAILURE, errno, "fork failure");
	} else if (fork_id == 0) {
		// In child process
		if (execv("recover", line_tokenized) == -1) {
			error(EXIT_FAILURE, errno, "execv failure");
		}
		exit(0);
	} else {
		// In parent process
		if (wait(NULL) == -1) {
			error(EXIT_FAILURE, errno, "wait failure");
		} // Wondering if I should add a call to WIFEXITED here?
	}
}

char **tokenizeEntry(char *input, const char *delim, ssize_t *length) {
// 	char **token_array = (char **) malloc((sizeof(char) * (size_t) *length) * 100000);
	static char *token_array[40]; // TODO: Remove static variable, can cause threading problems
	if (token_array == NULL) {
		error(EXIT_FAILURE, errno, "malloc failure");
	}
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


