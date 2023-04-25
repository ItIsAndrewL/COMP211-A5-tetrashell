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

#include "tetris.h"

// FORWARD DECLARATIONS
char* getFirstWord(char*);

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
      " ░   ░  ░  ░   ░  ░    ░  ░    ░  ░\n\n\033[0mThe ultimate destructive Tetris"
	  " utility tool of DOOM!!\n");
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

  // Basic Prompt & Exit
	char *current_line = NULL;
	size_t n = 0;
	ssize_t num_read;
	char *first_word;
	do {
		if (current_line != NULL) {
			// Else if ladder for commands
			first_word = getFirstWord(current_line);
		}
		printf("tetrashell> ");
	} while ((num_read = getline(&current_line, &n, stdin)) != -1 
							&& strcmp("exit\n", current_line) != 0);
}

// HELPER FUNCTIONS
char* getFirstWord(char* input) {
	char delim = ' ';
	// May want to add newline removal here (if it exists)
	return strsep(&input, &delim);
}


