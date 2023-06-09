// Collaborators/teammates: Andrew Lockard, Benjamin Eldridge

// INCLUDES
#include <errno.h>
#include <error.h>
#include <fcntl.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "tetris.h"

// FORWARD DECLARATIONS
int countArgs(char *);
void runRecover(char **);
void runCheck(char **, char *);
void runModify(char **, char *);
void runRank(char **, char *, int, char *);
void runInfo(char *, TetrisGameState *);
void runSwitch(char **, TetrisGameState **, int *, ssize_t *);
void runHelp(char *);
void runVisualize(TetrisGameState *);

// MAIN
int main(int argc, char **argv) {
  // Print intro message
  printf(
      "Welcome to..\n\n"
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
  TetrisGameState *game;

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

  if ((game = mmap(0, sizeof(TetrisGameState), PROT_READ, MAP_PRIVATE, fd,
                   0)) == MAP_FAILED) {
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
    if (current_line != NULL && strcmp("\n", current_line) != 0 &&
        strspn(current_line, delim) != (num_read - 1)) {
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
        runSwitch(&pathname, &game, &fd, &pathname_length);
      } else if (strcmp("help", line_tokenized[0]) == 0) {
        runHelp(line_tokenized[1]);
      } else if (strcmp("visualize", line_tokenized[0]) == 0) {
        runVisualize(game);
      } else {
        printf(
            "Command not recognized, supported commands inlcude:"
            "\nrecover\ncheck\nrank\nmodify\ninfo\nswitch\nhelp\nvisualize\n"
            "exit\nUse help <command> for more info\n");
      }
    }
    // Need to abbreviate pathname, maybe use tokenizing? Some string
    // manipulation method would also probably work
    printf(
        "\033[38;2;123;175;212m%s\033[0m\033[31m@tetrashell\033[0m[%.4s...][%d/"
        "%d]> ",
        login_info->pw_name, pathname, game->score, game->lines);
  } while ((num_read = getline(&current_line, &n, stdin)) != -1 &&
           strcmp("exit\n", current_line) != 0);

  close(fd);
  free(current_line);
  free(pathname);
  if (munmap(game, sizeof(TetrisGameState)) == -1)
    error(EXIT_FAILURE, errno, "Error in munmap");

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
  }  // Wondering if I should add a call to WIFEXITED here?
}

void runCheck(char **line_tokenized, char *pathname) {
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
  }  // Wondering if I should add a call to WIFEXITED here?
}

void runModify(char **line_tokenized, char *pathname) {
  pid_t fork_id = fork();
  if (fork_id == -1) {
    error(EXIT_FAILURE, errno, "fork failure");
  } else if (fork_id == 0) {
    // In child process
    char *const new_args[5] = {line_tokenized[0], line_tokenized[1],
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
  if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1)
    error(EXIT_FAILURE, errno, "pipe failure");

  pid_t pid = fork();
  if (pid == -1) {
    error(EXIT_FAILURE, errno, "fork failure");
  } else if (pid) {
    // In parent
    int res;
    // Close pipe in/out unused side
    if (close(pipe_in[0]) == -1 || close(pipe_out[1]) == -1)
      error(EXIT_FAILURE, errno, "close failure in pipe in");

    // Write to stdin & send EOF
    if (write(pipe_in[1], pathname, strlen(pathname)) == -1)
      error(EXIT_FAILURE, errno, "write failure");

    if (close(pipe_in[1]) == -1)
      error(EXIT_FAILURE, errno, "close failure for pipe in");

    if (wait(&res) == -1) error(EXIT_FAILURE, errno, "wait failure");

    // Read output from child function
    const int READ_SIZE = 4096;
    size_t num_read = 0, total_read = 0, alloc_size = 0;
    char *in;
    do {
      total_read += num_read;
      if (total_read + READ_SIZE >= alloc_size) {
        alloc_size = alloc_size == 0 ? READ_SIZE : alloc_size * 2;
        in = realloc(in, alloc_size + 1);
        if (in == NULL) error(EXIT_FAILURE, errno, "realloc failure");
      }
    } while ((num_read = read(pipe_out[0], in + total_read, READ_SIZE)) > 0);
    // Error check read
    if (num_read == -1) {
      free(in);
      error(EXIT_FAILURE, errno, "error in reading rank output");
    }
    // Null terminate the input
    in[total_read] = '\0';

    // Count number of output (since there may not be 1000 saves)
    unsigned int num_lines = 0;
    int i;
    for (i = 0; i < total_read; i++) {
      if (in[i] == '\n') num_lines++;
    }
    // Don't miss last one
    if (in[i - 1] != '\n') num_lines++;

    char **leaderboard = malloc(sizeof(char *) * num_lines);
    if (leaderboard == NULL) {
      free(in);
      error(EXIT_FAILURE, errno, "error in creating leaderboard malloc");
    }

    // Create our inputted str we will look for
    size_t len = strlen(pathname) + strlen(uName) + 2;  // +2 for \n & "/"
    char *my_file = malloc(sizeof(char) * len);
    if (snprintf(my_file, len, "%s/%s", uName, pathname) < 0) {
      free(in);
      free(my_file);
      free(leaderboard);
      error(EXIT_FAILURE, errno, "error in snprintf");
    }

    // Break input up & store pointers in leaderboard
    i = 0;
    int my_save_i = -1;
    leaderboard[i] = in;
    for (int c = 0; c < total_read; c++) {
      if (in[c] == '\n') {
        in[c] = '\0';
        if (++i < num_lines) {
          leaderboard[i] = in + c + 1;
        }
        // Save the index of my save
        if (strcmp(leaderboard[i - 1], my_file) == 0) {
          my_save_i = i - 1;
        }
      }
    }

    // Check if file was not found on the server
    if (numArgs < 3 && my_save_i == -1) {
      free(in);
      free(leaderboard);
      free(my_file);
      printf(
          "Your file was not found on the server, make sure it passes "
          "check and try again\n");
      return;
    }

    // If number was specified, get it or the min num of lines
    if (numArgs >= 3) {
      int arg = atoi(line_tokenized[2]);
      if (arg == 0) {
        fprintf(stderr, "Error in converting arg 2 into int");
        free(in);
        free(leaderboard);
        free(my_file);
        return;
      }
      int amt_to_print = arg < num_lines ? arg : num_lines;
      for (int d = 0; d < amt_to_print; d++) {
        if (d == my_save_i) {
          printf("\033[3m>>> %i %s <<<\033[m\n", d + 1, leaderboard[d]);
        } else {
          printf("%i %s\n", d + 1, leaderboard[d]);
        }
      }
    } else {
      // Print out around our quicksave
      int end = my_save_i + 5 <= num_lines ? my_save_i + 5 : num_lines;
      for (int d = my_save_i - 5 < 0 ? 0 : my_save_i - 5; d <= end; d++) {
        if (d == my_save_i) {
          printf("\033[3m>>> %i %s <<<\033[m\n", d + 1, leaderboard[d]);
        } else {
          printf("%i %s\n", d + 1, leaderboard[d]);
        }
      }
    }

    // Free prior to exit
    free(in);
    free(leaderboard);
    free(my_file);

    if (close(pipe_out[0]) == -1)
      error(EXIT_FAILURE, errno, "error closing pipe out");
  } else {
    // In child
    if (close(pipe_in[1]) == -1 || close(pipe_out[0]) == -1)
      error(EXIT_FAILURE, errno, "close failure for pipe");

    if (dup2(pipe_in[0], 0) == -1 || dup2(pipe_out[1], 1) == -1)
      error(EXIT_FAILURE, errno, "dup2 failure for pipe in");

    // Update Args & Quickrank
    char *updated_args[5];
    if (numArgs >= 3) {
      updated_args[0] = "rank";
      updated_args[1] = line_tokenized[1];
      updated_args[2] = line_tokenized[2];
      updated_args[3] = "uplink";
      updated_args[4] = NULL;
    } else if (numArgs == 2) {
      updated_args[0] = "rank";
      updated_args[1] = line_tokenized[1];
      updated_args[2] = "1000";  // Defaults to 1000 lines for PrettyRank
      updated_args[3] = "uplink";
      updated_args[4] = NULL;
    } else {
      updated_args[0] = "rank";
      updated_args[1] = "score";  // Deafults to score
      updated_args[2] = "1000";   // Defaults to 1000 lines for PrettyRank
      updated_args[3] = "uplink";
      updated_args[4] = NULL;
    }

    if (execv("rank", updated_args) == -1)
      error(EXIT_FAILURE, errno, "execv failure");
  }
}

void runInfo(char *pathname, TetrisGameState *game) {
  printf("Current savefile: %s\nScore: %d\nLines: %d\n", pathname, game->score,
         game->lines);
}

void runSwitch(char **pathname, TetrisGameState **game, int *fd,
               ssize_t *pathname_length) {
  close(*fd);
  free(*pathname);
  *pathname = NULL;
  *pathname_length = 0;

  printf("Enter the new path to the Tetris quicksave: ");
  if ((*pathname_length = getline(pathname, pathname_length, stdin)) == -1) {
    error(EXIT_FAILURE, errno, "getline failure");
  }

  // Testing for newline at end
  if ((*pathname)[*pathname_length - 1] == '\n') {
    (*pathname)[*pathname_length - 1] = '\0';
  }

  if ((*fd = open(*pathname, O_RDWR)) == 0) {
    error(EXIT_FAILURE, errno, "open failure");
  }

  printf("You have entered %s as your file\n", *pathname);

  if ((*game = mmap(0, sizeof(TetrisGameState), PROT_READ, MAP_PRIVATE, *fd,
                    0)) == MAP_FAILED) {
    error(EXIT_FAILURE, errno, "mmap failure");
  }
}

void runHelp(char *command) {
  // Else if ladder for commands
  if (command == NULL) {
    fprintf(stderr,
            "Please provide a command that you would like help with, like "
            "this:\nhelp <command>\n");
  } else if (strcmp("recover", command) == 0) {
    printf(
        "recover <path_to_disk.img file>\nScans the disk.img file for valid "
        "quicksaves, and outputs them in a new folder, recovered which is "
        "created in the current directory\n");
  } else if (strcmp("check", command) == 0) {
    printf(
        "check\nRuns the current quicksave through the validation software to "
        "determine if the current quicksave is valid or not\n");
  } else if (strcmp("modify", command) == 0) {
    printf(
        "modify <lines/score> <new_amount>\nModifies the current quicksave's "
        "lines or score to the new_amount\n");
  } else if (strcmp("rank", command) == 0) {
    printf(
        "rank [lines/score] [amount_to_show]\nUploads your current quicksave "
        "to the server leaderboard and displays the specified amount_to_show "
        "top leaders based off the lines or score argument provided.\n");
  } else if (strcmp("info", command) == 0) {
    printf("info\nPrints current saves pathname, score, and lines\n");
  } else if (strcmp("switch", command) == 0) {
    printf(
        "switch <path_to_new_Tetris_Quicksave.bin>\nSwitches the current "
        "quicksave to the new_Tetris_Quicksave.bin file\n");
  } else if (strcmp("help", command) == 0) {
    printf("help <command>\nPrints usage information for command\n");
  } else if (strcmp("visualize", command) == 0) {
    printf(
        "visualize\nPrints out the current state of the game board as it would "
        "be displayed in game.\n");
  } else if (strcmp("exit", command) == 0) {
    printf("exit\nExits tetrashell\n");
  } else {
    printf(
        "Command not recognized, try getting help "
        "with:\nrecover\ncheck\nrank\nmodify\ninfo\nswitch\nhelp\nvisualize\n");
  }
}

void runVisualize(TetrisGameState *game) {
  printf("+-----Game board-----+\n");
  for (int i = 0; i < BLOCKS_WIDE * BLOCKS_TALL; i++) {
    if (i == 0) {
      printf("|");
    } else if (i % BLOCKS_WIDE == 0 && i != 0) {
      printf("|\n|");
    }
    printf("%c%c", *((game->board) + i), *((game->board) + i));
    if (i == (BLOCKS_WIDE * BLOCKS_TALL) - 1) {
      printf("|\n");
    }
  }
  int piece_array_width = 4;
  int piece_array_height = 4;

  printf("+--------------------+\n\n+Current:+\n");

  for (int i = 0; i < piece_array_width * piece_array_height; i++) {
    if (i == 0) {
      printf("|");
    } else if (i % piece_array_width == 0 && i != 0) {
      printf("|\n|");
    }
    printf("%c%c", *((tetris_pieces[game->current_piece]) + i),
           *((tetris_pieces[game->current_piece]) + i));
    if (i == (piece_array_width * piece_array_height) - 1) {
      printf("|\n");
    }
  }
  printf("+--------+\n\n");

  printf("+--Next--+\n");
  for (int i = 0; i < piece_array_width * piece_array_height; i++) {
    if (i == 0) {
      printf("|");
    } else if (i % piece_array_width == 0 && i != 0) {
      printf("|\n|");
    }
    printf("%c%c", *((tetris_pieces[game->next_piece]) + i),
           *((tetris_pieces[game->next_piece]) + i));
    if (i == (piece_array_width * piece_array_height) - 1) {
      printf("|\n");
    }
  }
  printf("+--------+\n");
}
