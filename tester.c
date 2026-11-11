/*
 * Public exact-output tester for Clone Wars: Gregor and the C-Squad.
 *
 * Run with:
 *     make test
 *
 * For each public case, the tester runs:
 *     ./c_squad tests/inputs/<file> tester_outputs/<file>
 *
 * and compares the produced file byte-for-byte against:
 *     tests/expected/<file>
 *
 * The public-test score printed at the end is only a debugging aid. It is NOT
 * the assignment grade; hidden tests, memory correctness, code quality, and
 * other grading components are not represented here.
 */

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define TEST_TIMEOUT_SECONDS 10

typedef struct {
    const char *file;
    const char *label;
} TestCase;

static const TestCase TESTS[] = {
    {"01_basic_rescue.txt", "Basic rescue"},
    {"02_item_transfer.txt", "Inventory transfer"},
    {"03_ally_wall_reflection.txt", "Ally wall reflection"},
    {"04_gregor_corner_reflection.txt", "Gregor corner reflection"},
    {"05_commando_disables_ally.txt", "Commando disables ally"},
    {"06_ion_blaster_encounter.txt", "ION_BLASTER disables commando"},
    {"07_stunner_stuns_commando.txt", "STUNNER stuns commando"},
    {"08_stunrod_stuns_commando.txt", "STUNROD stuns commando"},
    {"09_unarmed_injury.txt", "Unarmed injury"},
    {"10_own_medkit_auto_use.txt", "Automatic MEDKIT use"},
    {"11_triple_collision_medkit_transfer.txt", "Triple collision + MEDKIT transfer"},
    {"12_stunned_commando_harmless_to_ally.txt", "Stunned commando is harmless"},
    {"13_commando_reactivation.txt", "Two-turn commando reactivation"},
    {"14_equipment_priority.txt", "Equipment priority"},
    {"15_inventory_growth_and_order.txt", "Inventory growth + ordering"},
    {"16_rescue_rate_50_boundary.txt", "50 percent boundary"},
    {"17_rescue_rate_success.txt", "Success percentage"},
    {"18_gregor_captured_after_injury.txt", "Gregor capture after injury"},
    {"19_capture_triple_collision_timing.txt", "Capture triple-collision timing"},
    {"20_full_scenario.txt", "Combined challenge scenario"}
};

static const size_t TEST_COUNT = sizeof(TESTS) / sizeof(TESTS[0]);

static int file_exists(const char *path) {
    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        return 0;
    }
    fclose(f);
    return 1;
}

static int ensure_output_directory(void) {
    if (mkdir("tester_outputs", 0777) == 0 || errno == EEXIST) {
        return 1;
    }
    perror("mkdir tester_outputs");
    return 0;
}

/*
 * Returns:
 *   0..255 : child exit status
 *   124    : timeout
 *   125    : tester-side process error
 */
static int run_student_program(const char *input_path, const char *output_path) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return 125;
    }

    if (pid == 0) {
        execl("./c_squad", "./c_squad",
              input_path, output_path, (char *)NULL);
        perror("execl ./c_squad");
        _exit(127);
    }

    const long checks_per_second = 10;
    const long max_checks = TEST_TIMEOUT_SECONDS * checks_per_second;
    const struct timespec sleep_time = {0, 100000000L}; /* 0.1 s */

    for (long i = 0; i < max_checks; ++i) {
        int status = 0;
        pid_t result = waitpid(pid, &status, WNOHANG);

        if (result == pid) {
            if (WIFEXITED(status)) {
                return WEXITSTATUS(status);
            }
            if (WIFSIGNALED(status)) {
                return 128 + WTERMSIG(status);
            }
            return 125;
        }

        if (result < 0) {
            perror("waitpid");
            return 125;
        }

        nanosleep(&sleep_time, NULL);
    }

    kill(pid, SIGKILL);
    waitpid(pid, NULL, 0);
    return 124;
}

static void print_byte(FILE *stream, int c) {
    if (c == EOF) {
        fprintf(stream, "<EOF>");
    } else if (c == '\n') {
        fprintf(stream, "\\n");
    } else if (c == '\r') {
        fprintf(stream, "\\r");
    } else if (c == '\t') {
        fprintf(stream, "\\t");
    } else if (c >= 32 && c <= 126) {
        fprintf(stream, "'%c'", c);
    } else {
        fprintf(stream, "0x%02X", (unsigned int)(unsigned char)c);
    }
}

/* Returns 1 for an exact match and 0 otherwise. */
static int compare_exact(const char *expected_path, const char *actual_path) {
    FILE *expected = fopen(expected_path, "rb");
    FILE *actual = fopen(actual_path, "rb");

    if (expected == NULL || actual == NULL) {
        if (expected == NULL) {
            fprintf(stderr, "Could not open expected output: %s\n", expected_path);
        }
        if (actual == NULL) {
            fprintf(stderr, "Could not open student output: %s\n", actual_path);
        }
        if (expected != NULL) fclose(expected);
        if (actual != NULL) fclose(actual);
        return 0;
    }

    size_t byte_index = 0;
    size_t line = 1;
    size_t column = 1;

    for (;;) {
        int e = fgetc(expected);
        int a = fgetc(actual);

        if (e != a) {
            printf("\n      first difference: byte %zu, line %zu, column %zu\n",
                   byte_index, line, column);
            printf("      expected: ");
            print_byte(stdout, e);
            printf("\n      actual:   ");
            print_byte(stdout, a);
            printf("\n");
            fclose(expected);
            fclose(actual);
            return 0;
        }

        if (e == EOF) {
            fclose(expected);
            fclose(actual);
            return 1;
        }

        ++byte_index;
        if (e == '\n') {
            ++line;
            column = 1;
        } else {
            ++column;
        }
    }
}

int main(void) {
    if (!ensure_output_directory()) {
        return EXIT_FAILURE;
    }

    int passed = 0;
    int failed = 0;

    printf("Operation: C-Squad - Escape from Abafar - Public Tester\n");
    printf("============================================================\n");

    for (size_t i = 0; i < TEST_COUNT; ++i) {
        char input_path[256];
        char expected_path[256];
        char actual_path[256];

        snprintf(input_path, sizeof(input_path),
                 "tests/inputs/%s", TESTS[i].file);
        snprintf(expected_path, sizeof(expected_path),
                 "tests/expected/%s", TESTS[i].file);
        snprintf(actual_path, sizeof(actual_path),
                 "tester_outputs/%s", TESTS[i].file);

        printf("[%02zu/%02zu] %-36s ",
               i + 1, TEST_COUNT, TESTS[i].label);
        fflush(stdout);

        if (!file_exists(input_path) || !file_exists(expected_path)) {
            printf("ERROR (test files missing)\n");
            ++failed;
            continue;
        }

        remove(actual_path);
        int exit_code = run_student_program(input_path, actual_path);

        if (exit_code == 124) {
            printf("FAIL (timeout after %d s)\n", TEST_TIMEOUT_SECONDS);
            ++failed;
            continue;
        }

        if (exit_code != 0) {
            printf("FAIL (program exited with code %d)\n", exit_code);
            ++failed;
            continue;
        }

        if (compare_exact(expected_path, actual_path)) {
            printf("PASS\n");
            ++passed;
        } else {
            printf("      result: FAIL (output mismatch)\n");
            printf("      expected: %s\n", expected_path);
            printf("      actual:   %s\n", actual_path);
            ++failed;
        }
    }

    double percentage = TEST_COUNT == 0
                        ? 0.0
                        : 100.0 * passed / (double)TEST_COUNT;

    printf("============================================================\n");
    printf("PASS: %d   FAIL: %d\n", passed, failed);
    printf("PUBLIC TEST SCORE: %d/%zu (%.2f%%)\n",
           passed, TEST_COUNT, percentage);
    printf("Note: this public-test score is not your assignment grade.\n");

    return failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
