# Plan: Scheduler: explicit skip, defined build-failure status, and a supported independent-assembly opt-out
**Project:** /home/wsong83/proj/xiangshan/cpu-sec-test-env/benchmark
**Plan ID:** 2026-10-12-scheduler-skip-and-build-status
**Created:** 2026-10-12

## Issue Log

Rows ordered newest-first (latest issues at top). New issues are prepended; resolved issues stay in place with updated status.

| Issue | Status | Found | Resolved | Resolved By |
|-------|--------|-------|----------|-------------|
| `2026-10-12-scheduler-skip-and-build-status-peer-vfunc-missing-cstdint` — `cfi/return-to-peer-vfunc.cpp` uses `uintptr_t` without `#include <cstdint>`; g++ 15.2 rejects it and the native full-suite run records the record 1025 (failed to build). Pre-existing, root cause `6a7b00c` (2024-11-12). [report](./tmp/reports/2026-10-12-scheduler-skip-and-build-status-peer-vfunc-missing-cstdint.json) | Resolved | 2026-10-12 | 2026-10-12 | tester |

This plan is new work on a drafted issue; it resolves no existing `defer-issue-*.json` and opens none. The pre-existing `report_gen` hang found during planning is fixed in-plan by Step 3 (Summary carries the evidence) rather than deferred, and the pre-existing compile defect the iteration-3 acceptance run surfaced is fixed in-plan by Step 8 (user disposition, 2026-10-12 — coder work, since the test-case sources are this repo's product code).

## Guidance for Coder

**Files to modify:** `scheduler/run-test.cpp`, `Makefile`

Both are outside the planner's Write/Edit allowlist, which is why they are coder work. Five steps, two files.

**Do not** fix the 8 pre-existing `run-test.cpp` compiler warnings or the dead `has_make` variable (deferred issue `run-test-compiler-warnings`), touch the `continue` path at line 115 (deferred issue `run-test-continue-variables`), or edit any `lib/*/indepassembly.asm` (deferred issue `indepassembly-return-site-untranslated`). The new code must compile without adding warnings, and the existing 8 must remain exactly 8.

1. **Step 1: Scheduler: per-record `skip` mechanism** (coder) — the registry gains two optional fields: `skip`, an array of `ARCH` values naming the platforms the record is not runnable on, and `skip-reason`, a free-text string. A record is skipped when its `skip` list contains the arch the scheduler was built for.
   - A file-scope constant `static const int not_run_code = 1025;` beside the other globals (after `bool trace_run = false;`), with a one-line comment: the scheduler-owned result meaning the case was not run (skipped for this platform, or could not be built).
   - An arch fallback next to the `RUN_PREFIX` block: `#ifndef CSB_TARGET_ARCH` → `#define CSB_TARGET_ARCH ""`. A scheduler built by hand (no Makefile define) has no arch, so every `skip` list is inert.
   - A helper `bool case_skipped(nlohmann::ordered_json tcase, std::string& reason)`: returns false when the record has no `skip` field; otherwise true when any entry of the array equals `CSB_TARGET_ARCH` while the macro is non-empty; fills `reason` from `skip-reason` when present, empty string otherwise. Forward-declare it beside `requires_check` (line 68). Match the file's existing idiom (`requires_check` also takes `nlohmann::ordered_json` by value).
   - In `run_tests`, immediately after `auto tcase = config_db[cn];` (line 523) and **before** `arg_parser` (line 526) and `requires_check` (line 530): when skipped, print `Skipped case: <cn> is not runnable on <CSB_TARGET_ARCH>` (append `: <reason>` when a reason exists) to `std::cerr`, record `result_db[cn]` as `{result: not_run_code, make-time: 0, run-time: 0, file-size: 0}`, `dump_json(result_db, "results.json", debug_run)`, and `continue`.
   Placement before `requires_check` is deliberate: a skipped record must not be requeued on account of unsatisfied prerequisites, and it must be recorded even under `make-only`.
   → coder verify (auto): `case_skipped(` is defined in `scheduler/run-test.cpp` and takes `(nlohmann::ordered_json, std::string&)`; `not_run_code` is defined once as `1025`; the `#ifndef CSB_TARGET_ARCH` fallback exists; in `run_tests` the `case_skipped` call site precedes both `arg_parser(tcase` and `requires_check(tcase)`; the skip branch sets the three zero fields and `continue`s
   → coder verify (scripted): `./tmp/verification/2026-10-12-scheduler-skip-and-build-status-step1.py`
   → tester verify: a skipped record whose `program` is shared with a non-skipped record — the sibling still builds and runs (skip is per-record, not per-program); a skipped record records 1025 under `make-only` while `test/<program>` is never created

2. **Step 2: Scheduler: build failures record the not-run code** (coder) — in the final result selection (lines 667–679), a non-empty `make_results` must record `not_run_code`, never the raw make status. The raw status moves to `result_db[cn]["make-status"]` (its only structured home besides the log), and `result_db[cn]["file-size"]` is defaulted to 0 when the case was never built — one `if(!result_db[cn].count("file-size")) result_db[cn]["file-size"] = 0;`. The stderr line at 669 currently prints `test_result`; it must print the **raw** status instead, so the log never presents 1025 as if it were make's code:
   ```cpp
   if (!make_results.empty()) {  // not successfully built
     test_result = not_run_code;
     result_db[cn]["make-status"] = *make_results.cbegin();
     if(!result_db[cn].count("file-size")) result_db[cn]["file-size"] = 0;
     std::cerr << "Built failed, the first try error code::" << *make_results.cbegin() << std::endl;
   }
   ```
   The per-attempt line at 575–576 (`std::cout << "fail to make … with error status N"`, note: **stdout**, not stderr) is unchanged. While in this block, also guard the pre-existing empty-set dereference it contains: the trailing `if (!test_result) { test_result = *test_results.cbegin(); }` dereferences `cbegin()` on an **empty** set whenever a case was successfully built but never run (the `make-only` path — the plan's own scripts take it), which is undefined behaviour; take the branch only when `!test_results.empty()`. The effective `make-only` result is unchanged (0).
   → coder verify (auto): in the `!make_results.empty()` branch, `test_result` is assigned `not_run_code`, `result_db[cn]["make-status"]` receives `*make_results.cbegin()`, and the 669 stderr line prints `*make_results.cbegin()`; `test_result = *test_results.cbegin()` is reachable only under `!test_results.empty()`; `grep -n 'make_results.cbegin()' scheduler/run-test.cpp` shows it is not assigned to `test_result` anywhere
   → coder verify (scripted): `./tmp/verification/2026-10-12-scheduler-skip-and-build-status-step2.py`
   → tester verify: a record with `conf-macro` variants (several argument indices, one binary per macro set) still ends 1025 after its first failed build, with `make-status` present; a successful build followed by a later-index build failure does not overwrite a recorded `file-size` with 0; a build-failed run's stderr shows make's own status after `error code::`, never 1025

3. **Step 3: Scheduler: not-run counts in the report, and a terminating guard for the sys-info header** (coder) — two changes in `report_gen`:
   (a) After the per-case loop and before the `Compilation time:` footer, count the records whose `result` is `not_run_code`, split by whether `make-status` is present (present ⇒ build failure, absent ⇒ skip), and write exactly these two lines:
   ```
   # not run (skipped for this platform): N
   # not run (failed to build): M
   ```
   Both are `#`-prefixed on purpose: `script/gen_sum.py` skips `#`-leading lines, so the existing consumer parses the report unchanged (verified by the step-3 script). Emit both lines even when the counts are 0 (fixed shape).
   (b) Guard the header copy — **all three failure modes**, not just the unopenable file. The 2022 loop `while(!sys_info_file.eof())` never terminates when the stream fails without setting `eofbit`: an unopenable file (failbit at open, `eofbit` never set), a readable file containing a **≥255-character line** (`getline(buf, 256)` extracts 255 chars and sets failbit with `eofbit` clear; every subsequent `getline` is a sentry no-op and `eof()` stays false), and a mid-read I/O error (badbit). The last is reachable from the Makefile's own header recipe: `make -pn enable_full_address_sanitizer=yes enable_full_undefined_sanitizer=yes` yields a 276-character `OBJECT_CXXFLAGS = …` line. Each mode writes the uninitialized `buf` forever — observed: a 900 s run left a 7,134,961,896-byte `results.dat`. Emit the pinned placeholder when the file cannot be opened, and make the copy loop read-success-based while keeping healthy output byte-identical:
   ```cpp
   std::ifstream sys_info_file("test/sys_info.txt");
   char buf[256];
   if(!sys_info_file.good()) {
     report_file << "# test/sys_info.txt missing or unreadable: no system information recorded" << std::endl;
     std::cerr << "Warning: test/sys_info.txt missing or unreadable: no system information recorded" << std::endl;
   } else {
     while(!sys_info_file.eof()) {           // condition unchanged
       sys_info_file.getline(buf, 256);
       if(sys_info_file.fail() && !sys_info_file.eof()) break;   // long line or mid-read failure
       report_file << "# " << buf << std::endl;
     }
     sys_info_file.close();
   }
   ```
   The break predicate is verified termination-equivalent on the healthy path: a short-line file's final `getline` sets failbit **together with** eofbit (probe: fail=1 eof=1), so the break never fires and the output — including the historical trailing `"# "` line — is byte-identical; a ≥255-char line sets failbit *without* eofbit, so it breaks.
   → coder verify (auto): `report_gen` contains both pinned count-line strings and the pinned placeholder string; the `while(!sys_info_file.eof())` loop is reachable only through `if(!sys_info_file.good()) … else`; the loop body contains the `if(sys_info_file.fail() && !sys_info_file.eof()) break;` guard before the write; `make -B run-test 2>&1 | grep -c 'warning:'` still reports exactly the 8 pre-existing warnings
   → coder verify (scripted): `./tmp/verification/2026-10-12-scheduler-skip-and-build-status-step3.py` (asserts the unopenable case, the ≥255-char-line case, and a small report in both)
   → tester verify: a healthy run (with `test/sys_info.txt` present and short-lined) contains **no** placeholder line and its header matches the pre-change output line for line; a run without the header terminates within seconds and its `results.dat` is small; a header containing a ≥255-character line also terminates promptly

4. **Step 4: Makefile: `disable_independent_assembly` opt-out knob** (coder) — POSIX-only, three edits in `Makefile`:
   - the flag menu (after the `# specific hardware secrutiy features` block, before `# define paths and objects`) gains a small group:
     ```
     # build plumbing

     #disable_independent_assembly      = yes
     ```
   - after the POSIX line `independent_assembly := $(addprefix lib/$(ARCH)/, indepassembly.o)` (line 240), add:
     ```
       # A port whose independent-assembly object cannot be built yet may omit it.
       # The two programs that call the object's symbols (cfi-return-to-peer-asmfunc,
       # cfi-return-to-parent-non-call-site-by-asmfunc) then fail to link.
       ifdef disable_independent_assembly
         independent_assembly :=
       endif
     ```
   - the Windows branch (`lib/x86_64/visualcpp_indepassembly_func.obj`) is untouched. Inside the knob's `ifdef`, also keep `make clean` complete: add `rubbish += $(addprefix lib/$(ARCH)/, indepassembly.o)` so an object left by an earlier knob-off build is still removed (the common `rubbish += $(independent_assembly)` line further down expands an empty value under the knob).
   → coder verify (auto): `grep -nE '^#disable_independent_assembly\s*=\s*yes$' Makefile` matches; the `ifdef disable_independent_assembly` block sits after the POSIX `independent_assembly :=` line, inside the POSIX branch, and contains the `rubbish +=` line; `make -Bn test/mss-read-cross-page-by-index-in-stack disable_independent_assembly=yes` prints a link line with no `indepassembly.o`; the default dry-run still has it
   → coder verify (scripted): `./tmp/verification/2026-10-12-scheduler-skip-and-build-status-step4.py`
   → tester verify: with the knob and an intact `.asm`, an unrelated case builds and `make clean` still exits 0 and removes `lib/<arch>/indepassembly.o` (build it once without the knob first); without the knob the same case builds with the object linked

5. **Step 5: Makefile: pass `$(ARCH)` to the scheduler as `CSB_TARGET_ARCH`** (coder) — append the define **once, after the feature-forcing blocks** (after the `enable_aarch64_bti` block, which ends at line 381), mirroring the cheri-specific `SCHEDULER_CXXFLAGS += -mno-relax` precedent at line 358:
   ```make
   SCHEDULER_CXXFLAGS += -DCSB_TARGET_ARCH="\"$(ARCH)\""
   ```
   Placement is load-bearing: the base `SCHEDULER_CXXFLAGS` definitions (POSIX line 220, Windows line 92) are simply-expanded `:=` assignments, so a define baked into them captures `ARCH` **before** `enable_riscv64_cheri` (line 356, `ARCH := cheri_riscv64`) and `enable_aarch64_morello` (line 363, `ARCH := aarch64`) reassign it. On an x86_64 host a CHERI build would then report `x86_64`: a skip list naming `cheri_riscv64` would be silently inert while an `x86_64` skip would wrongly fire. After the feature blocks, `$(ARCH)` is final in every flow — host default, command-line override, and both feature-forced values. (`cl` accepts `-D`, so the single append serves the Windows branch too.)
   → coder verify (auto): `grep -c 'CSB_TARGET_ARCH' Makefile` returns 1 and that line sits after the `enable_aarch64_bti` block; `make -Bn run-test` prints `DCSB_TARGET_ARCH=…x86_64`; `make -Bn run-test ARCH=riscv64` prints `…riscv64`; `make -Bn run-test enable_riscv64_cheri=yes` prints `…cheri_riscv64`
   → coder verify (scripted): `./tmp/verification/2026-10-12-scheduler-skip-and-build-status-step5.py` (asserts the default and the cheri dry-runs)
   → tester verify: `make -Bn run-test enable_aarch64_morello=yes` prints `DCSB_TARGET_ARCH=…aarch64` (the forced value, not the host)

6. **Step 8: fix the pre-existing compile defect the acceptance run surfaced** (coder) — `cfi/return-to-peer-vfunc.cpp` uses `uintptr_t` (lines 35–36) without including `<cstdint>`; g++ 15.2.0 rejects it (`'uintptr_t' was not declared in this scope`), so `make -B test/cfi-return-to-peer-vfunc` exits 2 and the record is honestly recorded `1025` + `make-status: 2`. The file has carried this since 2024-11-12 (`6a7b00c`) and is untouched by this plan — it is the **only one of six `uintptr_t` users** that lacks the include (`acc/check-ASLR.cpp`, `acc/read-GOT.cpp`, `mts/double-free.cpp`, `cfi/jump-func-ra-from-heap-memcpy-implicit-arith.cpp`, `cfi/jump-func-ra-from-heap-memcpy-explicit-arith.cpp` all include `<cstdint>` themselves). Fix: add `#include <cstdint>` to the include block (after `<cstring>`, line 2), matching that convention. The sibling `cfi/return-to-peer-mfunc.cpp` shares the include block but never uses `uintptr_t` and compiles — no second fix.
   → coder verify (auto): `make -B test/cfi-return-to-peer-vfunc` exits 0; `grep -c 'include <cstdint>' cfi/return-to-peer-vfunc.cpp` returns 1; the 8 pre-existing `run-test.cpp` warnings remain exactly 8
   → tester verify: the unchanged `full-suite-native.py` exits 0 — zero 1025 records, both count lines `0`/`0`, `gen_sum.py` totalling 246

## Guidance for Tester

The repo has no automated test suite, no test runner and no CI; introducing one stays out of scope. Verification is by the planner-authored scripts under `./tmp/verification/` plus the temporary tests below.

**Tests to create:**
- **Permanent:** none — there is no test framework in this repo; standing one up is a separate plan. This is a deliberate decision, not an oversight.
- **Temporary:** mark all of the following as `(temporary)` and delete them once the plan is complete — the paths stay in this plan's metadata as the record of what was verified:
  - `./tmp/verification/2026-10-12-scheduler-skip-and-build-status-skip-per-record.py` — a skipped record whose `program` is shared with a non-skipped record: the sibling still builds and runs; skip does not leak to the program.
  - `./tmp/verification/2026-10-12-scheduler-skip-and-build-status-skip-make-only.py` — under `make-only`, a skipped record records 1025 with zero timings and `test/<program>` is never created.
  - `./tmp/verification/2026-10-12-scheduler-skip-and-build-status-not-run-negative-control.py` — the same scratch registry with the `skip` fields removed builds and runs the record normally: the 1025 came from the declaration, not from a broken build.
  - `./tmp/verification/2026-10-12-scheduler-skip-and-build-status-full-suite-native.py` — on x86_64, unmodified sources, the real 246-record registry: `make && ./run-test exhausted-run` completes; `results.dat` carries both count lines with `0`/`0`; **no** record reads 1025; `script/gen_sum.py` totals 246 case lines. This is the strongest regression evidence and is expected to take minutes.
  - `./tmp/verification/2026-10-12-scheduler-skip-and-build-status-makefile-clean-regression.py` — `make clean` exits 0 with and without the knob (removing `lib/<arch>/indepassembly.o` in both cases); a fresh `make` recreates `test/sys_info.txt`; the pre-existing 8 `run-test.cpp` warnings remain exactly 8 (no new ones).
  - `./tmp/verification/2026-10-12-scheduler-skip-and-build-status-missing-sysinfo-no-hang.py` — with `test/sys_info.txt` removed, a `fast-run` terminates promptly, `results.dat` stays small, and carries the placeholder line; a header containing a ≥255-character line also terminates promptly; negative control: with a short-lined header present, the placeholder is absent.

**Verification scripts to create:**
- The six paths listed under **Tests to create → Temporary** above. The planner-authored step scripts (`./tmp/verification/2026-10-12-scheduler-skip-and-build-status-step1.py` … `step6.py`) are **re-run, not re-created**: run all six first — they are the plan's primary evidence and each is already proven red against the pre-implementation tree.

**Ownership and retention.** The planner authors, owns and fixes every script in `./tmp/verification/`; the coder executes Steps 1–5's scripts and the tester executes all twelve, each reporting a failure back to the planner rather than editing the script. The files are temporary (deleted at completion); the paths above remain the plan's durable record of what was verified.

<!-- UPDATED: 2026-10-12 — three tester sessions ended (exit 0) before writing any
     report, though their checks had largely run; report-first ordering is the
     mitigation, per the user's iteration-2 direction. -->
**Round hygiene — write the report first (iteration-2 revision).** Three tester sessions on 2026-10-12 ended mid-work with no report; the missing artifact — not the missing work — is what blocks this plan. So write the session report to `./tmp/reports/` **early**, as a skeleton listing the checks and their status, and update it after each check completes (the report's own schema governs its fields). Order the work so decisive evidence lands first: the six planner step scripts, then `skip-per-record`, `skip-make-only`, `not-run-negative-control` and `missing-sysinfo-no-hang`, then `makefile-clean-regression`, and `full-suite-native` last as the longest. A session that ends early then still leaves a usable verdict rather than nothing.

<!-- UPDATED: 2026-10-12 — iteration 3 addendum: the long check must keep the turn alive. -->
**Iteration-3 addendum — the long check must keep the turn alive.** After the iteration-2 relaunch, the round's report records **sixteen of seventeen checks green** and `full-suite-native` as the only outstanding item; the session ended within ~30 s of handing that run to a background task — the third time that signature has ended a round. In headless mode the round is over the moment a turn ends, and a background task is not work the session waits for. So run the full-suite check so that **no turn ends while it is in flight**: split it into bounded foreground steps adapted from your script (build first, then the run, then the assertions, each staying under the tool's cap), write the report's final verdict before ending the turn, and only then re-run `make clean`. Run that check **first** this round — the other sixteen are already recorded green in the iteration-2 report — and re-run as much of the rest as the budget comfortably allows.

**Tests to investigate for retirement:**
- No test obsolescence identified. The repo contains no test suite, so nothing is tied to the code this plan changes.

**Retirement procedure:** not applicable this round.

## Summary

`cpu-sec-bench`'s scheduler (`scheduler/run-test.cpp`) records the **raw make exit status** as a case's result when its build fails (line 667–669, typically `2`), indistinguishable in `results.json`/`results.dat` from a genuine test outcome, and it has no way to declare that a case is deliberately not runnable on a platform. Both gaps block multi-arch bring-up (the drafted issue `2026-10-12-bench-skip-and-build-status.md`): on riscv64/aarch64/cheri_riscv64 the independent-assembly object `lib/<ARCH>/indepassembly.asm` still carries untranslated x86 bodies (deferred issue `indepassembly-return-site-untranslated`), so nothing links, and a `./run-test exhausted-run` today cannot tell "this case measured nothing there" apart from "this case genuinely returned 2".

**Design (confirmed with the user 2026-10-12).**
- **One new scheduler-owned result code, `1025`** — *not run: skipped for this platform, or could not be built*. Build failures keep the raw make status in a results.json sub-field `make-status` (and the log); skip has no status. Dependents of a 1025 record keep gating to `1024` with no special-casing (under `fast-run`; `exhausted-run` disables the prerequisite gates by design, so dependents run there): 1025 is neither `0` nor an `expect-results` code, which `case_parser`'s gate already treats as failing. `1025` is unused anywhere in the tree (probed; `expect-results` codes in use top out at `290`).
- **Skip declaration in the registry** — `"skip": ["riscv64","aarch64","cheri_riscv64"]` plus `"skip-reason": "…"`, matched against a new `CSB_TARGET_ARCH` define the Makefile passes from `$(ARCH)` **after the feature-forcing blocks** (a single `SCHEDULER_CXXFLAGS +=` after the `enable_aarch64_bti` block). Placement matters: the existing `:=` definitions expand before `enable_riscv64_cheri`/`enable_aarch64_morello` reassign `ARCH`, so a define baked into them would carry the *host* arch and a forced-port build (e.g. CHERI on an x86_64 host) would report `x86_64` — the very flow whose skip lists name `cheri_riscv64`. A hand-built scheduler has no arch and therefore no skips. With the late append, `enable_aarch64_morello` (forced `aarch64`) and `enable_riscv64_cheri` (forced `cheri_riscv64`) both report the arch they actually build.
- **No skip declarations ship in this plan's `configure.json`** — today's three affected arches are handled by the opt-out plus build-failure auto-recording (exactly what the env-side interim lane assumes), and a declaration would go stale the moment the `indepassembly` fix lands. The mechanism ships documented and tested through scratch registries.
- **Make knob `disable_independent_assembly=yes`** — documented in the menu, POSIX-only, empties `independent_assembly`; probed: the ad-hoc form `independent_assembly=` already works, and `MAKEFLAGS='disable_independent_assembly=yes'` reaches the scheduler's own `make -B` invocations with command-line strength (probed, and asserted by the step-2/3 scripts) — the route a scheduler-driven run on a not-ready port needs. Direct builds use `make disable_independent_assembly=yes`.
- **Correction to the draft's premise, verified:** **two** programs reference the object's symbols, not one — `cfi/return-to-peer-asmfunc.cpp` (takes `assembly_return_site`'s address) and `cfi/return-to-parent-non-call-site-by-asmfunc.cpp` (calls `assembly_helper`). With the knob and a broken `.asm`, a sampled acc case and a cpi case (through `libcfi.so`) both link; both dependents fail with their symbol named. Without the knob, a broken `.asm` fails **every** build, since `$(independent_assembly)` is a prerequisite of all five link families.
- **Report** — `results.dat` gains the two `#`-prefixed count lines above (skip and build-failed counted separately), keeping `gen_sum.py` compatible.

**Pre-existing defect found during planning, fixed in-plan by Step 3(b).** Red-proving the step-5 script stumbled into `report_gen`'s header copy: `while(!sys_info_file.eof())` (line 186, from `1e0c2fa3`, 2022-09-04) never terminates on an unopenable `test/sys_info.txt` and writes the uninitialized `buf` forever — observed: a 900 s run left a **7,134,961,896-byte** `results.dat`; a 30 s cap wrote 218 MB. `make` normally creates the file as a prerequisite of `run-test`, so the reachable path is a deploy that carries the scheduler binary without the header — exactly the partial-deploy shape the all-arch env work plans. The user chose to fold the 3-line guard into this plan (healthy output byte-identical) rather than defer it; the step-3 script asserts termination, the placeholder line, and report size.

**Alternatives considered and rejected**
- *A scheduler CLI `--skip <prog>`* — no reason text, no per-platform declaration, and every invocation must remember the flags; the registry field keeps the suite self-describing (the reason the draft's own requirement asks for a recorded code with a reason).
- *A separate per-arch skip file (`skip-<ARCH>.json`)* — a new file convention for what one optional record field expresses.
- *`-2` as the build-failure code (the draft's first suggestion)* — `run_cmd()` already returns `-2` as an internal "should not run here" sentinel (line 486), and negatives are `-1`/config-error territory in the documented contract.
- *Two codes (`1025` skipped / `1026` build-failed)* — the draft asks for one dedicated code; the split lives in the report's two count lines and `make-status`, keeping the fixed contract one number smaller.
- *Abandoning a case's remaining argument indices after its first build failure* — the current loop re-attempts the same failing build once per argument index (32 failed makes in the opt-out path for the two dependents); harmless, and trimming it would change behaviour for records whose per-index macros differ.
- *Shipping skip declarations for the two asmfunc records now* — stale-skip hazard; the build-failure code already tells the truth ("tried and broken" rather than "declared inapplicable").

**Explicitly out of scope**
- The `lib/*/indepassembly.asm` translation itself (deferred issue `indepassembly-return-site-untranslated`, fix text ready; its own plan).
- The two `run-test.cpp` deferred issues (`run-test-continue-variables`, `run-test-compiler-warnings`); the new code adds no warnings and verification avoids `continue`.
- The env-side lanes (deploy, emulators, guests) — that is `cpu-sec-test-env`'s own drafted issue.
- `INTERNAL.md` (outside the planner's Write/Edit allowlist).
- Staging or committing anything; `/update-and-commit` owns staging, the `plans/` archive and `plans/content.json`.

## Repo Mode

Public

*Detected at plan creation. Determines: plan archival path, commit-message content, staged files.*

## Document Overrides

| Document | Rule Overridden | Reason |
|----------|----------------|--------|
| `CLAUDE.md` | Rules: the exit-code contract's "never invent one" | The plan adds exactly one scheduler-owned code, `1025`, following the `1024` precedent; the same plan records it in `README.md` and `doc/architecture.html`, so "the full contract is in doc/architecture.html" stays true. |
| `CLAUDE.md` | Subagents: "No project-specific overrides" — and, behind it, the base doctrine's "the coder never touches tests" | **User direction, 2026-10-12.** This repo has no `src/` tree, so the coder/tester lane boundary is ambiguous here: the benchmark's product *is* its test-case sources. A lane-boundary clarification was added to `CLAUDE.md`'s Subagents section — the test-case sources under `acc/` `cfi/` `cpi/` `mss/` `mts/`, plus `lib/`, `scheduler/`, `script/` and `configure.json`, are product code (coder work), while the verification/acceptance scripts under `./tmp/verification/` are tester work. That clarification is what makes Step 8 a coder step rather than a test-file edit. |

The single override above is deliberate and was surfaced by the pre-implementation mega-audit: `CLAUDE.md`'s literal wording forbids inventing codes, while extending the contract with a documented scheduler-owned code is exactly what `1024` did. Every other documented rule stays in force — in particular `CLAUDE.md`'s "after changing the result-code contract or `configure.json`'s field set, update both `README.md`'s 'Test results' table and `doc/architecture.html`" instruction is followed in Step 6.

## Risks

| Risk | Impact | Mitigation |
|------|--------|-----------|
| The exit-code contract gains a number | Downstream consumers of `results.dat` could misread `1025` | `gen_sum.py` verified unchanged by the step-3 script (its `#`-skipping already covers the new lines); `1025` never enters any `expect-results`; README and `doc/architecture.html` updated in the same plan |
| The opt-out's MAKEFLAGS route is a GNU-make behaviour, not the repo's existing idiom | A user could invoke it in a way that silently does not reach the scheduler's internal `make` | Probed before planning (variable assignments in `MAKEFLAGS` act with command-line strength, unlike plain environment variables, which the makefile's `:=` beats); the menu-line/`make … =yes` routes work regardless; `doc/porting.html` documents all three, and the step-2/3 scripts assert the MAKEFLAGS route end-to-end |
| A skip declaration could outlive its reason | A record stays skipped after it becomes runnable (e.g. after the `indepassembly` fix) | No declarations ship in this plan; the mechanism is exercised only through scratch registries; `skip-reason` is mandatory by documentation and surfaced in the run output |
| Step 3(b) changes 4-year-old report behaviour | A header-copy regression could alter every report | The healthy path is kept byte-identical (`while(!sys_info_file.eof())` loop untouched); the guard fires only when the stream is not `good()`; the tester verifies a healthy run contains no placeholder line |
| `./run-test continue` is unusable (deferred `run-test-continue-variables`) | A `continue`-based verification would mislead | Every check uses fresh runs; no step depends on `continue` |
| The Windows/`visualcpp` path is untested here (no MSVC) | The knob is POSIX-only and the Windows arch define is unexercised | The knob leaves the Windows branch untouched; the define is inert unless a record declares `"skip": ["x86_64"]`; both facts are documented |
| The two dependent programs cannot build whenever the opt-out is used | They are recorded `1025` (build-failed) rather than measured | Intended and honest — the deferred `indepassembly` fix removes the cause; the acceptance expects exactly this |

## Proposed Changes

Each step includes verification checks tagged with confidence (see Prime Directive 4).
`(auto)` = mechanical, coder self-verifies. `(scripted)` = deterministic multi-step, coder runs provided script.
Every step must have at least one verification check executable by its owner (coder steps: coder checks; tester steps: tester verify; planner steps: Evidence).
Steps MUST use this numbered-list format exactly. The `### Step N:` heading format is deprecated and must not be used.

The six `(scripted)` checks are **end-of-coder-round** checks, not per-step gates: each exercises the completed feature set — step1.py needs Step 5's `CSB_TARGET_ARCH` define, step2.py needs Step 4's knob, step3.py needs Steps 1–5 together — so all six go green only after Step 5 lands. A red script mid-round is expected and is not a defect in the step being worked; the `(auto)` checks are the per-step gates.

1. **Step 1: Scheduler: per-record `skip` mechanism (arch-matched, recorded as 1025)** (coder) — see Guidance for Coder. The registry gains `skip` (array of `ARCH` values) and `skip-reason` (string); a record whose `skip` contains the scheduler's `CSB_TARGET_ARCH` is neither built nor run, is recorded `{result: 1025, make-time: 0, run-time: 0, file-size: 0}`, and its reason is printed. The check sits before the prerequisite gate so a skipped record never requeues.
    → coder verify (auto): `case_skipped(nlohmann::ordered_json, std::string&)` defined; call site precedes both `arg_parser` and `requires_check`; the three zero fields set; `#ifndef CSB_TARGET_ARCH` fallback present
    → coder verify (scripted): script at `./tmp/verification/2026-10-12-scheduler-skip-and-build-status-step1.py` (proven red: the marked record built, ran, and surfaced no reason)
    → tester verify: shared-program sibling still builds and runs; `make-only` records 1025 without creating the binary; a control run without the `skip` fields builds the record normally

2. **Step 2: Scheduler: build failures record the not-run code, raw status kept in `make-status`** (coder) — see Guidance for Coder. The `!make_results.empty()` branch assigns `not_run_code`, stores the raw status in `result_db[cn]["make-status"]`, and defaults a never-built case's `file-size` to 0.
    → coder verify (auto): `test_result` is never assigned `*make_results.cbegin()`; `make-status` and the `file-size` default are present in that branch
    → coder verify (scripted): script at `./tmp/verification/2026-10-12-scheduler-skip-and-build-status-step2.py` (proven red: with the knob a no-op, both dependents built and recorded 0)
    → tester verify: multi-index `conf-macro` record still ends 1025 with `make-status`; a later-index failure does not zero an already-recorded `file-size`

3. **Step 3: Scheduler: not-run counts in the report, and a terminating guard for the sys-info header** (coder) — see Guidance for Coder. Two `#`-prefixed count lines with the pinned strings; the header copy guarded by `if(!sys_info_file.good())` (pinned placeholder on report + stderr) **and** a `fail() && !eof()` break inside the loop, so all three runaway modes terminate — unopenable file, ≥255-character line, mid-read failure — with healthy output byte-identical.
    → coder verify (auto): both count strings and the placeholder string present; the `while(!sys_info_file.eof())` loop reachable only via the `else`; `make -B run-test` still emits exactly the 8 pre-existing warnings
    → coder verify (scripted): script at `./tmp/verification/2026-10-12-scheduler-skip-and-build-status-step3.py` (proven red: no count lines, and a missing header hung the run — 218,930,190 bytes in 30 s, cleaned up by the script)
    → tester verify: healthy run has no placeholder line; missing-header run terminates promptly with a small report carrying the placeholder; `gen_sum.py` totals all records

4. **Step 4: Makefile: `disable_independent_assembly` opt-out knob** (coder) — see Guidance for Coder. Menu entry plus the POSIX `ifdef` that empties `independent_assembly`.
    → coder verify (auto): menu regex matches; the `ifdef` block is inside the POSIX branch after line 240; dry-run link lines differ with and without the knob
    → coder verify (scripted): script at `./tmp/verification/2026-10-12-scheduler-skip-and-build-status-step4.py` (proven red: no menu line, the knob did not remove the object, and a broken `.asm` failed every build)
    → tester verify: knob + intact `.asm` still builds unrelated cases and `make clean` exits 0; the two dependents fail with `assembly_return_site` / `assembly_helper` named

5. **Step 5: Makefile: pass `$(ARCH)` to the scheduler as `CSB_TARGET_ARCH`** (coder) — see Guidance for Coder. One `SCHEDULER_CXXFLAGS +=` after the feature-forcing blocks, so the define carries the **final** `ARCH` in every flow (host default, command-line override, `enable_riscv64_cheri`, `enable_aarch64_morello`) — not the pre-force capture a `:=`-line placement would produce.
    → coder verify (auto): the single `CSB_TARGET_ARCH` line sits after the `enable_aarch64_bti` block; default / `ARCH=riscv64` / `enable_riscv64_cheri=yes` dry-runs print `x86_64` / `riscv64` / `cheri_riscv64`
    → coder verify (scripted): script at `./tmp/verification/2026-10-12-scheduler-skip-and-build-status-step5.py` (proven red: no define on the compile line; its negative control — a hand-built scheduler treats skips as inert — already passes pre-change, as designed)
    → tester verify: `make -Bn run-test enable_aarch64_morello=yes` reaches `aarch64`

6. **Step 6: Documentation: README, architecture, porting, CLAUDE.md** (planner) — the four files below, written after the coder round so they describe implemented behaviour.
    → Evidence: `./tmp/verification/2026-10-12-scheduler-skip-and-build-status-step6.py` exits 0 (README row; architecture code row + count line + `skip`/`skip-reason` fields; porting knob + MAKEFLAGS + both dependent programs; CLAUDE.md 1025 + `skip-reason` + knob; `script/check_catalog.py` still 246/246 & 103/103); the `/doc-structure` gate (`python3 ~/.claude/scripts/doc_structure_check.py --mode public`) reports no violation, its PASS block quoted into the session summary; `git diff --stat -- README.md doc/ CLAUDE.md` (scoped, so uncommitted coder changes are out of the tally) shows only the four files' intended hunks
    → tester verify: `doc/architecture.html`'s code table and `README.md`'s table agree on `1025` and on every pre-existing row (no drift introduced by the edit)

7. **Step 7: Behavioural acceptance and regression** (tester) — the temporary tests in Guidance for Tester: the six scripts re-run on the finished tree, the five tester-authored checks, the full-suite native run, and cleanup of scratch artifacts.
    → tester verify: all six scripts exit 0; `full-suite-native` completes with zero 1025 records and `gen_sum.py` totalling 246; `missing-sysinfo-no-hang` and its negative control both pass; report results in the tester session report, and record any pre-existing or out-of-scope observation with `pre_existing: true` rather than as prose
8. **Step 8: fix the pre-existing compile defect the acceptance run surfaced** (coder) — see Guidance for Coder. `cfi/return-to-peer-vfunc.cpp` gains `#include <cstdint>`; the record then builds, and the unchanged `full-suite-native` acceptance goes green. Surfaced by the tester's iteration-3 run and dispositioned into this plan by the user (2026-10-12): coder work, not a deferral; the issue report is [2026-10-12-scheduler-skip-and-build-status-peer-vfunc-missing-cstdint.json](./tmp/reports/2026-10-12-scheduler-skip-and-build-status-peer-vfunc-missing-cstdint.json).
    → tester verify: the unchanged `full-suite-native.py` exits 0 — zero 1025 records, both count lines `0`/`0`, `gen_sum.py` total 246

## Guidance for Planner

Documentation tasks you will execute yourself (not the coder):

- **Files to modify:**
  - `./README.md`
  - `./doc/architecture.html`
  - `./doc/porting.html`
  - `./CLAUDE.md`
- **`.claude/mega-audit-files.json`:** no change. The three declared entries (`doc/`, `CLAUDE.md`, `README.md`) already cover this plan's planner-owned files; no new locations.
- **When:** after the coder round (the docs describe implemented behaviour and the pinned strings the scripts assert); `README.md`'s and `doc/architecture.html`'s edits are the `## Update and Commit` rule's mandatory reconciliation for a result-code/field-set change.
- **What to sync:**
  - `README.md` — "Test results" table: `* 1025: The case was not run: it is skipped on this platform, or it could not be built.`
  - `doc/architecture.html` — result-code table row for `1025`; the scheduler algorithm's step 2 ("Compute the pending list") notes that arch-skipped records are recorded 1025 and never built or run (the skip check precedes argument expansion, so the page's numbered list stays 1–7 with the note attached to step 2); the build step records `not_run_code` + `make-status` and its stderr line prints make's own status; the Record step gains the two count lines and the header guard (terminating on an unopenable header, a ≥255-character line, and a mid-read failure); the registry table gains `skip` and `skip-reason`; `<meta name="last-modified">` **and** the footer date bumped.
  - `doc/porting.html` — "What a test case links against": the opt-out knob, both link-dependent programs by name, and the three invocation routes (`make disable_independent_assembly=yes`; `MAKEFLAGS='disable_independent_assembly=yes' ./run-test …` for scheduler-driven runs; the menu line); "Adding an instruction set" mentions the knob as the bring-up interim; `<meta name="last-modified">` **and** the visible footer `Last updated` date (line 225) bumped.
  - `CLAUDE.md` — Architecture's scheduler description (skip check, 1025, counts, guard), the record-fields list (`skip`, `skip-reason`), the Commands flag paragraph (the knob), and a Rules sentence that `1025` is scheduler-owned and must never appear in `expect-results`.
- **Verification scripts to author:** the six step scripts above; each already exists and is proven red against the pre-implementation tree (evidence in the History row). The planner is their sole owner — the coder and tester execute them and report failures back; neither edits a planner-authored script (see Guidance for Tester's "Ownership and retention").
- **Workspace hygiene:** the scratch trees live under `./tmp/2026-10-12-scheduler-skip-and-build-status-scratch-step*/` (flat under `tmp/`, not under `verification/`); the repo root itself must be left clean — the plan's probes ran only in scratch copies, and `make clean` follows any in-root build (the step-5 and step-6 dry-runs build nothing).

## History

One row per event (spec revision / mega-audit / review / implementation round). Conclusion = one-sentence outcome or what changed. Dismissed = compact dismissed-findings list for the reviewer — verbatim issue slugs linked as `[<slug>](<report-path>)`, no prose-only entries (the update-and-commit cross-ref and the reviewer both match these cells verbatim). Link to report files; never restate findings inline.

| Date | Event | Conclusion | Dismissed |
|------|-------|------------|-----------|
| 2026-10-12 | Initial plan | Phase 1–3 reconnaissance verified the draft's premises and corrected two (the opt-out's dependent set is **two** programs, and `-2` collides with `run_cmd`'s internal sentinel); user confirmed all four design questions as recommended plus folding in the `report_gen` header hang found during red-proofing (7.1 GB runaway `results.dat`) as Step 3(b). Six verification scripts written and proven red. | — |
| 2026-10-12 | Mega-audit | Pre-implementation, 9 lenses: **0 High** — the report's single `severity:"High"` entry is the reality-checker's no-violation placeholder ("No reality violations found … None required"), not a finding; 8 Medium, 14 Low. All 8 Mediums fixed in this revision: the `CSB_TARGET_ARCH` define moved after the feature-forcing blocks (a `:=`-line append captures the host arch on forced ports — [medium-00-defect-tracer](./tmp/reports/2026-10-12-scheduler-skip-and-build-status-mega-audit-2026-10-12.json) [medium-14-edge-case-explorer](./tmp/reports/2026-10-12-scheduler-skip-and-build-status-mega-audit-2026-10-12.json)), the sys-info guard widened to all three runaway modes — unopenable file, ≥255-char line, mid-read failure ([medium-15-edge-case-explorer](./tmp/reports/2026-10-12-scheduler-skip-and-build-status-mega-audit-2026-10-12.json) [medium-19-security](./tmp/reports/2026-10-12-scheduler-skip-and-build-status-mega-audit-2026-10-12.json)), the metadata `tester_files` union reconciled ([medium-07-rule-enforcer](./tmp/reports/2026-10-12-scheduler-skip-and-build-status-mega-audit-2026-10-12.json)), scripts marked end-of-round checks ([medium-08-conflict-resolver](./tmp/reports/2026-10-12-scheduler-skip-and-build-status-mega-audit-2026-10-12.json)), single script ownership ([medium-21-boundary-checker](./tmp/reports/2026-10-12-scheduler-skip-and-build-status-mega-audit-2026-10-12.json)), the porting footer-date sync ([medium-22-boundary-checker](./tmp/reports/2026-10-12-scheduler-skip-and-build-status-mega-audit-2026-10-12.json)). Folded Lows: the 669-message pin and cout/cerr attribution ([low-02-defect-tracer](./tmp/reports/2026-10-12-scheduler-skip-and-build-status-mega-audit-2026-10-12.json) [low-09-conflict-resolver](./tmp/reports/2026-10-12-scheduler-skip-and-build-status-mega-audit-2026-10-12.json)), the knob's `rubbish` line ([low-03-defect-tracer](./tmp/reports/2026-10-12-scheduler-skip-and-build-status-mega-audit-2026-10-12.json)), the architecture algorithm's skip-note placement ([low-10-conflict-resolver](./tmp/reports/2026-10-12-scheduler-skip-and-build-status-mega-audit-2026-10-12.json)), the exit-code override row ([low-11-conflict-resolver](./tmp/reports/2026-10-12-scheduler-skip-and-build-status-mega-audit-2026-10-12.json)), the scoped doc diff ([low-12-conflict-resolver](./tmp/reports/2026-10-12-scheduler-skip-and-build-status-mega-audit-2026-10-12.json)), the temporary/durable script wording ([low-13-conflict-resolver](./tmp/reports/2026-10-12-scheduler-skip-and-build-status-mega-audit-2026-10-12.json)), the fast-run-scoped gating sentence ([low-16-edge-case-explorer](./tmp/reports/2026-10-12-scheduler-skip-and-build-status-mega-audit-2026-10-12.json)), and the pre-existing empty-set guard at line 679 ([low-17-edge-case-explorer](./tmp/reports/2026-10-12-scheduler-skip-and-build-status-mega-audit-2026-10-12.json)). Report: [2026-10-12-scheduler-skip-and-build-status-mega-audit-2026-10-12.json](./tmp/reports/2026-10-12-scheduler-skip-and-build-status-mega-audit-2026-10-12.json) | Low, acknowledged no-fix: [low-04-oversight-catcher](./tmp/reports/2026-10-12-scheduler-skip-and-build-status-mega-audit-2026-10-12.json) (spawn failure records raw errno — a `no-make` deployment error, out of scope), [low-05-oversight-catcher](./tmp/reports/2026-10-12-scheduler-skip-and-build-status-mega-audit-2026-10-12.json) (no rollback section — the template has none; forward compatibility held: results.json only gains a field), [low-18-edge-case-explorer](./tmp/reports/2026-10-12-scheduler-skip-and-build-status-mega-audit-2026-10-12.json) (`skip` value type unchecked in the spirit of every other registry field; the documented shape is an array), [low-20-security](./tmp/reports/2026-10-12-scheduler-skip-and-build-status-mega-audit-2026-10-12.json) (fail-open for a hand-built scheduler — documented design; the Makefile always passes the define) — plus [high-06-reality-checker](./tmp/reports/2026-10-12-scheduler-skip-and-build-status-mega-audit-2026-10-12.json), the severity-labelled clean report. |
| 2026-10-12 | Implementation round (coder) | First launch (13:14–13:29) exited 0 with no report and only Step 1 in the working tree; classified a failed round per the outcome table and relaunched exactly once under the retry rule (marker persisted, then removed on success). The retry (13:31–14:02) completed Steps 1–5 and wrote [2026-10-12-scheduler-skip-and-build-status-coder-2026-10-12.json](./tmp/reports/2026-10-12-scheduler-skip-and-build-status-coder-2026-10-12.json): build pass, the pre-existing warning set unchanged at exactly 8, zero issues filed, all five planner scripts exit 0 in-round, `make clean` run. | — |
| 2026-10-12 | Spec revision (/update-plan) | Planner Step 6 landed after the coder round: `README.md` (1025 row), `doc/architecture.html` (code row; algorithm steps 2/5/7; registry fields; dates), `doc/porting.html` (the knob, both link-dependent programs, the three invocation routes, dates), `CLAUDE.md` (rules, commands, scheduler list, record fields); `step6.py` and `check_catalog.py` both green. Then the **tester round ended twice without a report** (14:08–14:32 and its sanctioned retry 14:36–14:50; both exit 0, child-side, well short of the launcher's 1 h cap) — no tester verdict exists. Evidence instead: the retry re-ran all six planner scripts and reached its own checks; a partial root suite run (71 records) held only expected codes (56×0, 3×1, 12×16). Surfaced the repeating round failure to the user, who chose to re-enter Phase 4.6 for loop iteration 2; the spent tester retry marker is cleared so the new round starts with its own budget. | — |
| 2026-10-12 | Spec revision (/update-plan) | Loop iteration 2's tester round ended without a report as well (15:49–16:08, exit 0, mid-work, after running most of its per-step checks) — three sessions, one signature. Applied iteration 2's one relaunch under the round contract and revised **Guidance for Tester** with report-first round hygiene (write the report skeleton before the long checks and update it after each; decisive evidence ordered first), so a truncated session still leaves a verdict. | — |
| 2026-10-12 | Implementation round (tester) | The iteration-2 relaunch — the first tester session to leave an artifact — wrote the skeleton report at [2026-10-12-scheduler-skip-and-build-status-tester-2026-10-12.json](./tmp/reports/2026-10-12-scheduler-skip-and-build-status-tester-2026-10-12.json) before its checks, then recorded **16 of 17 green** (all six planner step scripts, all five tester checks, all five prior-round extras; `blocking_failures: []`) with **`full-suite-native` pending** — the session ended 31 s after backgrounding that run. The report declares itself an incomplete round, not a verdict. This round also created five tester extras (`step2-build-failure-detail`, `healthy-header-byte-identity`, `step4-knob-intact-asm`, `step5-morello-dry-run`, `step6-doc-parity`), now listed in the metadata. | — |
| 2026-10-12 | Spec revision (/update-plan) | Guidance for Tester gained the iteration-3 addendum: the full-suite check must not be backgrounded — no turn may end while it is in flight — and it runs first, since the other sixteen checks are recorded green. Metadata `tester_files` extended with the five tester extras; the tester retry marker is cleared for the new round. | — |
| 2026-10-12 | Implementation round (tester) | Iteration 3 finished properly — report written at 17:25, `make clean` and a tree-state check after it. **16 of 17 checks green**, including the full 246-record native `exhausted-run` in the foreground (inside the tool cap, no backgrounding) and every per-step acceptance bullet. The one red is `full-suite-native`'s zero-1025 expectation: `cfi-return-to-peer-vfunc` records `1025` + `make-status: 2` because the file (added 2024-11-12, `6a7b00c`) uses `uintptr_t` without `<cstdint>` — pre-existing, untouched by this plan, and surfaced honestly exactly as the new mechanism intends. The tester filed [the issue report](./tmp/reports/2026-10-12-scheduler-skip-and-build-status-peer-vfunc-missing-cstdint.json); no plan-caused regression anywhere. | — |
| 2026-10-12 | Spec revision (/update-plan) | Dispositioned the pre-existing finding on user direction: **fixed in-plan as Step 8** (the coder adds `#include <cstdint>` to `cfi/return-to-peer-vfunc.cpp`, matching the five other `uintptr_t` users; the unchanged `full-suite-native` acceptance then closes Step 7). Recorded in the Issue Log (`Fix Planned`), `Proposed Changes`, `coder_files` and `steps` metadata, and a second Document Overrides row. The user also directed a durable **repo-local lane-boundary clarification** in `CLAUDE.md`'s Subagents section: this repo has no `src/` tree, so the test-case sources are its product code and source fixes to them are coder work. | — |
| 2026-10-12 | Implementation round (coder) | Step 8 round (the plan's second coder round, report suffix `-2`): `#include <cstdint>` added to `cfi/return-to-peer-vfunc.cpp` after `<cstring>`, matching the five other `uintptr_t` users; all three auto checks green (the target builds, exit 0; the include present; `run-test.cpp` warnings still exactly 8); the sibling `cfi/return-to-peer-mfunc.cpp` confirmed to need no second fix; `make clean` run. [report](./tmp/reports/2026-10-12-scheduler-skip-and-build-status-coder-2026-10-12-2.json) | — |
| 2026-10-12 | Implementation round (tester) | The confirming round's first attempt died as a skeleton again (the session ended as the acceptance command returned, discarding a completed `results.dat`); its sanctioned relaunch ran the suite first, in the foreground, and finished in 20 minutes. **17/17 checks green, `overall_result: SUCCESS`** — `full-suite-native` passes (both count lines `0`/`0`, all 246 records present, none at 1025, `gen_sum.py` total 246), closing Step 7 and Step 8's tester verify. The tester deleted all 17 temporary scripts and the plan's scratch trees, and merged the peer-vfunc issue report to a resolution carrying `Resolved`. One recorded exception: the final `make clean` was permission-denied in that unattended session, so gitignored root artifacts remained. [report](./tmp/reports/2026-10-12-scheduler-skip-and-build-status-tester-2026-10-12-3.json) | — |
| 2026-10-12 | Spec revision (/update-plan) | Post-tester finalization: the Issue Log row `peer-vfunc-missing-cstdint` → `Resolved` (Resolved By tester, 2026-10-12); the tester's recorded `make clean` exception closed by the planner running it; no in-scope issues remain, so the plan proceeds to the Phase 6 review. | — |
| 2026-10-12 | Review | Phase 6 final review (reviewer subagent, Sonnet): verdict **"Issues found — non-blocking"** — 0 Critical, 0 Warning, 3 Suggestions; ready to commit as-is. It independently re-verified `check_catalog.py` at 246/246 and 103/103, zero `skip` declarations in `configure.json`, the single `CSB_TARGET_ARCH` define after the feature blocks (Makefile:395), and that the `1024`-gating path (`case_parser:321-343`) really does treat a `1025` prerequisite as failing. It also corrected the mega-audit's [low-18-security](./tmp/reports/2026-10-12-scheduler-skip-and-build-status-mega-audit-2026-10-12.json) probe evidence — numeric entries in a `skip` array iterate once and throw, rather than "iterate without matching" — while confirming that dismissal's rationale (no type validation, like every other registry field); recorded as the first suggestion below. [report](./tmp/reports/2026-10-12-scheduler-skip-and-build-status-review-2026-10-12.json) | Suggestions (non-blocking deferred improvements): [review-s1-skip-array-nonstring-throws](./tmp/reports/2026-10-12-scheduler-skip-and-build-status-review-2026-10-12.json) (a non-string entry in `skip` throws `nlohmann::json::type_error` and aborts the run — a docs note or an `is_string()` guard would close it), [review-s2-case-skipped-by-value-copy](./tmp/reports/2026-10-12-scheduler-skip-and-build-status-review-2026-10-12.json) (`case_skipped` takes the record by value, copying per case; `const&` would be free — the plan prescribed the by-value form to match `requires_check`'s idiom), [review-s3-report-double-iteration](./tmp/reports/2026-10-12-scheduler-skip-and-build-status-review-2026-10-12.json) (`report_gen` iterates `result_db` twice back-to-back; the count fold could live in the first loop) |

## Plan Metadata

```json
{
  "plan_id": "2026-10-12-scheduler-skip-and-build-status",
  "steps": [
    "Step 1: Scheduler: per-record skip mechanism (arch-matched, recorded as 1025)",
    "Step 2: Scheduler: build failures record the not-run code, raw status kept in make-status",
    "Step 3: Scheduler: not-run counts in the report, and a terminating guard for the sys-info header",
    "Step 4: Makefile: disable_independent_assembly opt-out knob",
    "Step 5: Makefile: pass $(ARCH) to the scheduler as CSB_TARGET_ARCH",
    "Step 6: Documentation: README, architecture, porting, CLAUDE.md",
    "Step 7: Behavioural acceptance and regression",
    "Step 8: Fix the pre-existing cfi/return-to-peer-vfunc.cpp compile defect (missing <cstdint>)"
  ],
  "coder_files": ["scheduler/run-test.cpp", "Makefile", "cfi/return-to-peer-vfunc.cpp"],
  "tester_files": [
    "./tmp/verification/2026-10-12-scheduler-skip-and-build-status-skip-per-record.py",
    "./tmp/verification/2026-10-12-scheduler-skip-and-build-status-skip-make-only.py",
    "./tmp/verification/2026-10-12-scheduler-skip-and-build-status-not-run-negative-control.py",
    "./tmp/verification/2026-10-12-scheduler-skip-and-build-status-full-suite-native.py",
    "./tmp/verification/2026-10-12-scheduler-skip-and-build-status-makefile-clean-regression.py",
    "./tmp/verification/2026-10-12-scheduler-skip-and-build-status-missing-sysinfo-no-hang.py",
    "./tmp/verification/2026-10-12-scheduler-skip-and-build-status-step2-build-failure-detail.py",
    "./tmp/verification/2026-10-12-scheduler-skip-and-build-status-healthy-header-byte-identity.py",
    "./tmp/verification/2026-10-12-scheduler-skip-and-build-status-step4-knob-intact-asm.py",
    "./tmp/verification/2026-10-12-scheduler-skip-and-build-status-step5-morello-dry-run.py",
    "./tmp/verification/2026-10-12-scheduler-skip-and-build-status-step6-doc-parity.py"
  ],
  "doc_files": ["README.md", "doc/architecture.html", "doc/porting.html", "CLAUDE.md"],
  "verification_scripts": [
    "./tmp/verification/2026-10-12-scheduler-skip-and-build-status-step1.py",
    "./tmp/verification/2026-10-12-scheduler-skip-and-build-status-step2.py",
    "./tmp/verification/2026-10-12-scheduler-skip-and-build-status-step3.py",
    "./tmp/verification/2026-10-12-scheduler-skip-and-build-status-step4.py",
    "./tmp/verification/2026-10-12-scheduler-skip-and-build-status-step5.py",
    "./tmp/verification/2026-10-12-scheduler-skip-and-build-status-step6.py"
  ],
  "repo_mode": "Public",
  "document_overrides": [
    "CLAUDE.md: Rules, the exit-code contract's \"never invent one\" (Reason: adds exactly one documented scheduler-owned code, 1025, following the 1024 precedent, recorded in README.md and doc/architecture.html in the same plan)",
    "CLAUDE.md: Subagents \"No project-specific overrides\" + the base doctrine's coder/test-file boundary (Reason: user direction 2026-10-12 — this repo has no src/ tree, so a lane-boundary clarification was added to CLAUDE.md: the test-case sources are the benchmark's product, which makes the Step 8 compile fix coder work rather than test-file editing)"
  ]
}
```

## Audit Accumulation

*Counters reset when mega-audit runs. Used by Phase 5 entry trigger.*

| Counter | Value | Last Updated |
|---------|-------|--------------|
| issues_resolved_since_audit | 0 | 2026-10-12 |
| steps_changed_since_audit | 0 | 2026-10-12 |
| files_changed_since_audit | 0 | 2026-10-12 |

## Documentation

| File | Audience | Keep in sync with | Notes |
|------|----------|-------------------|-------|
| `CLAUDE.md` | Claude (planner/coder/tester) | Structure, Commands, Architecture sections | Edited (Step 6): scheduler description, record fields, the flag menu mention, the 1025 rule. |
| `README.md` | Users | **Result-code table** — must stay in step with `doc/architecture.html` | Edited (Step 6): the `1025` row. |
| `doc/architecture.html` | Users, maintainers | **`configure.json` field set, `run-test.cpp` algorithm, result-code table** | Edited (Step 6): 1025 row, skip check, not-run recording, count lines, header guard, `skip`/`skip-reason` rows. |
| `doc/porting.html` | Porters, maintainers | **`lib/` layout, Makefile `ARCH`/`CLIBAPI` selection, link inputs** | Edited (Step 6): the opt-out knob and its invocation routes. |
| `doc/content.html`, `doc/test-catalog.html`, `doc/style.css` | Users | — / `configure.json` (catalog) | Unchanged: no record names or counts change; `script/check_catalog.py` re-run as a regression. |
| `INTERNAL.md` | Maintainers | — | Drifted rationale narrative; outside the planner's Write allowlist — declared no-action. |

## Final Results

**Completion date:** 2026-10-12 — **Status:** COMPLETED

**What shipped.** The three gaps that blocked multi-arch bring-up are closed in the scheduler and the build system, plus one pre-existing defect the acceptance run surfaced:

| Step | Deliverable | Verified by |
|------|-------------|-------------|
| 1 | Per-record `skip` / `skip-reason` registry fields; a record whose `skip` names the scheduler's arch is recorded `1025` and never built or run (the check precedes argument expansion and prerequisite gating) | `step1.py`; tester checks |
| 2 | Build failures record `1025`, never the raw make status; the raw status lives in `results.json`'s `make-status`, a never-built case defaults `file-size` 0, and the log prints make's own status | `step2.py`; `step2-build-failure-detail` |
| 3 | `results.dat` gains `# not run (skipped for this platform): N` and `# not run (failed to build): M`; `report_gen`'s header copy terminates on all three runaway modes (unopenable file, ≥255-char line, mid-read failure) with healthy output byte-identical — a pre-existing defect that could fill the disk | `step3.py`; `missing-sysinfo-no-hang`; `healthy-header-byte-identity` |
| 4 | The `disable_independent_assembly` opt-out knob (flag menu + POSIX `ifdef`), with the `MAKEFLAGS` route documented for scheduler-driven runs | `step4.py`; `makefile-clean-regression`; `step4-knob-intact-asm` |
| 5 | `CSB_TARGET_ARCH` appended after the feature-forcing blocks, so a forced-port build (CHERI, Morello) reports the arch it actually builds, not the host | `step5.py`; `step5-morello-dry-run` |
| 6 | Documentation synced: `README.md` (1025 row), `doc/architecture.html` (code row; algorithm steps 2/5/7; registry fields; dates), `doc/porting.html` (the knob, both link-dependent programs, the three invocation routes, dates), `CLAUDE.md` (rules, commands, scheduler list, record fields, and the lane-boundary clarification) | `step6.py`; `step6-doc-parity`; `check_catalog.py` |
| 7 | Behavioural acceptance: 17 checks, including the full 246-record native `exhausted-run` | tester report (SUCCESS, 17/17) |
| 8 | Pre-existing compile defect fixed: `cfi/return-to-peer-vfunc.cpp` gains `#include <cstdint>` (the only one of six `uintptr_t` users lacking it), so the native suite is 246/246 buildable again | coder auto checks; the unchanged `full-suite-native` acceptance |

**Files changed:** `scheduler/run-test.cpp`, `Makefile`, `cfi/return-to-peer-vfunc.cpp`, `README.md`, `doc/architecture.html`, `doc/porting.html`, `CLAUDE.md`.

**Test results.** Tester round 3 (post-Step-8): `overall_result: SUCCESS`, 17/17, zero blocking failures, zero skipped — `full-suite-native` reports the exhausted-run completing with both count lines `0`/`0`, all 246 records present, none at `1025`, and `gen_sum.py` totalling 246. Both coder rounds report `build_result: pass`, with the pre-existing `run-test.cpp` warning set unchanged at exactly 8.

**Review.** Phase 6 reviewer (Sonnet): "Issues found — non-blocking" — no Critical findings; ready to commit. Its three Suggestions are recorded as deferred improvements in the History's Review row (the non-string `skip` entry type-error, the by-value `case_skipped` parameter, and `report_gen`'s double iteration).

**Issues.** `2026-10-12-scheduler-skip-and-build-status-peer-vfunc-missing-cstdint` — found by the tester at Step 7, fixed in-plan as Step 8, `Resolved` (Resolved By tester, 2026-10-12).

**Open deferrals in the repository** (pre-existing; none opened, resolved or addressed by this plan — listed for visibility): `catalog-guard-hardening` (deferred 2026-10-10), `indepassembly-return-site-untranslated` (2026-10-11), `run-test-compiler-warnings` (2026-10-10), `run-test-continue-variables` (2026-09-30). No deferral from a prior plan is resolved by this work.

**Known caveats.** (1) The three reviewer Suggestions above — all optional, all in `scheduler/run-test.cpp`. (2) No `skip` declarations ship in `configure.json` by design: the mechanism is exercised through scratch registries, and ~~the affected arches stay covered by the opt-out plus build-failure auto-recording until the `indepassembly-return-site-untranslated` deferral closes.~~ Resolved 2026-10-14 by plan 2026-10-14-indepassembly-return-site-translation: the deferral closes with that plan — the opt-out (`disable_independent_assembly`) and the skip injection are now removable rather than load-bearing, which is the successor env plan's step; the auto-recording of build failures remains valid scheduler behaviour. (3) Windows/MSVC and the real CHERI/Morello builds are verified by dry-run only — no such toolchain exists on this host; the reviewer lists the same two areas as not fully verified.