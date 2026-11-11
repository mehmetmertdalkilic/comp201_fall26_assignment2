Clone Wars: Gregor and the C-Squad - Student Starter Package
===================================================

This package contains the starter code and 20 public exact-output tests for
COMP 201 Assignment 2.

This is a what-if scenario inspired by The Clone Wars episode "Missing in Action".
No familiarity with the TV series is required. All official rules are in the handout.

Files
-----
main.c
    Starter source file. The required structs, suggested helper-function
    declarations, implementation hints, and an implementation roadmap are
    already included. Complete the TODO in this file.

Makefile
    Builds the simulator and public tester.

tester.c
    Public exact-output tester. You do not need to modify this file.

tests/inputs/
    20 public input files, ordered approximately from easier to harder.

tests/expected/
    The corresponding expected output files.

tests/README.txt
    A short explanation of what each public case is intended to exercise.

Quick start
-----------
1. Compile your simulator:

       make

2. Run all public tests:

       make test

3. Remove generated executables and tester output files:

       make clean

You can also run one case manually. For example:

    ./c_squad tests/inputs/01_basic_rescue.txt my_output.txt

Then compare it with:

    tests/expected/01_basic_rescue.txt

Recommended workflow
--------------------
Do not try to implement everything at once. The public tests are deliberately
ordered so that the early cases focus on individual rules and the later cases
combine several rules. A useful workflow is to make the earliest failing case
pass before moving on.

The tester compares files byte-for-byte. Spaces, capitalization, line breaks,
and the final newline therefore matter.

The score printed by the public tester is only a debugging aid. It is NOT your
assignment grade. The official grader may include hidden cases, memory checks,
code-quality requirements, version-control requirements, and other criteria
listed in the assignment handout.

Memory checking
---------------
Passing the public output tests does not prove that your heap management is
correct. Test your program with Valgrind on the course Linux environment. For
example:

    valgrind --leak-check=full --show-leak-kinds=all \
        ./c_squad tests/inputs/15_inventory_growth_and_order.txt /tmp/out.txt

Read the assignment handout carefully for the authoritative game rules and
submission requirements.
