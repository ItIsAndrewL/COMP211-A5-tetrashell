# Tetrashell

Tetrashell is a COMP211 project designed to give the user the power to
destroy others in the mighty game of tetris.

## Installation

1. Have `make` installed
2. Download/clone the repo to the course server (with the binaries)
3. `cd` to repo location and run `make`
4. Execute with `./tetrashell`

## Usage

On bootup you will be prompted to enter a path to a valid Tetris Quicksave file, this will be the file that will be editied in tetrashell.

Shell prompt:
`username@tetrashell[save_path][save_score/save_lines]>` (7pts)

The following commands are supported in the tetrashell:

- `modify <lines/score> <new_amount>` (15pts)
  - Modifies the current quicksave's `lines` or `score` to the `new_amount`
- `rank [lines/score] [amount_to_show]` (25pts + QR 5pts + PR 15pts)
  - Uploads your current quicksave to the server leaderboard and
  displays the specified `amount_to_show` top leaders based off the
  `lines` or `score` argument provided.
  - With no `amount_to_show` provided, the 5 game saves above and below yours will be printed
  - Defaults to `score` ranking metric without the first argument
- `check` (15pts)
  - Runs the current quicksave through the validation software to
  determine if the curernt quicksave is valid or not
- `recover <path_to_disk.img file>` (10pts)
  - Scans the `disk.img` file for valid quicksaves, and outputs them
  in a new folder, `recovered` which is created in the current
  directory
- `switch` (5pts)
  - Prompts for a new file, then switches the current quicksave to that new file
- `visualize` (15pts)
  - Prints out the current state of the game board as it would be
  displayed in game.
- `info` (5pts)
  - Prints current saves pathname, score, and lines
- `help <command>` (7pts)
  - Prints usage information for `command`
- `exit` (5pts)
  - Exits tetrashell

## Authors

Benjamin Eldridge, Andrew Lockard
