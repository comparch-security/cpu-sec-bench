# Plan: Complete the enable_riscv64_cheri flag set — the assembler target for the independent-assembly object
**Project:** /home/wsong83/proj/xiangshan/cpu-sec-test-env/benchmark
**Plan ID:** 2026-10-18-cheri-asmflags
**Created:** 2026-10-18

## Issue Log

Rows ordered newest-first.

| Issue | Status | Found | Resolved | Resolved By |
|-------|--------|-------|----------|-------------|
| [cheri-asm-size-directives](./tmp/reports/defer-issue-cheri-asm-size-directives.json) — CHERI linker warns on the unsized capability relocation for `assembly_return_site` (`lib/cheri_riscv64/indepassembly.asm` declares no `.size` directives). Pre-existing and non-blocking (link succeeds); `lib/` is out of this plan's scope. Deferred 2026-10-18. | Open | 2026-10-18 | — | — |
| [review-lines-bench-expectation-gaps](../../../tmp/reports/defer-issue-review-lines-bench-expectation-gaps.json) — three records' result codes fall outside their `expect-results` (255/1 reproduce on the native reference, 267 riscv64-only); this plan's suite run reproduces them identically. Cross-repo deferral (env repo, `target_repo: comparch-security/cpu-sec-bench`); fix planned in `2026-10-14-deferred-issue-sweep` Step 4. | Deferred (cross-repo) | 2026-10-18 | — | — |
| [cheri-asmflags-step1-script-stale-object](./tmp/reports/2026-10-18-cheri-asmflags-step1-script-stale-object.json) | Resolved | 2026-10-18 | 2026-10-18 | tester |
| cheri-asm-target-flags — `enable_riscv64_cheri` adds no `--target` to `CXXFLAGS`/`OBJECT_CXXFLAGS` and no `ASMFLAGS` at all: with a caller-supplied CTSRD `clang++`, the C++ path fails on `-mabi=` and the asm recipe assembles RISC-V text for the host triple. Surfaced by the env plan `2026-10-18-drop-indepassembly-workarounds` (2026-10-18, probe of `lib/cheri_riscv64/indepassembly.asm`: no target → `invalid instruction mnemonic 'addi'`; with the triple → rc=0, RISC-V FreeBSD ELF). This plan closes it bench-side. | Resolved | 2026-10-18 | 2026-10-18 | tester |

## Guidance for Coder

**Files to modify:**

- `./Makefile`

**Do NOT:** touch `lib/`, the scheduler, `configure.json`, or any other file; change the existing `-mno-relax` / `-march=rv64gcxcheri` / `-mabi=l64pc128d` / `-cheri-bounds=very-aggressive` flags; add `--target` to `SCHEDULER_CXXFLAGS` (the scheduler stays a host binary); rename or remove `disable_independent_assembly`.

**Step-by-step with verification:** Step 1 is coder-owned; Step 2 is tester-owned. `(auto)` checks are mechanical commands whose output is the check; the `(scripted)` check is `python3 ./tmp/verification/2026-10-18-cheri-asmflags-step1.py`, run from the bench repo root (exit 0 = pass; proven red against the pre-change tree).

## Guidance for Tester

**Tests to create:**

- **Temporary:** `./tmp/verification/2026-10-18-cheri-asmflags-linkcheck.sh` — the link-stage proof the translation plan's reviewer left open (its caveat 2). With `CXX`/`ASM` set to the built CTSRD SDK `clang++` **path only** — `CXX` additionally carrying `--sysroot=<the env's CHERI_SYSROOT_RISCV64 pin>`, and **neither** carrying `--target`, `-march` or `-mabi` — run from the bench root:

  `make -B test/cfi-return-to-peer-asmfunc test/cfi-return-to-parent-non-call-site-by-asmfunc enable_riscv64_cheri=yes ARCH=cheri_riscv64 CXX="$CLANG --sysroot=$SYSROOT" ASM="$CLANG" OBJDUMP=riscv64-linux-gnu-objdump`

  Assert: exit 0 (both units link — the first link-stage exercise of the translated object); `readelf -h` on each binary reports machine RISC-V; `nm` lists both `assembly_helper` and `assembly_return_site` as defined text symbols; then `make clean enable_riscv64_cheri=yes ARCH=cheri_riscv64` and assert `git status --short` is empty **and** `lib/cheri_riscv64/indepassembly.o` does not exist (the object is git-ignored, so git status alone cannot see it). Also re-run `python3 ./tmp/verification/2026-10-18-cheri-asmflags-step1.py` and record its exit status. Record the exact `CXX`/`ASM` strings in the report: the proof is that the bench's own flag block supplies the triple/march/mabi, not the caller.

**Tests to investigate for retirement:** No test obsolescence identified — no test case, record or helper changes.

## Summary

**Problem.** The bench's per-platform flag blocks are designed to make a configuration self-sufficient apart from the compiler choice. `enable_aarch64_morello` carries `--target=aarch64-unknown-freebsd` on `CXXFLAGS` and `OBJECT_CXXFLAGS` and — since the translation plan — an `ASMFLAGS` line. `enable_riscv64_cheri` carries only `-mno-relax -march=rv64gcxcheri -mabi=l64pc128d -cheri-bounds=very-aggressive` on the two C++ lines: no `--target`, no `ASMFLAGS`. The gap was invisible while every consumer built with `disable_independent_assembly=yes`; the env's successor plan (drop the workaround, bump `BENCH_COMMIT`, re-run all lanes) is the first consumer to build the object, and it fails: the CTSRD clang defaults to the x86-64 host triple, rejecting `-mabi=l64pc128d` for C++ and `addi` as an instruction for the assembly (probe, 2026-10-18, with the built SDK). The env could smuggle `--target` through `ASM=`, but that leaves the bench's own block incorrect for every other consumer — the same class of gap the Morello commit fixed.

**Approach.** Mirror the Morello block in `enable_riscv64_cheri`: prepend `--target=riscv64-unknown-freebsd` to the `CXXFLAGS` and `OBJECT_CXXFLAGS` lines and add

```
ASMFLAGS += --target=riscv64-unknown-freebsd -march=rv64gcxcheri -mabi=l64pc128d
```

The purecap sysroot and the compiler binary stay caller-supplied: the sysroot is a build artifact at a machine-local path the bench cannot own, and the toolchain choice is already the caller's (`CXX=`/`ASM=`).

**Alternatives considered:** caller-supplied `--target` (the status quo, and what the env would otherwise do) — rejected: that is the workaround being removed, and it repeats per consumer; adding only `ASMFLAGS` — rejected: a consumer passing only `CXX=`/`ASM=` paths still fails the C++ compile on `-mabi=`, and the Morello precedent owns all three lines.

**Out of scope:** the purecap sysroot pin; the pre-existing `$(ARCH)` evaluation quirk at `Makefile:251` (masked by command-line `ARCH=`, noted by the translation plan's reviewer as S5); the env-side workaround removal, pin bump and lane re-runs (env plan `2026-10-18-drop-indepassembly-workarounds`).

**Base and handoff.** This plan is based on `db843151e3187e2c136ecbdf4befc0794f1cea78` (the current detached checkout of the submodule; clean tree). Its fix commit must be **pushed to `origin/master`** before it can be pinned: env plan `2026-10-18-drop-indepassembly-workarounds` sets `BENCH_COMMIT` to this plan's commit, checks the submodule out at it, and re-runs every non-x86 lane. The bench repo is public; the commit message follows the repo's own conventions.

**Tester prerequisites (build artifacts of the env repo, verified present 2026-10-18):** CTSRD SDK `clang++` at `../build/cheri/output/sdk/bin/clang++` (keep the `clang++` spelling — clang picks its driver mode from `argv[0]`) and the purecap sysroot at `../build/cheri/output/sdk/sysroot-riscv64-purecap` (the env's `CHERI_SYSROOT_RISCV64` pin). If either is absent, the test cannot run on this host — say so in the report rather than skipping silently.

## Repo Mode

Public

## Document Overrides

| Document | Rule Overridden | Reason |
|----------|----------------|--------|

## Risks

| Risk | Impact | Mitigation |
|------|--------|------------|
| `--target` in `CXXFLAGS` also rides the link line | none — the link target must match the objects | intended; the same lines already carry `-march`/`-mabi`; the dry run and the tester's link confirm it |
| The C++ path's `-mabi=` failure is only masked, not fixed, if the triple lands on the wrong line | the link-stage test still fails | Step 1's scripted check asserts the exact triple on both C++ lines and the ASMFLAGS line separately, and runs the real assemble rule |

## Proposed Changes

Each step includes verification checks tagged with confidence. `(auto)` = mechanical, coder self-verifies. `(scripted)` = deterministic multi-step, coder runs the provided script. Every step has at least one verification check executable by its owner.

1. **Step 1:** In the `enable_riscv64_cheri` block of `Makefile`, prepend `--target=riscv64-unknown-freebsd` to the `CXXFLAGS` and `OBJECT_CXXFLAGS` lines, add `ASMFLAGS += --target=riscv64-unknown-freebsd -march=rv64gcxcheri -mabi=l64pc128d`, and add the comment mirroring the Morello block ("the CTSRD clang defaults to the host triple … --target is mandatory on every path"). Leave `SCHEDULER_CXXFLAGS += -mno-relax` unchanged.
   → coder verify (auto): `sed -n '/ifdef enable_riscv64_cheri/,/^endif/p' Makefile` shows `--target=riscv64-unknown-freebsd` on the `CXXFLAGS`, `OBJECT_CXXFLAGS` and new `ASMFLAGS` lines and not on `SCHEDULER_CXXFLAGS`; `make -n -B lib/cheri_riscv64/indepassembly.o enable_riscv64_cheri=yes ARCH=cheri_riscv64 ASM=<SDK clang++> | grep indepassembly` prints an assemble command carrying `--target=riscv64-unknown-freebsd -march=rv64gcxcheri -mabi=l64pc128d`; `make -n -B run-test enable_riscv64_cheri=yes` prints a host scheduler compile line with no `--target`
   → coder verify (scripted): `python3 ./tmp/verification/2026-10-18-cheri-asmflags-step1.py` — block flags exact per line, scheduler line untouched, and a real `make -B lib/cheri_riscv64/indepassembly.o` with the SDK `ASM` produces a RISC-V FreeBSD ELF defining `assembly_helper`/`assembly_return_site`
2. **Step 2 (tester):** Run the link-stage check over the two asmfunc units with caller-supplied toolchain paths only (see Guidance for Tester), then clean and confirm the tree is clean.
   → tester verify: both units link; `readelf -h` reports RISC-V; `nm` shows both symbols defined; `git status --short` empty after `make clean`; the exact caller `CXX`/`ASM` strings are recorded in the tester report

## Guidance for Planner

Documentation tasks you will execute yourself (not the coder):

- **Files to modify:**
  - `./doc/porting.html`
- **When:** after Step 2 passes, before the final revision.
- **What to sync:** the hardware-switch paragraph — add `--target=riscv64-unknown-freebsd` to the `enable_riscv64_cheri` option list; and the following paragraph ("The `--target` triple is part of that requirement …") — both CHERI blocks now carry the triple themselves, so the caller requirement is the toolchains (`CXX=`/`ASM=`) plus the purecap sysroot.
- **`./CLAUDE.md`:** no change — its rules and command menu do not reference the triple.

## History

| Date | Event | Conclusion | Dismissed |
|------|-------|------------|-----------|
| 2026-10-18 | Initial plan | Complete the cheri flag block by mirroring the Morello precedent; small plan, 2 steps, 1 source file, no mega-audit trigger. | — |
| 2026-10-18 | Spec revision (/update-plan) | Coder round 1 succeeded; Step-1 script now cleans with the cheri configuration active and asserts the object is gone, and the tester's tree-clean assertion is hardened for the git-ignored object ([cheri-asmflags-step1-script-stale-object](./tmp/reports/2026-10-18-cheri-asmflags-step1-script-stale-object.json) set Fix Planned). | — |
| 2026-10-18 | Spec revision (/update-plan) | Tester round 1 SUCCESS: Step 2's link-stage proof passed and the full suite is byte-identical to the reference; both prior issues confirmed fixed (Resolved by tester); the pre-existing expectation-gap deferral is logged and a new CHERI `.size` linker-warning deferral filed; no in-scope issues remain — finalize path. | — |
| 2026-10-18 | Review | Verdict "Issues found — non-blocking" (0 Critical, 0 Warning, 3 Suggestions). The two doc Suggestions were fixed in this revision (added the `ARCH=cheri_riscv64` make-line requirement, refreshed the freshness dates); report: [2026-10-18-cheri-asmflags-review-2026-10-18.json](./tmp/reports/2026-10-18-cheri-asmflags-review-2026-10-18.json). | [reviewer-suggestion-makefile-377-comment](./tmp/reports/2026-10-18-cheri-asmflags-review-2026-10-18.json) |

## Plan Metadata

```json
{
  "plan_id": "2026-10-18-cheri-asmflags",
  "steps": ["Step 1: Add the triple to the cheri CXX/OBJECT_CXX flags and the missing ASMFLAGS line", "Step 2: Link-stage verification of the two asmfunc units with caller-supplied toolchain paths"],
  "coder_files": ["./Makefile"],
  "tester_files": ["./tmp/verification/2026-10-18-cheri-asmflags-linkcheck.sh"],
  "doc_files": ["./doc/porting.html"],
  "verification_scripts": ["./tmp/verification/2026-10-18-cheri-asmflags-step1.py"],
  "repo_mode": "Public",
  "document_overrides": []
}
```

## Audit Accumulation

*Counters reset when mega-audit runs. Used by Phase 5 entry trigger.*

| Counter | Value | Last Updated |
|---------|-------|--------------|
| issues_resolved_since_audit | 2 | 2026-10-18 |
| steps_changed_since_audit | 0 | 2026-10-18 |
| files_changed_since_audit | 0 | 2026-10-18 |

## Documentation

Documentation owned by the planner: `./doc/porting.html` (switch-flag list — see Guidance for Planner). No `README.md` or `CLAUDE.md` change: no user-facing command, record or result code moves.

## Final Results

**Status:** COMPLETED
**Completed:** 2026-10-18

### What was implemented

- `./Makefile` — the `enable_riscv64_cheri` block now owns the assembler target on every consumer path: `--target=riscv64-unknown-freebsd` prepended to `CXXFLAGS` and `OBJECT_CXXFLAGS`; a new `ASMFLAGS += --target=riscv64-unknown-freebsd -march=rv64gcxcheri -mabi=l64pc128d`; a comment mirroring the Morello block. `SCHEDULER_CXXFLAGS` is unchanged (the scheduler stays a host binary).
- `./doc/porting.html` — the switch list now shows the triple for `enable_riscv64_cheri`; the caller-requirements paragraph states that both CHERI blocks carry their own triple, that the purecap sysroot is passed through `CXX=`, and that the riscv64 CHERI build also passes `ARCH=cheri_riscv64` on the make line (the documented `Makefile:251` quirk).

### Verification

- **Coder round 1** (2026-10-18): `build_result: pass`. Step 1 (auto) 3/3; (scripted) PASS — the SDK assembles `lib/cheri_riscv64/indepassembly.o` as a RISC-V FreeBSD ELF defining `assembly_helper` and `assembly_return_site`.
- **Tester round 1** (2026-10-18): `overall_result: SUCCESS` (4/4). Step 2 link-stage proof with caller-supplied toolchain paths only (no `--target`/`-march`/`-mabi` in `CXX`/`ASM`): both asmfunc units link, `readelf -h` reports RISC-V, both symbols defined; cheri-aware clean leaves no artifact. Full suite `./run-test exhausted-run`: 246/246 records, 0 skipped, 0 failed to build, result codes identical to the 2026-10-13 x86_64 reference (0 differences). `script/check_catalog.py`: 246/246 records and 103/103 programs match `configure.json`.
- **Reviewer** (2026-10-18, sonnet): "Issues found — non-blocking"; 0 Critical, 0 Warning, 3 Suggestions (2 doc Suggestions fixed; 1 comment nit dismissed). Doc-structure gate: PASS.

### Issues

- **Resolved:** `cheri-asm-target-flags` (the plan's issue — the flag block is complete), `cheri-asmflags-step1-script-stale-object` (verification-script cleanup hardened and confirmed).
- **Deferred (this repo):** `cheri-asm-size-directives` — CHERI linker warning on the unsized capability relocation for `assembly_return_site` (`lib/cheri_riscv64/indepassembly.asm` has no `.size` directives); `lib/` was out of this plan's scope.
- **Deferred (cross-repo):** `review-lines-bench-expectation-gaps` — three records' result codes fall outside their `expect-results`; fix planned in `2026-10-14-deferred-issue-sweep` Step 4 (`target_repo: comparch-security/cpu-sec-bench`).

### Known caveats

- The `Makefile:251` `$(ARCH)` evaluation quirk is untouched (out of scope); `doc/porting.html` now documents the `ARCH=cheri_riscv64` make-line requirement it imposes.
- Reviewer hunches (non-defects today): the cheri/morello blocks' flags leak into the MSVC path on Windows (pre-existing for the C++ lines too); `ASMFLAGS` omits `-mno-relax` (assembled object byte-identical for the current asm source); the env's guest scheduler keeps its caller-supplied triple because `SCHEDULER_CXXFLAGS` is host-only by design.
- **Handoff:** the fix commit must be pushed to `origin/master` before the env plan `2026-10-18-drop-indepassembly-workarounds` can pin `BENCH_COMMIT` to it and re-run every non-x86 lane.
