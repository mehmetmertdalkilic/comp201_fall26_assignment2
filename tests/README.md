Clone Wars: Gregor and the C-Squad - Public Test Guide
=========================================================

Run all public cases with: make test

The tester compares each output against tests/expected/ byte-for-byte.
The cases are arranged approximately from easier to harder.

01_basic_rescue.txt
    Basic movement, contact with an ally droid, rescue, and termination.

02_item_transfer.txt
    Rescuing an ally appends multiple items to Gregor's inventory in their original order.

03_ally_wall_reflection.txt
    An ally droid reflects from the right wall before moving left.

04_gregor_corner_reflection.txt
    Gregor's diagonal movement reflects in both axes at a corner.

05_commando_disables_ally.txt
    An active Commando Droid intercepts and disables an ally outside Gregor's cell.

06_ion_blaster_encounter.txt
    During a three-character encounter, the ION_BLASTER permanently disables the patrol before Gregor rescues the ally.

07_stunner_stuns_commando.txt
    A STUNNER temporarily stuns a Commando Droid, leaving it on the grid.

08_stunrod_stuns_commando.txt
    A STUNROD produces the same two-turn stun behavior.

09_unarmed_injury.txt
    Gregor, initially OK and unarmed, stuns a Commando Droid but becomes INJURED, then rescues an ally.

10_own_medkit_auto_use.txt
    An injured Gregor uses the MEDKIT already in his inventory immediately.

11_triple_collision_medkit_transfer.txt
    An ally's MEDKIT cannot help Gregor before his encounter; after the rescue it can restore him immediately.

12_stunned_commando_harmless_to_ally.txt
    An ally remains safe when sharing a cell with a stunned Commando Droid.

13_commando_reactivation.txt
    Two full stunned turns, reactivation, preserved patrol direction, and a later encounter.

14_equipment_priority.txt
    ION_BLASTER takes priority over STUNNER and STUNROD when all are available.

15_inventory_growth_and_order.txt
    Repeated rescues trigger inventory growth while preserving item order and duplicates.

16_rescue_rate_50_boundary.txt
    Exactly 1 of 2 allies rescued: 50.00% produces FAILED.

17_rescue_rate_success.txt
    Exactly 2 of 3 allies rescued: 66.67% produces SUCCESS, alongside several other rules.

18_gregor_captured_after_injury.txt
    A second unprotected encounter while INJURED leads to GREGOR CAPTURED.

19_capture_triple_collision_timing.txt
    An ally carrying ION_BLASTER and MEDKIT cannot transfer them before Gregor is captured; the ally is disabled too.

20_full_scenario.txt
    Combined challenge: four allies, three patrol droids, reflections, injury, MEDKIT, stun timer, inventory transfer, and the 50.00% boundary.

Important
---------
These are public examples, not the entire specification.
Hidden tests and memory checks may also be used.
The public-test score is not the assignment grade.
