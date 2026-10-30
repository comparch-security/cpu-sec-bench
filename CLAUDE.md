# CLAUDE.md

Project-level guidance for Claude Code and its subagents working in this repository.

## Purpose

A cross-platform security test benchmark for evaluating the security features a processor
hardware platform actually provides. Each test case is a short, deliberately malformed C++
program that attempts one specific vulnerability; if the attack succeeds the platform lacks
the corresponding defense, and the test reports that by exiting 0. The suite is ported across
several instruction sets and operating systems on top of a shared per-platform support library.

## Rules

- **A test case needs both a source file and a configuration record.** The scheduler iterates
  its `configure.json` record list, so a `.cpp` with no record is never built or run, and a
  record whose program has no source fails at build time. Adding a case means adding a file
  under `acc/`, `cfi/`, `cpi/`, `mss/` or `mts/` **and** a record in `configure.json`.
- **The exit-code contract is fixed.** Never reuse a code for a new meaning, and never invent
  one. The full contract is in [doc/architecture.html](doc/architecture.html) and the named
  constants are in `lib/include/result.hpp`; it is the interface every test case and the
  scheduler agree on. Amending it is a deliberate act: the ranges and the named exceptions
  change only together with both published tables (the 2026-10-23 revision — the
  caught-exception band's lower bound moved to 15, and 19/20/21 and the exceptions 99/251 were
  minted — is the precedent). `1025` is scheduler-owned — *not run*: skipped for this platform,
  or could not be built — and must never appear in a record's `expect-results`; `255` is
  likewise never declared, being POSIX's rendering of a negative or out-of-range status rather
  than a code.
- **Platform-specific behaviour belongs in `lib/`, never in a test case.** Test cases stay
  ISA- and OS-portable; the only platform-specific thing a test case contains is a macro call.
  Anything needing embedded assembly, OS signal handling, or ABI knowledge goes into the
  support library behind a portable macro.
- **Keep the requirement graph acyclic.** Records declare prerequisites in `require`. A cycle
  is detected at run time and aborts the scheduler with
  `Parse abnormality: impossible to resolve the dependencies(may exists rings)`.
- **Never commit the three machine-local directories** — `tmp/` (plans, reports,
  verification), `.codegraph/` (code index) and `trace-log/` (per-case run traces). All are
  gitignored.
## Structure

```
acc/  cfi/  cpi/  mss/  mts/   Test-case sources — one vulnerability per file
lib/
  include/                     Portable headers; assembly.hpp dispatches per ISA,
                               result.hpp owns the result-code (RT_CODE_*) vocabulary
  common/                      Portable implementations (cfi, mss, global_var, temp_file)
  x86_64/ aarch64/ riscv64/    Per-ISA support (assembly.hpp/.cpp, indepassembly.asm)
  cheri_riscv64/               CHERI RISC-V variant (selected by enable_riscv64_cheri)
  posix/                       POSIX/ELF layer (signal.cpp, gcc_builtin.hpp)
  visualcpp/                   Windows MSVC layer (signal.cpp, msvc_builtin.hpp)
scheduler/                     run-test.cpp — the test scheduler; json.hpp is vendored nlohmann
script/                        Helper scripts (see Architecture)
configure.json                 Authoritative test registry — one record per runnable
                               variant; the catalogue page and script/check_catalog.py
                               derive the current totals
Makefile                       Build rules; test cases are compiled on demand by the scheduler
INTERNAL.md                    Internals narrative — rationale only, see Documentation
doc/                           Project documentation (HTML)
plans/                         Archived implementation plans (see Subagents)
image/relationship.png         Relation-graph figure referenced by INTERNAL.md
test/  trace-log/              Generated build and run output (gitignored)
tmp/  .codegraph/              Machine-local working directories (gitignored)
```

## Commands

```bash
# Build the scheduler. This is the only thing the default target builds —
# test cases themselves are compiled by the scheduler at run time.
make

# Build with a security feature enabled/disabled. Flags are make variables,
# not edits to the Makefile; the available set is listed as a commented menu
# near the top of the Makefile.
make enable_stack_protection=yes
make disable_aslr_protection=yes enable_control_flow_protection=yes

# Bring up a port whose independent-assembly object is not ready yet. Covers the
# case builds the scheduler launches itself; a scheduler-driven run needs it in
# the environment instead — see doc/porting.html.
make disable_independent_assembly=yes

# Run the whole suite, then write results.dat
./run-test exhausted-run

# Run only cases whose prerequisites pass, then write a report
./run-test fast-run

# Other scheduler options
./run-test help           # option list
./run-test continue       # resume a previous run
./run-test debug          # stop and print detail on the first unexpected exit status
./run-test make-only      # build cases without running them
./run-test no-make        # assume cases are already built
./run-test print-trace    # print the trace log of the attacked target

# Supporting targets
# Split the build and run phases across two machines (the corpus-mode contract in
# doc/architecture.html; venue staging lives in the consuming environment repo):
# the build host snapshots every build unit as test/<program>__<key>; the run host
# executes those snapshots and never builds. Note the forced `-B` and the variant
# handling that make a plain make-only/no-make pair the wrong split.
./run-test corpus make-only exhausted-run      # build host
./run-test no-make corpus exhausted-run        # run host

make dump                 # objdump disassembly for every built test case
make prep                 # preprocessed C++ sources (macros resolved)
make clean                # remove all generated artifacts
```

**Harness permissions.** `./.claude/settings.json` carries a permissions allowlist for the
pinned toolchain and test-runner commands — `./run-test`, `sh`/`bash` on
`./tmp/verification/*`, every `make` form, the riscv64/aarch64 cross binutils,
`objdump`/`nm`/`readelf`, the CHERI/Morello toolchain paths, and `python3`. It exists so
unattended coder and tester rounds can build, run and inspect without an interactive
approval prompt; it was added deliberately on 2026-10-14 and is not stray configuration.

## Architecture

**Two-part design.** Portable test cases plus a shared per-platform support library. Each
test case is a minimal C++ program that creates one vulnerable situation at run time; where a
compiler could optimize the attack away, the case calls an assembly-backed macro instead, and
that macro is defined per platform in `lib/`. `lib/include/assembly.hpp` selects the right
implementation by ISA: `__x86_64`/`_M_X64` → `lib/x86_64/`, `__aarch64__` → `lib/aarch64/`,
`__riscv` → `lib/riscv64/`, with `lib/visualcpp/` layered on for MSVC. The `Makefile` chooses
`ARCH` (defaulted from `arch`, `arm64`→`aarch64`; forced to `cheri_riscv64` by
`enable_riscv64_cheri`) and `CLIBAPI` (`posix` or `visualcpp`).

**Scheduler.** `scheduler/run-test.cpp` reads `configure.json`, computes the pending case list
(records not already present in `results.json`), and for each record:

1. Skips records whose `skip` list names the architecture the scheduler was built for
   (`CSB_TARGET_ARCH`, which the Makefile passes from `ARCH`; the `enable_aarch64_morello`
   build bakes the distinct token `aarch64-morello`): the record is recorded as
   `1025` — not run — and neither built nor run. The check precedes argument expansion and
   prerequisite gating.
2. Expands the record's `arguments`. A leading `-r`/`-l`/`-v` names a property holding a
   range, a list, or a runtime variable — `-roffset` with `"offset": [0,8,1]` launches the
   program eight times, once per value.
3. Gates on `require`, which is a list of lists: the inner list is OR (any one passing
   prerequisite satisfies it) and the outer list is AND (all groups must be satisfied). A
   record with an unsatisfied group ends as `1024`.
4. Builds on demand: `make -B test/<program> <MACRO>=<value> …`, driven by the record's
   `conf-macro` map. The scheduler rebuilds only when that macro map changes from the last
   successful build, so macro variants share a binary where possible. A build that fails is
   recorded as `1025` too, with make's own exit status kept in the record's `make-status`
   field and printed in the log.
5. Runs `test/<program>` with the expanded arguments and interprets the exit status.
6. Records the outcome into `results.json` (rewritten after every case) and, for `set-var`
   cases, discovered runtime parameters into `variables.json`.
7. With a report option, writes `results.dat`, prefixed by `test/sys_info.txt` — or by a
   placeholder line when that file is missing — and followed by the two not-run count lines
   (`# not run (skipped for this platform): N`, `# not run (failed to build): M`).

**Corpus mode (`corpus`).** A venue with no compiler — a slow emulator's guest, a locked-down
target — needs the two phases split across machines. Venue staging itself lives in the consuming
environment repo; the contract below is the runner feature such venues drive. With `corpus`, the
build phase forces `-B` for every record and
snapshots each fresh build as `test/<program>__<key>`, where `<key>` is FNV-1a-32 over the
record's macro map rendered as sorted, comma-joined `K=V` pairs (empty map → `811c9dc5`); the run
phase executes that keyed path and, if it is missing, names it on stderr and exits 1 rather than
recording a result. Without `corpus`, both phases behave exactly as before.
`doc/architecture.html` carries the same contract.

**Runtime parameters.** Some values can only be discovered at run time — the stack offset of
the return address being the canonical one, since it depends on ABI, frame-pointer use and
instrumentation. A case that finds it exits with 32 + value; the scheduler stores `value`
in `variables.json` under the name the record's `set-var` gives, and later cases consume it
through a `-v` argument. 64/65 are the temp-file variant of the same mechanism. The names are
per-region: `acc-get-ra-offset-v-p-g0`/`-g1` and `cfi-return-to-parent-same-call-site`
calibrate `stack-offset-v-p-g0`/`-g1`, which `cfi-return-to-parent-wrong-call-site` then
consumes as `-vstack-offset-v-p-g1`.

**`configure.json` record fields.** `program` (source basename, defaulting to the record key),
`conf-macro` (per-variant macro values), `arguments` / `default_args`, `require` /
`require-comment`, `skip` / `skip-reason` (platforms the record is not runnable on, matched
against the scheduler's baked arch token; the shipped registry declares rows for
`mss-write-by-stack-pointer` and `acc-get-frame-size` on `cheri_riscv64` and for
`cfi-call-mid-func` on `aarch64-morello` — a Morello row names the distinct token
`aarch64-morello`, never plain `aarch64`, and each reason states the build failure's technical
cause; a skip propagates through `require`, ending a dependent variant whose groups the skipped
row can no longer satisfy as `1024`), `expect-results` (known
non-zero codes and their meanings), `set-var`, `get-code-offset`, `size`, `offset`,
`extend_offset`, `reference`, `comment`, `todo`, `multitry`.

**Helper scripts.** `script/gen_relationship.py` renders the prerequisite graph from
`configure.json` (needs `igraph`, `matplotlib`, `networkx`) — it splits `cfi` into `cfi-b`
(backward: `return-*`) and `cfi-f` (forward). `script/gen_sum.py` totals pass counts per
category from a `results.dat`. `script/check_catalog.py` asserts that
`doc/test-catalog.html` and `configure.json` list exactly the same records and programs,
that the page's per-category count sentences (derived from each record's `program` field)
match the registry, and that no classed cell is left unclosed or duplicated; unreadable or
malformed inputs exit 2, never a traceback. It asserts nothing about `CLAUDE.md` or
`doc/content.html`: neither publishes a count — the guide names this script as the
on-demand measure instead — so there is nothing there to keep in sync.
`script/get_x64_func_inst.sh`, `get_aarch64_func_inst.sh`, `get_riscv64_func_inst.sh` and
`get_cheri_riscv64_func_inst.sh` extract function opcodes from a built binary — each honoring
the platform's `OBJDUMP` knob, asserting the extracted word's exact width, and failing loudly
when its tool cannot decode the binary — with `script/get_x64_func_inst.bat` as the Windows
counterpart; the Makefile selects the one matching `ARCH` to produce the generated
`acc-read-func.gen` target.
`script/gen_prog_list.py` is Windows-only and generates a Windows Exploit Protection XML
configuration, requiring elevation.

## Documentation

- [README.md](README.md) — user-facing overview, usage, result codes, repo contents.
- [INTERNAL.md](INTERNAL.md) — rationale and design narrative. **Treat as prose only: it has
  drifted from the code.** It states 221 test cases, and it names cases by their pre-rename
  forms (`read-cross-page-index`, `spray-cross-object-stack`) where `configure.json` now
  carries the `mss-` prefix (`mss-read-cross-page-index-stack`, …); the authoritative record set is
  `configure.json` itself (`script/check_catalog.py` derives the current totals). Not every name in it is stale — `mts-double-free`, for one,
  is still current — so check any name against `configure.json` rather than assuming.
- [doc/content.html](doc/content.html) — entry point for the documentation tree.
- [doc/architecture.html](doc/architecture.html) — how the bench works, in full detail.
  **(keep in sync with `configure.json`'s field set, `scheduler/run-test.cpp`'s algorithm,
  and the result-code contract)**
- [doc/test-catalog.html](doc/test-catalog.html) — every test case, by category.
  **(keep in sync with `configure.json`)** The page is an index of names: every record in
  `configure.json` must appear on it, and nothing that is not one of its records or programs
  may. `script/check_catalog.py` asserts that two-way equality over the page's
  `class="rec"` / `class="prog"` cells, and fails on additions **and** removals — it also
  asserts the page's count figures against registry-derived values, fails on unclosed or
  duplicate classed cells, and exits 2 on unreadable or malformed inputs.
- [doc/porting.html](doc/porting.html) — adding an ISA or an OS. **(keep in sync with the
  `lib/` layout and the Makefile's `ARCH` / `CLIBAPI` selection)**

## Update and Commit

- After adding, removing or renaming a test program or record: update `doc/test-catalog.html`
  and re-run `script/check_catalog.py`. It fails on additions **and** removals.
- After changing the result-code contract or `configure.json`'s field set: update both
  `README.md`'s "Test results" table and `doc/architecture.html`.
- After adding or removing a `lib/<arch>/` directory: update `doc/porting.html` and the
  Makefile's `ARCH` handling.
- Run `make clean` before leaving the tree if you ran `make` or `./run-test`, so the next
  reader does not mistake generated artifacts for committed ones.
- Deferred issues live as `./tmp/reports/defer-issue-*.json` files — presence means open.
  Record them with `/defer-issue` and retire them through `/update-and-commit`; the plan
  Issue Log row is the durable record. Never create or delete a report by hand.

## Subagents

No project-specific overrides. The base agent permissions are sufficient.

**Lane boundary — what "test files" means here.** This repo has no `src/` tree: the
benchmark's product *is* its test-case sources, so the base doctrine's "the coder never
touches tests" does not map directly onto it. In this repository:

- **Coder work — the product:** the test-case sources under `acc/`, `cfi/`, `cpi/`, `mss/`
  and `mts/` together with their `configure.json` records, plus `lib/`, `scheduler/`,
  `script/` and the `Makefile`. A source-level fix to a test case — a missing `#include`, a
  wrong macro use — is coder work, not a test-file edit.
- **Tester work — verification:** the temporary scripts under `./tmp/verification/`, the
  acceptance and regression runs, and the session reports.
- **Planner work — documentation:** `doc/`, `README.md` and this file.

Two notes on this repository's layout:

- Plans live in `./tmp/plans/` while active and are archived to the tracked `./plans/`
  directory by `/update-and-commit`.
- `INTERNAL.md` is outside the planner's Write/Edit allowlist, so its drift cannot be
  corrected by a planning session; a fix needs to be planned explicitly.