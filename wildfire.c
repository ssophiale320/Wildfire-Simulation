
/// Simulates a wildfire spread through a grid of trees. Each cell in the grid can be empty,
/// contain a tree, be burning, or burned out. The simulation includes features:
///     - Initial burning trees based on user-defined proportions.
///     - Tree density that affects how many trees are in the grid.
///     - A chance for trees to catch fire from neighboring burning trees.
///
/// The simulation runs in Overlay Display Mode, where the grid is printd in the terminal
/// and updated in real-time until all fires are extinguished or the user interrupts the
/// program.
///
/// Run the program with options to customize the simulation:
///     ./wildfire [-bN] [-cN] [-dN] [-nN] [-pN] [sN] [-H]
///
/// @file   wildfire.c
/// @author Sophia Le (sel5881@rit.edu)
///

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#define EMPTY 0
#define TREE 1
#define BURNING 2
#define BURNED 3
#define BURN_STAGES 3

#define BURNING_PERCENT 10
#define CATCH_FIRE_PERCENT 30
#define DENSITY 50
#define NEIGHBOR_EFFECT 25
#define PRINT_CYCLES -1
#define GRID_SIZE 10
#define PRINT_MODE 0
#define OVERLAY_MODE 1

static int size = GRID_SIZE;
static int density = DENSITY;
static int burning_percent = BURNING_PERCENT;
static int catch_fire_percent = CATCH_FIRE_PERCENT;
static int neighbor_effect = NEIGHBOR_EFFECT;
static int max_cycles = PRINT_CYCLES;
static int current_cycle = 0;
static int mode = OVERLAY_MODE;
static int print_mode = 0;

// BONUS
static int lightning_active = 0;
static double lightning_chance = 0.01;

void print_usage();
void init_grid(int grid[size][size]);
void print(int grid[size][size]);
void update(int grid[size][size], int next[size][size], int burn_counter[size][size], int *changes);
int count_burning_neighbors(int grid[size][size], int row, int col);
int count_tree_neighbors(int grid[size][size], int row, int col);
void run_sim();

/**
 * Processes command-line arguments and sets the simulation parameters.
 *
 * @param argc: The argument count
 * @param argv: The argument values
 * @return: 0 upon success or a non-zero value upon failure
 */
int main(int argc, char * argv[]) {
        for(int i = 1; i < argc; i++) {
                if(argv[i][0] == '-') {
                        switch(argv[i][1]) {
                                case 'H':
                                        print_usage();
                                        return EXIT_SUCCESS;
                                case 'b':
                                        burning_percent = atoi(&argv[i][2]);
                                        if(burning_percent < 1 || burning_percent > 100) {
                                                fprintf(stderr, "(-bN) proportion already burning must be an integer in [1...100].\n");
                                                return EXIT_FAILURE;
                                        } break;
                                case 'c':
                                        catch_fire_percent = atoi(&argv[i][2]);
                                        if(catch_fire_percent < 1 || catch_fire_percent > 100) {
                                                fprintf(stderr, "(-cN) probability a tree will catch fire must be an integer in [1...100].\n");
                                                return EXIT_FAILURE;
                                        } break;
                                case 'd':
                                        density = atoi(&argv[i][2]);
                                        if(density < 1 || density > 100) {
                                                fprintf(stderr, "(-dN) density of trees in the grid must be an integer in [1...100].\n");
                                                return EXIT_FAILURE;
                                        } break;
                                case 'n':
                                        neighbor_effect = atoi(&argv[i][2]);
                                        if(neighbor_effect < 0 || neighbor_effect > 100) {
                                                fprintf(stderr, "(-nN) %%neighbors influence catching fire must be an integer in [0...100].\n");
                                                return EXIT_FAILURE;
                                        } break;
                                case 'p':
                                        print_mode = 1;
                                        mode = PRINT_MODE;
                                        max_cycles = atoi(&argv[i][2]);
                                        if(max_cycles < 0 || max_cycles > 10000) {
                                                fprintf(stderr, "(-pN) number of states to print must be an integer in [0...100000].\n");
                                                return EXIT_FAILURE;
                                        } break;
                                case 's':
                                        size = atoi(&argv[i][2]);
                                        if(size < 5 || size > 40) {
                                                fprintf(stderr, "(-sN) simulation grid size must be an integer in [5...40].\n");
                                                return EXIT_FAILURE;
                                        } break;
                                case 'L':
                                        lightning_active = -1;
                                        lightning_chance = atof(&argv[i][2]);
                                        if(lightning_chance < 0 || lightning_chance > 1) {
                                                fprintf(stderr, "(-Ln) lightning probability must be a floating-point number in [0.0...1.0].\n");
                                                return EXIT_FAILURE;
                                        } break;
                                default:
                                        fprintf(stderr, "Unknown option: %s\n", argv[i]);
                                        print_usage();
                                        return EXIT_FAILURE;
                        }
                }
        }
        run_sim();
        return EXIT_SUCCESS;
}

/**
 * Prints the usage information for the simulation.
 */
void print_usage() {
        fprintf(stderr, "usage: wildfire [options]\n");
        fprintf(stderr, " -H # View simulation options and quit.\n");
        fprintf(stderr, " -bN # proportion of trees that are already burning. 0 < N < 101.\n");
        fprintf(stderr, " -cN # probability that a tree will catch fire. 0 < N < 101.\n");
        fprintf(stderr, " -dN # density: the proportion of trees in the grid. 0 < N < 101.\n");
        fprintf(stderr, " -nN # proportion of neighbors that influence a tree catching fire. -1 < N < 101.\n");
        fprintf(stderr, " -pN # number of states to print before quitting. -1 < N < ...\n");
        fprintf(stderr, " -sN # simulation grid size. 4 < N < 41.\n");
}

/**
 * Initializes the simulation grid with trees and empty cells.
 *
 * @param grid: The grid to be initialized
 */
void init_grid(int grid[size][size]) {
        int tot_cells = size * size;
        int tot_trees = (density * tot_cells) / 100;
        int tot_burning = (burning_percent * tot_trees) / 100;

        for(int r = 0; r < size; r++) {
                for(int c = 0; c < size; c++) {
                        grid[r][c] = EMPTY;
                }
        }

        srand(time(NULL));
        while(tot_trees > 0) {
                int row = rand() % size;
                int col = rand() % size;

                if(grid[row][col] == EMPTY) {
                        if(tot_burning > 0) {
                                grid[row][col] = BURNING;
                                tot_burning--;
                        } else {
                                grid[row][col] = TREE;
                        }
                        tot_trees--;
                }
        }
}

/**
 * Prints the current state of the grid.
 *      ' ' (space) represents an empty cell
 *      'Y' represents a living tree
 *      '*' represents a burning tree
 *      '.' represents a burned-out tree
 *
 * @param grid: The grid to be printed out
 */
void print(int grid[size][size]) {
        for(int r = 0; r < size; r++) {
                for(int c = 0; c < size; c++) {
                        if(grid[r][c] == EMPTY) printf(" ");
                        else if (grid[r][c] == TREE) printf("Y");
                        else if (grid[r][c] == BURNING) printf("*");
                        else printf(".");
                }
                printf("\n");
        }
}

/**
 * Updates the grid for the next step based on the current grid state.
 *
 * @param grid: The current grid state
 * @param next: The grid state for the next step
 * @param burn_counter: The burn counter for each tree
 * @param changes: A pointer to store the number of changes made in this update
 */
void update(int grid[size][size], int next[size][size], int burn_counter[size][size], int *changes) {
        *changes = 0;

        for(int r = 0; r < size; r++) {
                for(int c = 0; c < size; c++) {
                        if(grid[r][c] == BURNING) {
                                burn_counter[r][c]++;
                                if(burn_counter[r][c] >= BURN_STAGES) next[r][c] = BURNED;
                                else next[r][c] = BURNING;
                                (*changes)++;
                        } else if (grid[r][c] == TREE) {
                                int burning_neighbors = count_burning_neighbors(grid, r, c);
                                int total_neighbors = count_tree_neighbors(grid, r, c);

                                if(total_neighbors > 0) {
                                        double proportion_burning = (double)burning_neighbors / total_neighbors;
                                        if(proportion_burning >= (neighbor_effect / 100.0)) {

                                                if((rand() % 100) < catch_fire_percent) {
                                                        next[r][c] = BURNING;
                                                        burn_counter[r][c] = 0;
                                                        (*changes)++;
                                                } else {
                                                        next[r][c] = TREE;
                                                }
                                        } else {
                                                next[r][c] = TREE;
                                        }
                                } else {
                                        next[r][c] = TREE;
                                }
                        } else {
                                next[r][c] = grid[r][c];
                        }
                }
        }
}

/**
 * Counts the number of burning neighbors for a specific cell.
 *
 * @param grid: The grid to be checked
 * @param row: The row of the cell
 * @param col: The column of the cell
 * @return: The number of burning neighbors
 */
int count_burning_neighbors(int grid[size][size], int row, int col) {
        int burning_neighbors = 0;

        for(int i = row - 1; i <= row + 1; i++) {
                for(int j = col - 1; j <= col + 1; j++) {
                        if(i >= 0 && i < size && j >= 0 && j < size) {
                                if(!(i == row && j == col) && grid[i][j] == BURNING) {
                                        burning_neighbors++;
                                }
                        }
                }
        }
        return burning_neighbors;
}

/**
 * Counts the number of tree neighbors (burning or not)
 *
 * @param grid: The grid to be checked
 * @param row: The row of the cell
 * @param col: The column of the cell
 * @return: The number of tree neighbors
 */
int count_tree_neighbors(int grid[size][size], int row, int col) {
        int neighbors = 0;

        for(int i = row-1; i <= row+1; i++) {
                for(int j = col-1; j <= col+1; j++) {
                        if(i >= 0 && i < size && j >= 0 && j < size) {
                                if(!(i == row && j == col) && (grid[i][j] == TREE || grid[i][j] == BURNING)) {
                                        neighbors++;
                                }
                        }
                }
        }
        return neighbors;
}

/**
 * Simulates a lightning strike on the grid.
 *
 * @param grid: The current state of the grid
 * @param burn_counter: Tracks the burn stages of each burning tree
 */
void lightning(int grid[size][size], int burn_counter[size][size]) {
        if(lightning_active && ((double)rand() / RAND_MAX) < lightning_chance) {
                int strike_r = rand() % size;
                int strike_c = rand() % size;

                if(grid[strike_r][strike_c] == TREE) {
                        grid[strike_r][strike_c] = BURNING;
                        burn_counter[strike_r][strike_c] = 0;
                        printf("Lightning struck at (%d, %d)!\n", strike_r, strike_c);
                }
        }
}

/**
 * ANSI escape code to clear the terminal screen
 */
void clear_screen() {
        printf("\033[H\033[J");
}

/**
 * Moves the terminal cursor to a specific position
 *
 * @param row: The row of the cell
 * @param col: The column of the cell
 */
void move_cursor(int row, int col) {
        printf("\033[%d;%dH", row + 1, col + 1);
}

/**
 * Runs the wildfire simulation
 */
void run_sim() {
        int grid[size][size];
        int next[size][size];

        init_grid(grid);
        int cumulative_changes = 0;

        int burn_counter[size][size];
        for(int r = 0; r < size; r++) {
                for(int c = 0; c < size; c++) {
                        burn_counter[r][c] = 0;
                }
        }

//      clear_screen(); // clears terminal window
        if(mode == PRINT_MODE) {
                printf("===========================\n");
                printf("======== Wildfire =========\n");
                printf("===========================\n");
                printf("=== Print %02d Time Steps ===\n", current_cycle);
                printf("===========================\n");
        }

        while( 1 ) {
                if(mode == OVERLAY_MODE) {
                        clear_screen();
                        move_cursor(0,0);
                }

                printf("Cycle: %d\n", current_cycle);
                print(grid);

                if(max_cycles != -1 && current_cycle >= max_cycles) break;

                //move_cursor(size, 0);
                printf("size: %d, pCatch: %.2f, density: %.2f, pBurning: %.2f, pNeighbor: %.2f\n", size, catch_fire_percent / 100.0, density / 100.0, burning_percent / 100.0, neighbor_effect / 100.0);

                memset(next, EMPTY, sizeof(next));

                int changes_in_current_step = 0;
                lightning(grid, burn_counter);
                update(grid, next, burn_counter, &changes_in_current_step);
                cumulative_changes += changes_in_current_step;

                printf("cycle: %d, current changes: %d, cumulative changes: %d\n", current_cycle, changes_in_current_step, cumulative_changes);

                int fires_out = 1;
                for(int r = 0; r < size && fires_out; r++) {
                        for(int c = 0; c < size; c++) {
                                if(grid[r][c] == BURNING) {
                                        fires_out = 0;
                                        break;
                                }
                        }
                }

                if(fires_out) {
                        printf("Fires are out.\n");
                        break;
                }

                for(int r = 0; r < size; r++) {
                        for(int c = 0; c < size; c++) {
                                grid[r][c] = next[r][c];
                        }
                }
                current_cycle++;

                if(mode == OVERLAY_MODE) usleep(200000);
        }
        printf("Simulation finished after %d cycles.\n", current_cycle);
}





