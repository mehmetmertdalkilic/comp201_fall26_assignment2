/*
 * COMP 201 - Fall 2026
 * Assignment 2: Clone Wars: Gregor and the C-Squad
 *
 * YOUR_NAME, YOUR_ID
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    ALLY_DROID,
    GREGOR,
    COMMANDO_DROID
} CharacterType;

typedef enum {
    GREGOR_OK,
    GREGOR_INJURED,
    GREGOR_CAPTURED
} GregorStatus;

typedef enum {
    COMMANDO_ACTIVE,
    COMMANDO_STUNNED
} CommandoStatus;

typedef struct Character {
    CharacterType type;

    /* Character coordinates */
    int row;
    int col;

    /* Current movement direction. Each component is -1, 0, or +1. */
    int d_row;
    int d_column;

    /* Used while computing simultaneous movement. */
    int next_row;
    int next_col;

    /* Dynamically growing array of dynamically allocated item strings. */
    char **inventory;
    int item_count;
    int item_capacity;

    GregorStatus gregor_status;       /* meaningful only for GREGOR */
    CommandoStatus commando_status;   /* meaningful only for COMMANDO_DROID */
    int stun_turns;               /* meaningful only for a STUNNED COMMANDO_DROID */
} Character;

typedef struct {
    /* Dynamically growing array of Character pointers. */
    Character **characters;
    int count;
} Cell;

typedef struct {
    /* Dynamically allocated two-dimensional grid of Cells. */
    Cell **cells;
    int height;
    int width;
} Grid;

/*
 * ==========================================================================
 * START HERE
 * ==========================================================================
 *
 * Do not try to implement the whole simulation in one pass. A good approach is
 * to make the early public tests pass first, then move on to the later ones.
 * The 20 provided tests are arranged approximately from easier to harder.
 *
 * The supplied structs are part of the intended design. You may add helper
 * functions and local variables as needed, but keep the required behavior and
 * output format from the handout.
 */

/*
 * --------------------------------------------------------------------------
 * SUGGESTED HELPER FUNCTIONS
 * --------------------------------------------------------------------------
 *
 * You are NOT required to use these exact functions or signatures. They are
 * provided only as a possible way to split the assignment into manageable
 * pieces. If you use them, you must write the implementations yourself.
 */

Grid *create_grid(int height, int width);
Character *create_character(CharacterType type, int row, int col);

void add_character(Cell *cell, Character *character);
void remove_character(Cell *cell, Character *character);

void add_item(Character *character, const char *item_name);
void remove_item(Character *character, int index);
int find_item(const Character *character, const char *item_name);

void compute_next_position(const Grid *grid, Character *character);
void apply_all_movements(Grid *grid /* add anything else you need */);
void resolve_cell(Grid *grid, int row, int col /* add anything else you need */);
void update_stunned_commandos(Grid *grid);

void print_turn(FILE *output, const Grid *grid,
                const Character *gregor, int turn);

void free_character(Character *character);
void free_grid(Grid *grid);

/*
 * --------------------------------------------------------------------------
 * IMPLEMENTATION HINTS
 * --------------------------------------------------------------------------
 *
 * 1) GRID ALLOCATION
 *
 *    A convenient representation is:
 *
 *        grid->cells = malloc(height * sizeof(Cell *));
 *
 *    followed by one allocation for each row. Remember to initialize every
 *    Cell so that:
 *
 *        characters == NULL
 *        count == 0
 *
 *    Draw the memory layout on paper if the Cell ** representation is
 *    confusing.
 *
 *    Useful type reminders:
 *
 *        grid->cells                 has type Cell **
 *        grid->cells[row]            has type Cell *
 *        grid->cells[row][col]       has type Cell
 *        &grid->cells[row][col]      has type Cell *
 *
 *    When allocating an array, sizeof(*pointer) is often the safest style:
 *
 *        grid->cells = malloc(height * sizeof(*grid->cells));
 *        grid->cells[row] = malloc(width * sizeof(*grid->cells[row]));
 *
 *    The first allocation stores Cell * row pointers; the second stores actual
 *    Cell objects.
 *
 *
 * 2) CHARACTERS SHOULD BE ALLOCATED ONLY ONCE
 *
 *    A Character should have one malloc when it is created. When it moves,
 *    move its POINTER from one Cell to another. Do not copy/recreate the
 *    Character on every turn.
 *
 *        old cell  ---- Character * ----> Character on the heap
 *                                        ^
 *        new cell  ---- Character * -----|
 *
 *    The old pointer should be removed from the old cell before/while the
 *    same pointer is inserted into the destination cell.
 *
 *
 * 3) GROWING A CELL
 *
 *    Cell.characters is an array of Character * values. When a new pointer
 *    must be added, resize the array with realloc. Use a temporary pointer:
 *
 *        Character **tmp = realloc(...);
 *
 *    and update cell->characters only after checking tmp != NULL.
 *
 *    When removing a pointer, shift the later pointers left so that the
 *    first cell->count entries are always valid.
 *
 *
 * 4) INVENTORY OWNERSHIP
 *
 *    inventory is a dynamic array of char * pointers. Each item string must
 *    have its own heap allocation. Keep item_count and item_capacity separate.
 *
 *    A common growth strategy is to start with a small positive capacity and
 *    double it whenever item_count == item_capacity. Other correct strategies
 *    are also acceptable.
 *
 *    When an ally droid is RESCUED, the item strings must remain alive after the
 *    ally is freed. One option is to transfer the existing char * pointers
 *    into Gregor's inventory, then free only the ally's inventory ARRAY.
 *    Another option is a correct deep copy. Please be consistent about ownership.
 *
 *    When an ally droid is DISABLED, nobody receives the items, so both the strings and
 *    the inventory array must be freed.
 *
 *
 * 5) SIMULTANEOUS MOVEMENT -- VERY IMPORTANT!
 *
 *    Do NOT move one character and immediately resolve its collision. A safe
 *    high-level plan for each turn is:
 *
 *      Phase A: compute next_row/next_col for EVERY movable character.
 *      Phase B: move all Character * pointers to their destination cells.
 *      Phase C: update the characters' row/col values.
 *      Phase D: only then resolve interactions in the cells.
 *
 *    This prevents the result from depending on the order in which you scan
 *    the grid.
 *
 *    Be especially careful when modifying a Cell.characters array while
 *    iterating over it. Removing index i shifts later elements left.
 *
 *
 * 6) WALL REFLECTION
 *
 *    First calculate the intended next position. If a component would leave
 *    the grid, flip that direction component and THEN compute the destination.
 *
 *    For Gregor, the row and column components reflect independently, so both
 *    may flip at a corner.
 *
 *
 * 7) STUNNED COMMANDO DROIDS
 *
 *    COMMANDO_STUNNED characters do not move or hurt anyone. A commando newly stunned
 *    during the current turn must still remain stunned for the NEXT TWO COMPLETE
 *    turns. Do not accidentally decrement its timer immediately in the same
 *    turn in which it became stunned.
 *
 *    When it becomes ACTIVE again, keep its previous movement direction.
 *
 *
 * 8) INTERACTION ORDER
 *
 *    Read the handout rules carefully before implementing resolve_cell().
 *    In particular, for GREGOR + ALLY_DROID + COMMANDO_ACTIVE, Gregor's encounter with
 *    the commando is resolved before the ally transfers any items.
 *
 *    Ally droids never use the items they carry themselves.
 *
 *
 * 9) ITEM REMOVAL
 *
 *    When Gregor consumes an item:
 *      - free that item's string exactly once,
 *      - shift the later char * pointers left,
 *      - decrement item_count.
 *
 *    Preserve the relative order of the remaining items. Items received from
 *    a rescued ally should be appended in the ally's existing order.
 *
 *
 * 10) OUTPUT
 *
 *    For each cell, decide what one character symbol to print using the
 *    priority from the handout. The display does not change what pointers are
 *    actually stored in the Cell.
 *
 *    Exact formatting matters: the provided tester compares output files
 *    byte-for-byte.
 *
 *
 * 11) CLEANUP
 *
 *    Think "inside out": item strings -> inventory arrays -> Characters ->
 *    cell character-pointer arrays -> grid rows -> grid itself.
 *
 *    Do not free a Character merely because it moved out of a cell. Free it
 *    only when the game rules destroy that character or during final cleanup.
 *
 *    Gregor must stay allocated long enough to print his inventory if
 *    he is captured.
 */

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file.txt> <output_file.txt>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *input = fopen(argv[1], "r");
    if (input == NULL) {
        fprintf(stderr, "Error: could not open input file '%s'.\n", argv[1]);
        return EXIT_FAILURE;
    }

    FILE *output = fopen(argv[2], "w");
    if (output == NULL) {
        fprintf(stderr, "Error: could not open output file '%s'.\n", argv[2]);
        fclose(input);
        return EXIT_FAILURE;
    }

    /*
     * A possible implementation order is shown below. Treat this as a roadmap,
     * not as required pseudocode.
     *
     * ----------------------------------------------------------------------
     * STEP 1: READ THE HEADER
     * ----------------------------------------------------------------------
     * Read:
     *      GRID <height> <width>
     *      CHARACTERS <count>
     *
     * Then allocate and initialize the Grid.
     *
     * ----------------------------------------------------------------------
     * STEP 2: READ AND CREATE THE CHARACTERS
     * ----------------------------------------------------------------------
     * For every character line:
     *   - convert the type text (GREGOR/ALLY_DROID/COMMANDO_DROID) to CharacterType,
     *   - allocate one Character,
     *   - initialize its position, direction, status, and inventory,
     *   - allocate/copy each item string,
     *   - insert the Character * into the correct Cell.
     *
     * It will probably be useful to keep a separate Character *gregor pointer.
     * Also store the initial number of allied droids for the final percentage.
     *
     * Initial states from the handout:
     *   GREGOR   -> GREGOR_OK
     *   COMMANDO_DROID -> COMMANDO_ACTIVE, stun_turns = 0
     *
     * ----------------------------------------------------------------------
     * STEP 3: SIMULATION LOOP
     * ----------------------------------------------------------------------
     * Repeat until a termination condition is reached:
     *
     *   1. Compute ALL destinations (next_row, next_col).
     *   2. Apply ALL movements.
     *   3. Resolve cell interactions.
     *   4. Update already-stunned commando droids' timers.
     *   5. Print the turn.
     *   6. Check whether the simulation is over.
     *
     * You may need a small amount of extra bookkeeping to distinguish commando droids
     * that were already stunned at the beginning of the turn from commando droids that
     * were newly stunned during this turn.
     *
     * ----------------------------------------------------------------------
     * STEP 4: RESULT + CLEANUP
     * ----------------------------------------------------------------------
     * Print the final result exactly as required by the handout, then free all
     * remaining heap allocations exactly once.
     *
     * Tip: run `make test` frequently. The public tests are ordered roughly from
     * basic behavior to combined edge cases, so the first failing test is often
     * a useful place to continue debugging.
     */

    /* TODO: Implement the simulation. */

    fclose(output);
    fclose(input);
    return EXIT_SUCCESS;
}
