# Plan: Translate assembly_return_site for riscv64 / aarch64 / cheri_riscv64 and add the Morello purecap variant, closing deferral indepassembly-return-site-untranslated
**Project:** /home/wsong83/proj/xiangshan/cpu-sec-test-env/benchmark
**Plan ID:** 2026-10-14-indepassembly-return-site-translation
**Created:** 2026-10-14

## Issue Log

Rows ordered newest-first.

| Issue | Status | Found | Resolved | Resolved By |
|-------|--------|-------|----------|-------------|
| [morello-purecap-indepassembly-bodies](./tmp/reports/defer-issue-indepassembly-return-site-untranslated.json) — Morello purecap rejects the aarch64 indepassembly bodies (pre-existing helper included); no purecap-clean translation exists | Resolved | 2026-10-13 | 2026-10-14 | [tester](./tmp/reports/2026-10-14-indepassembly-return-site-translation-tester-2026-10-14-2.json) |
| [indepassembly-return-site-untranslated](./tmp/reports/defer-issue-indepassembly-return-site-untranslated.json) | Resolved | 2026-10-11 | 2026-10-14 | [tester](./tmp/reports/2026-10-14-indepassembly-return-site-translation-tester-2026-10-14-2.json) |

Note on the first row: the Morello finding has no separate report file — it was surfaced by the env plan's dialect probe (recorded at `../results/cheri/probe-2026-10-13.json` and in `../scripts/cheri/dialect_probe.sh` evidence). The row links the same deferral JSON because that report carries the probe verdict provenance. As a row without its own `defer-issue-*.json`, it must be fully resolved by this plan (no deferral retirement applies to it).

## Guidance for Coder

**Files to modify:**

- `./lib/riscv64/indepassembly.asm`
- `./lib/aarch64/indepassembly.asm`
- `./lib/cheri_riscv64/indepassembly.asm`
- `./lib/aarch64/indepassembly_morello.asm` (new file)
- `./Makefile`

**Do NOT:** touch `./lib/x86_64/indepassembly.asm`, `./lib/x86_64/visualcpp_indepassembly_func.asm` or any other `lib/` file; remove or rename the `disable_independent_assembly` knob; touch `configure.json`, the test sources under `acc/ cfi/ cpi/ mss/ mts/`, `script/`, or `scheduler/`; reformat the existing `assembly_helper` bodies in the three files; change the `.globl` / `.type` directive lines (keep the per-arch syntax distinction: `@function` for riscv64/cheri_riscv64, `%function` for aarch64).

**Step-by-step with verification:** each step below carries coder checks tagged `(auto)` or `(scripted)` — `(auto)` checks are mechanical commands whose output is the check; the `(scripted)` check is `sh ./tmp/verification/2026-10-14-indepassembly-return-site-translation-step4.sh`, run from the repo root (exit 0 = pass, red until Step 4 lands). The coder self-verifies / runs them and records the evidence in the session report. Steps 1–4 are coder-owned; Step 5 is tester-owned.

## Guidance for Tester

**Tests to create:**

- **Temporary:** `./tmp/verification/2026-10-14-indepassembly-return-site-translation-native.sh` — native x86_64 validation: `make` (the scheduler), then `./run-test exhausted-run`; assert (a) the results for `cfi-return-to-peer-asmfunc` and `cfi-return-to-parent-non-call-site-by-asmfunc` are **successes in the scheduler's sense** — exit 0, **or** a code listed in the record's `expect-results` (both records declare only `290`, the CHERI code; a healthy native run exits 0, and 0 is never a member of `expect-results` because the scheduler short-circuits it to success at `scheduler/run-test.cpp:700-702`, so the assertion must be `result == 0 or result in expect-results`, never bare membership); (b) the run's not-run counts are `# not run (skipped for this platform): 0` and `# not run (failed to build): 0`; (c) optional comparison against the pre-change native reference stamp at `../results/x86_64/<stamp>/results.json` (cross-repo read; skip with a note if the sandbox denies it). Run `make clean` when done.
- **Temporary:** `./tmp/verification/2026-10-14-indepassembly-return-site-translation-inspect.sh` — independent re-assembly and disassembly of all four `.asm` files (three if the Step 4e fallback fired; do not reuse the coder's commands): assemble each with its pinned toolchain **and target triple** as listed in Steps 1–4 (both the CHERI and the Morello clangs need their `--target=` — without it they reject `-mabi=` on this host), then `objdump -d` the objects and verify the instruction sequences match the pinned texts in this plan (riscv64/cheri_riscv64: `addi sp,sp,-16` / `sd ra` / `sd s0` / `li a0,0` / `call exit` / restore / `ret`; aarch64: `sub sp,sp,#0x10` / `stp x29,x30,[sp]` / `mov x0,#0` / `bl exit` / restore / `ret`; morello: whatever body Step 4 settled on — verify it against the recorded choice in the coder's report); independently re-run the zero-x86-token scan over the four files.

**Coverage boundary (state it in the tester report):** in-guest cross-arch execution is out of scope here — the riscv64/aarch64 lanes currently run with the workarounds (`disable_independent_assembly=yes` + skip injection) and re-run only in the successor env plan (see Summary); the morello lane has no emulator until the C1 CHERI bring-up; and the cheri_riscv64 body (Step 3) has no executing lane before C1 either — the capability-ABI semantics of its plain `call`/`ret` pair are for the CHERI port owner to confirm, as the source deferral states. This plan's tester validates natively and at the object level only.

**Tests to investigate for retirement:** No test obsolescence identified — no test case, record, or helper is removed by this change.

## Summary

**Problem.** `assembly_return_site` in `lib/riscv64/indepassembly.asm`, `lib/aarch64/indepassembly.asm` and `lib/cheri_riscv64/indepassembly.asm` has carried the untranslated x86_64 body (`push %rbp; movq %rsp,%rbp; xor %rdi,%rdi; call exit; …`) since commit 789916e (2024-11-05). The object is linked into every test (`Makefile:494-545` path, `independent_assembly := $(addprefix lib/$(ARCH)/, indepassembly.o)` at `Makefile:244`), so every non-x86 port must build with `disable_independent_assembly=yes`, and the two records that call the object's symbols (`cfi-return-to-peer-asmfunc`, `cfi-return-to-parent-non-call-site-by-asmfunc`) are skipped on all non-x86 lanes via the env's injected `skip` list. The fix-forward decision was made 2026-10-12 (reverting to a pre-789916e bench commit was rejected: it would discard two years of suite progress).

**Approach.** Translate the symbol per arch from the already-verified fix texts (the env plan's dialect probe assembled and linked them; the riscv64 text additionally passed a full link probe), keep the existing `assembly_helper` bodies untouched, and add the missing piece the probe exposed: under `enable_aarch64_morello` the configuration is purecap (`--target=aarch64-unknown-freebsd -march=morello -mabi=purecap`), where the plain-aarch64 bodies (helper included) are rejected — so the morello combination gets its own variant file, `lib/aarch64/indepassembly_morello.asm`, selected by the Makefile when the flag is set (mirroring the existing MSVC variant precedent). The purecap bodies are structural translations of both functions, planner-probe-verified 2026-10-14 (the primary candidates assemble clean, both symbols defined, `exit` undefined for the link); Step 4a re-confirms them against the built Morello toolchain. Enabling that path also requires completing its flag set: the Morello clang defaults to the host triple and then rejects `-mabi=purecap`, so `--target=aarch64-unknown-freebsd` is added to the block's pre-existing `CXXFLAGS`/`OBJECT_CXXFLAGS` lines and to the new `ASMFLAGS` line. The missing `ASMFLAGS` target is the audit's High finding (2026-10-14): every documented invocation was unbuildable while all of the plan's own checks — dry runs — stayed green.

**Key facts the steps depend on:**

- riscv64/cheri_riscv64 `assembly_return_site` text (probe-verified with `riscv64-linux-gnu-as` and with the CTSRD clang — `--target=riscv64-unknown-freebsd -march=rv64gcxcheri -mabi=l64pc128d`; the triple is mandatory, the SDK clang otherwise defaults to the host triple and rejects `-mabi=`):
  ```
  assembly_return_site:
    # Save the stack info
    addi sp, sp, -16
    sd ra, 8(sp)
    sd s0, 0(sp)
    # Set exit code
    li a0, 0
    call exit
    # Restore stack info
    ld s0, 0(sp)
    ld ra, 8(sp)
    addi sp, sp, 16
    ret
  ```
- aarch64 `assembly_return_site` text (probe-verified with the aarch64 cross assembler):
  ```
  assembly_return_site:
    # Save the stack info
    sub sp, sp, #0x10
    stp x29, x30, [sp]
    # Set exit code
    mov x0, #0
    bl exit
    # Restore stack info
    ldp x29, x30, [sp]
    add sp, sp, #0x10
    ret
  ```
- The Morello toolchain lives at `../build/cheri/output/morello-sdk/bin/clang`; the probe invocation family is in `../scripts/cheri/dialect_probe.sh` (read-only reference). Under `-mabi=purecap` the plain-aarch64 bodies are rejected (`stp x29, x30, [sp]` → "invalid operand for instruction"); capability frames use `c29`/`c30` pairs (`stp c29, c30, [csp, #-32]!` / `ldp c29, c30, [csp], #32`); a function pointer arrives in `c0` and a return uses `c30`.

**Alternatives considered:** (a) a single aarch64 source assembling under both ABIs — rejected: no instruction form is valid under both (`stp c29,c30` unknown to plain `as`; `stp x29,x30,[sp]` rejected under purecap); (b) dropping the `exit` call from `assembly_return_site` — rejected: changes the symbol's semantics, which the two asmfunc records exercise; (c) leaving morello on `disable_independent_assembly` — kept only as the explicit fallback of Step 4.

**Out of scope (named successors):** the env-side follow-up (bump `BENCH_COMMIT`, remove the skip injection in `scripts/common/derive_registry.py` and `disable_independent_assembly` from `scripts/common/build_corpus.sh`, re-run the riscv64/aarch64 lanes so the two records execute for the first time, retire the env-side copy of the deferral) is its own env plan after this one commits and pushes; the CHERI emulator/image lanes are the C1 bring-up.

## Repo Mode

Public

## Document Overrides

| Document | Rule Overridden | Reason |
|----------|----------------|--------|

## Risks

| Risk | Impact | Mitigation |
|------|--------|------------|
| Every Morello candidate body is rejected by the purecap assembler | morello stays unbuildable without the workaround | primary candidate pair planner-probe-verified 2026-10-14; the bounded experiment (Step 4a) covers the per-line alternatives; explicit fallback (Step 4e: revert the variant, record the evidence, file a new deferral) if all are rejected |
| Morello purecap semantics cannot be proven before the C1 emulator exists | a latent wrong-jump in the future morello lane | prefer the structurally closest translation; state the boundary in the tester report; C1's acceptance runs the full suite on it |
| An edit slips into the plain aarch64 `assembly_helper` while editing the file | plain-build regression | Step 2's checks re-assemble the whole file and the tester re-inspects the disassembly; the helper body must stay byte-identical |
| Deferral half-open: env lanes still skip/disable until the successor env plan | the two records remain unverified in-guest for now | successor plan named in Summary; retirement mechanics in Guidance for Planner |

## Proposed Changes

Each step includes verification checks tagged with confidence. `(auto)` = mechanical, coder self-verifies.
Every step must have at least one verification check executable by its owner (coder steps: coder checks; tester steps: tester verify).

1. **Step 1:** In `lib/riscv64/indepassembly.asm`, replace the entire x86 body of `assembly_return_site` (the three `#`-commented x86 lines and `push %rbp` / `movq` / `xor` / `call` / `pop` / `ret`) with the pinned riscv64 text from Summary; normalize the label to column 0 (it currently carries two leading spaces); keep the `.globl`/`.type` directives and the `assembly_helper` body untouched; ensure the file ends with a trailing newline.
   → coder verify (auto): `riscv64-linux-gnu-as -o /tmp/indep-riscv64.o lib/riscv64/indepassembly.asm` exits 0; `riscv64-linux-gnu-objdump -t /tmp/indep-riscv64.o` lists both `assembly_helper` and `assembly_return_site` as `.text` symbols; `grep -nE 'push|movq|pop|%rsp|%rbp|%rdi' lib/riscv64/indepassembly.asm` returns nothing; `tail -c1 lib/riscv64/indepassembly.asm | od -An -tx1` is `0a`
2. **Step 2:** Same treatment in `lib/aarch64/indepassembly.asm` with the pinned aarch64 text; leave the `assembly_helper` body byte-identical (it is correct as-is).
   → coder verify (auto): `aarch64-linux-gnu-as -o /tmp/indep-aarch64.o lib/aarch64/indepassembly.asm` exits 0; `aarch64-linux-gnu-objdump -t /tmp/indep-aarch64.o` lists both symbols; the x86-token grep returns nothing; trailing newline present; `git diff lib/aarch64/indepassembly.asm` touches only the `assembly_return_site` stanza
3. **Step 3:** Same treatment in `lib/cheri_riscv64/indepassembly.asm` (identical text to the riscv64 file).
   → coder verify (auto): `../build/cheri/output/sdk/bin/clang --target=riscv64-unknown-freebsd -march=rv64gcxcheri -mabi=l64pc128d -x assembler -c -o /tmp/indep-cheri.o lib/cheri_riscv64/indepassembly.asm` exits 0 (toolchain path relative to this repo; absolute `/home/wsong83/proj/xiangshan/cpu-sec-test-env/build/cheri/output/sdk/bin/clang`; the `--target` triple is required — without it the SDK clang rejects `-mabi=` on this host, audit finding 2026-10-14); `riscv64-linux-gnu-objdump -t /tmp/indep-cheri.o` lists both symbols; x86-token grep empty; trailing newline present
4. **Step 4:** Add the Morello purecap variant and its Makefile selection.
   - a. Experiment (bounded — the primary candidate plus, per function, the listed per-line alternatives; if none assembles, go straight to (d) and take the fallback): assemble candidate bodies with `../build/cheri/output/morello-sdk/bin/clang --target=aarch64-unknown-freebsd -march=morello -mabi=purecap -x assembler -c` against `/tmp/*.o` outputs. Primary candidates (planner-probe-verified 2026-10-14 in this exact invocation: exit 0, both symbols defined, `exit` left undefined for the link) — `assembly_return_site`: `stp c29, c30, [csp, #-32]!` / `mov x0, #0` / `bl exit` / `ldp c29, c30, [csp], #32` / `ret`; `assembly_helper`: `stp c29, c30, [csp, #-32]!` / `str c0, [csp, #16]` (overwrite the saved link-register slot with the target capability) / `ldp c29, c30, [csp], #32` / `ret`. Per-line alternatives when the assembler rejects a line (name the failing line from its stderr): replace the pre-index pair with `sub csp, csp, #32` + `stp c29, c30, [csp]` and the paired restore with `ldp c29, c30, [csp]` + `add csp, csp, #32`; for the helper, fall back to `ldp c29, c30, [csp], #32` followed by `mov c30, c0` (equivalent: restore the frame chain, then set the link register from the passed capability). The offset spelling here is Morello's (`sub`/`add` on `csp`); `cincoffset` is CHERI-riscv spelling and the Morello assembler rejects it (audit finding 2026-10-14) — do not use it. Record the accepted body for both functions.
   - b. Create `lib/aarch64/indepassembly_morello.asm`: the same directive block as the aarch64 file (`.section .text`, the three `.globl` lines, `.type exit, %function`), then the accepted purecap `assembly_helper` and `assembly_return_site` bodies with the Step 1–3 comment style; trailing newline.
   - c. `Makefile`, two places:
     - At the `independent_assembly :=` definition (line 244) select the variant when the flag is set — `ifdef enable_aarch64_morello` → `independent_assembly := lib/aarch64/indepassembly_morello.o`, `else` the existing `$(addprefix lib/$(ARCH)/, indepassembly.o)` — placed BEFORE the existing `disable_independent_assembly` block (lines 249-252) so the disable knob still overrides.
     - In the existing `enable_aarch64_morello` block (lines 374-378): prepend `--target=aarch64-unknown-freebsd` to the option list on **both** existing lines (`CXXFLAGS += --target=aarch64-unknown-freebsd -march=morello -mabi=purecap -cheri-bounds=very-aggressive`, and the same for `OBJECT_CXXFLAGS`), and add `ASMFLAGS += --target=aarch64-unknown-freebsd -march=morello -mabi=purecap`. The triple is mandatory on every path: the Morello clang defaults to the host triple and then rejects `-mabi=purecap` (`clang: error: unsupported option '-mabi=' for target 'x86_64-unknown-linux-gnu'`, planner-verified 2026-10-14). The missing `ASMFLAGS` line is the audit's High finding — without it every documented invocation was unbuildable while `make -n` stayed green; the `CXXFLAGS`/`OBJECT_CXXFLAGS` gap is pre-existing, fixed here per the user's 2026-10-14 decision. The `.asm` recipe (Makefile:495-496) passes `$(ASMFLAGS)`; the Morello clang invoked as `$(ASM)` parses a `.asm` input as assembly once `--target` is present, so no `-x assembler` is needed (planner-probe-verified).
   - d. If no candidate and no alternative assembles clean (the bound in (a)), take the fallback (e) directly: do NOT leave a broken file.
   - e. Fallback (only if d applies): revert 4b–4c in the same session, write the failed-candidate evidence into the session report, and file a new deferral via `/defer-issue` describing the rejected forms — the plan then completes with Steps 1–3 + 5 only. In this branch Step 4's scripted check and the tester's fourth-file inspection are not applicable; the recorded fallback evidence replaces them.
   → coder verify (auto): (a–b) the final `lib/aarch64/indepassembly_morello.asm` assembles with the morello clang invocation above (exit 0) and `aarch64-linux-gnu-objdump -t` lists both symbols; the file ends with a trailing newline (`tail -c1 lib/aarch64/indepassembly_morello.asm | od -An -tx1` is `0a`) and the x86-token grep `grep -nE 'push|movq|pop|%rsp|%rbp|%rdi'` over it returns nothing; (c) the wiring dry run `make -n -B lib/aarch64/indepassembly_morello.o enable_aarch64_morello=yes` prints a recipe assembling `indepassembly_morello.asm` that carries `--target=aarch64-unknown-freebsd -march=morello -mabi=purecap`; `git diff Makefile` shows only the intended conditionals and flag lines
   → coder verify (scripted): `sh ./tmp/verification/2026-10-14-indepassembly-return-site-translation-step4.sh` exits 0 — it re-checks the wiring, performs the **real** Makefile build of the variant object with `ASM=<morello clang>` (the audit's High finding: a dry run cannot catch an assembler incapable of the flags), checks both symbols in the built object, removes it, and re-checks the plain path with `ARCH=aarch64` (the plain dry run needs that variable — without it, on an x86_64 host, `$(independent_assembly)` resolves to `lib/x86_64/indepassembly.o` and make fails with `No rule to make target`; do not add `2>&1` to that grep, since make's own error text contains `indepassembly.o` and would satisfy it spuriously)
   → tester verify (Step 5's inspect script): the fourth file's disassembly matches the accepted purecap bodies recorded by the coder
5. **Step 5:** Validate natively and independently inspect the translated objects (tester-owned; see Guidance for Tester for the two scripts).
   → tester verify: both temporary scripts pass; their reports state the coverage boundary (in-guest cross-arch execution deferred to the successor env plan and C1)

## Guidance for Planner

Documentation tasks you will execute yourself (not the coder):

- **Files to modify:**
  - `./doc/porting.html`
  - `./CLAUDE.md`
- **When:** after the coder finishes (before final revision).
- **What to sync:** the `indepassembly` paragraph (~lines 133-135) says the object is assembled from `lib/<ARCH>/indepassembly.asm`, "with the MSVC variant … used on Windows" — add the Morello variant sentence (`lib/aarch64/indepassembly_morello.asm`, selected under `enable_aarch64_morello`). Three further touch-ups the audit surfaced: the "What a test case links against" code block (~line 126) hardcodes `lib/<ARCH>/indepassembly.o`, which Step 4c makes false for Morello builds — add the variant path there; the "Hardware feature switches" paragraph (~lines 107-116) lists `-march=morello -mabi=purecap` for `enable_aarch64_morello` — extend it with the now-mandatory `--target=aarch64-unknown-freebsd` (the switch's flag set was incomplete for the assembler step, and for the C++ step on a non-FreeBSD-triple host, before this plan); and the `lib/aarch64/` directory row (~lines 55-57, which already claims Morello use) may want the variant file named. Re-check the file-listing sentence at ~lines 51-52 and the `disable_independent_assembly` passage (~lines 139-150) for needed touch-ups; the switch itself remains documented and valid.
- **New in this revision:** the repo now carries `./.claude/settings.json` (a permissions allowlist added 2026-10-14 with the user's approval, to unblock unattended rounds from the harness permission layer — see History). Add one line to `./CLAUDE.md`'s Commands section recording what the allowlist is for, so a future reader does not mistake it for stray config.
- **Deferral retirement:** this plan resolves the bench-side `./tmp/reports/defer-issue-indepassembly-return-site-untranslated.json` — `/update-and-commit` Step 9.6 retires it when the Issue Log row reaches Resolved. The ENV-side copy (`../tmp/reports/defer-issue-indepassembly-return-site-untranslated.json`) and the protocol-level workarounds (`skip` injection in `../scripts/common/derive_registry.py`; `disable_independent_assembly` in `../scripts/common/build_corpus.sh`) are NOT retired here — they belong to the successor env plan, which must also bump `BENCH_COMMIT` in `../env.sh` to this plan's commit. Do not touch the env repo from this plan.
- **Micro-decision for Step 4:** if the fallback (4e) fires, the Issue Log's morello row becomes `Open` with the new deferral linked; confirm with the user at the revision instead of resolving it silently.

## History

| Date | Event | Conclusion | Dismissed |
|------|-------|------------|-----------|
| 2026-10-14 | Initial plan | Fix-forward translation of the three `assembly_return_site` bodies plus the newly exposed Morello purecap variant, per the user's 2026-10-14 directive that B1 run immediately. | — |
| 2026-10-14 | Mega-audit | Pre-implementation, 8 of 9 lenses returned — **2 High, 9 Medium, 10 Low**. Both Highs are the same root defect, reproduced by the planner and fixed here: the new `ASMFLAGS` line lacked the mandatory `--target=aarch64-unknown-freebsd`, so every documented Morello invocation was unbuildable while all of the plan's checks (dry runs) stayed green ([high-00-defect-tracer](./tmp/reports/2026-10-14-indepassembly-return-site-translation-mega-audit-2026-10-14.json) [high-11-edge-case-explorer](./tmp/reports/2026-10-14-indepassembly-return-site-translation-mega-audit-2026-10-14.json)); the fix adds the triple to `CXXFLAGS`/`OBJECT_CXXFLAGS` too (pre-existing gap, user-approved 2026-10-14) and replaces the dry-run-only gate with a scripted real Makefile build. All 9 Mediums fixed: the Step 3 CHERI command's missing `--target` ([medium-01-defect-tracer](./tmp/reports/2026-10-14-indepassembly-return-site-translation-mega-audit-2026-10-14.json) [medium-12-edge-case-explorer](./tmp/reports/2026-10-14-indepassembly-return-site-translation-mega-audit-2026-10-14.json)), the plain dry run's missing `ARCH=aarch64` and its `2>&1` false-pass trap ([medium-02-defect-tracer](./tmp/reports/2026-10-14-indepassembly-return-site-translation-mega-audit-2026-10-14.json) [medium-13-edge-case-explorer](./tmp/reports/2026-10-14-indepassembly-return-site-translation-mega-audit-2026-10-14.json) [medium-18-security](./tmp/reports/2026-10-14-indepassembly-return-site-translation-mega-audit-2026-10-14.json)), the tester's inverted `expect-results` assertion — 0 is the scheduler's success and is never a member ([medium-03-defect-tracer](./tmp/reports/2026-10-14-indepassembly-return-site-translation-mega-audit-2026-10-14.json) [medium-14-edge-case-explorer](./tmp/reports/2026-10-14-indepassembly-return-site-translation-mega-audit-2026-10-14.json)), the `doc/porting.html` morello-flag claim reconciled with the now-mandatory triple ([medium-10-rule-enforcer](./tmp/reports/2026-10-14-indepassembly-return-site-translation-mega-audit-2026-10-14.json)), and the link-objects code block added to the planner's What-to-sync list ([medium-20-boundary-checker](./tmp/reports/2026-10-14-indepassembly-return-site-translation-mega-audit-2026-10-14.json)). Five Lows folded in passing: the dead `cincoffset` ladder rung ([low-04-defect-tracer](./tmp/reports/2026-10-14-indepassembly-return-site-translation-mega-audit-2026-10-14.json)), the 4a bound / 4d direct-to-fallback / 4e branch flow ([low-06-defect-tracer](./tmp/reports/2026-10-14-indepassembly-return-site-translation-mega-audit-2026-10-14.json)), the cheri_riscv64 omission from the coverage boundary ([low-07-defect-tracer](./tmp/reports/2026-10-14-indepassembly-return-site-translation-mega-audit-2026-10-14.json)), the fourth file's trailing-newline and x86-token checks ([low-08-defect-tracer](./tmp/reports/2026-10-14-indepassembly-return-site-translation-mega-audit-2026-10-14.json)), and the inspect script's fallback branch ([low-17-edge-case-explorer](./tmp/reports/2026-10-14-indepassembly-return-site-translation-mega-audit-2026-10-14.json)). **Partial audit:** the conflict-resolver lens hung >15 min with no journal progress; the workflow never aggregated, so the planner stopped it and reconstructed the report from the eight returned lens results — per the workflow-failure rule it is not counted toward the 3-iteration cap and may be re-run. Report: [2026-10-14-indepassembly-return-site-translation-mega-audit-2026-10-14.json](./tmp/reports/2026-10-14-indepassembly-return-site-translation-mega-audit-2026-10-14.json) | Low, acknowledged no-fix: [low-05-defect-tracer](./tmp/reports/2026-10-14-indepassembly-return-site-translation-mega-audit-2026-10-14.json) and [low-16-edge-case-explorer](./tmp/reports/2026-10-14-indepassembly-return-site-translation-mega-audit-2026-10-14.json) (the morello object missing from the disable knob's `rubbish` enumeration — hygiene-only, `*.o` is gitignored, and the combined flag route is not documented), [low-09-oversight-catcher](./tmp/reports/2026-10-14-indepassembly-return-site-translation-mega-audit-2026-10-14.json) (no rollback section — the template has none; Step 4e covers the plan's one risky change), [low-15-edge-case-explorer](./tmp/reports/2026-10-14-indepassembly-return-site-translation-mega-audit-2026-10-14.json) (token grep without `xor`/`leave` or word boundaries — the assembler check backstops it and the sibling steps share the pattern), [low-19-security](./tmp/reports/2026-10-14-indepassembly-return-site-translation-mega-audit-2026-10-14.json) (fixed `/tmp` object names, CWE-59 class on a single-user dev host) |
| 2026-10-14 | Implementation round (coder) — failed round; retry used (1) | Launch 13:25–13:46 exited 0 and wrote no session report (no `.prestate` snapshot existed, so the launcher surfaced it rather than relaunching); the tree nevertheless carries all of Steps 1–4, independently verified by the planner (§ assess-and-brief): all three translations assemble with their pinned toolchains and define both symbols, zero x86-token residue in all four files, trailing newlines present, the Morello variant builds through the Makefile, and `step4.sh` — red before this round — exits 0. The relaunch's brief is therefore report-only. | — |
| 2026-10-14 | Spec revision (/update-plan) | Post-coder invocation (round 2, the report-only retry): no new issues — [the coder's report](./tmp/reports/2026-10-14-indepassembly-return-site-translation-coder-2026-10-14-2.json) has `issues_filed: []` and `build_result: pass` — and no tester report exists yet, so both Issue Log rows stay `Fix Planned`; resolution awaits the tester's verification, which is why the finalize branch's precondition (every non-deferred issue Resolved) is not met. No plan content changed; the spent coder brief is removed from `## Immediate Actions` per the staleness safeguard. Next round: the tester, still iteration 1. | — |
| 2026-10-14 | Implementation round (tester) — failed round; retry used (1) | Launch 14:20–14:33 exited 0 with no report and no scripts on disk; its transcript shows the round had already checked all four files against the pinned texts and probed each toolchain's disassembly shape (riscv `call` → `auipc`+`jalr` relaxation; morello prints `ret c30` and `#-0x20`), ending mid-work as it began writing the two scripts — a truncation, not a disagreement with the plan. The relaunch's brief therefore carries report-first hygiene, so a second truncation still leaves a verdict. | — |
| 2026-10-14 | Implementation round (tester) — BLOCKED verdict | The retry (14:34–14:56) produced [the tester report](./tmp/reports/2026-10-14-indepassembly-return-site-translation-tester-2026-10-14.json) with `overall_result: BLOCKED`: this session's permission layer denied `./run-test`, `sh`, argumented `make`, the cross assemblers, `objdump` and the CHERI/Morello clangs, so Step 5's native-suite gate could not start — no pass/fail verdict issued. The report-first hygiene worked (skeleton on disk from 14:38; both scripts written) and its substitution evidence is green: its own content checker ran 38/38 PASS via `python3` (pinned texts, byte-identity, Makefile wiring, expected codes). | — |
| 2026-10-14 | Spec revision (/update-plan) — blocker resolution | The BLOCKED blocker was resolved, not revised around (stop-and-ask case 1). With the user's 2026-10-14 approval, a repo-level [`.claude/settings.json`](./.claude/settings.json) allowlist now covers `./run-test`, `sh`/`bash` on `tmp/verification/*`, every `make` form, the riscv64/aarch64 cross binutils, `objdump`/`nm`/`readelf`, the CHERI/Morello toolchain paths and `python3`; it was verified empirically against a throwaway `--settings <tester settings>` probe session (all five probed command forms ran without approval, where the same layer had refused them minutes earlier). Guidance for Planner gained the matching `CLAUDE.md` one-liner. No step or Guidance-for-Coder/Tester content changed. Next round: the tester again (iteration 1). | — |
| 2026-10-14 | Implementation round (tester) — SUCCESS | Relaunch 3 (15:02–15:34) wrote [the verdict report](./tmp/reports/2026-10-14-indepassembly-return-site-translation-tester-2026-10-14-2.json): `overall_result: SUCCESS`, 3/3, no blocking failures and no skipped-test warnings — the inspect script (revised this round for TAB-aware disassembly patterns and the Morello-capable objdump, since GNU `aarch64-linux-gnu-objdump` cannot decode purecap) passes on all four objects, the native `exhausted-run` is healthy with both asmfunc records at result 0 and not-run counts 0/0 against the pre-change reference stamp, and the repo's `check_catalog.py` guard stays green. Two observations explained rather than filed: `cfi-return-to-peer-asmfunc`'s log shows interim `Try test failed` lines (exit 1, then 267) before its successful third invocation — the scheduler's documented multitry/early-exit-on-0 loop, with a final result identical to the pre-change reference; and the cheri object's benign `x ISA extension xcheri` objdump warning. Temporary scripts retained in `./tmp/verification/` (machine-local, gitignored) as this plan's verification record. | — |
| 2026-10-14 | Spec revision (/update-plan) — finalize path | Post-tester invocation: [the SUCCESS report](./tmp/reports/2026-10-14-indepassembly-return-site-translation-tester-2026-10-14-2.json) verifies both Issue Log rows, so both moved `Fix Planned` → `Resolved` (Resolved By: tester) — the finalize branch's precondition is met and no in-scope issue remains. No step or Guidance content changed; the spent tester brief is removed from `## Immediate Actions`. Next: Phase 6 final review (reviewer subagent, then `## Final Results` and the `doc/porting.html` + `CLAUDE.md` sync). | — |
| 2026-10-14 | Review | Phase 6 final review (reviewer subagent, Sonnet): verdict **"Issues found — non-blocking"** — **0 Critical, 0 Warning, 5 Suggestions**; ready to commit as-is. It independently re-assembled and disassembled all four `.asm` files with their pinned toolchains and target triples (both symbols defined and `exit` UND in every object; the Morello disassembly byte-matches the plan's primary candidates), compared each translation programmatically and whitespace-insensitively against the plan's pinned texts, confirmed the helper stanzas byte-identical to HEAD and the `Makefile` change minimal (variant selection before and overridden by the disable knob, `--target` on all three flag lines), re-ran `step4.sh` to exit 0, and spot-checked the scheduler's 0-success short-circuit that the tester's assertion (a) rests on. [report](./tmp/reports/2026-10-14-indepassembly-return-site-translation-review-2026-10-14.json) | Suggestions (non-blocking): [review-s1-link-stage-unexercised](./tmp/reports/2026-10-14-indepassembly-return-site-translation-review-2026-10-14.json) — no gate links the three non-x86 objects; folded into `## Final Results` as a handoff caveat, [review-s2-toolchain-selection-doc](./tmp/reports/2026-10-14-indepassembly-return-site-translation-review-2026-10-14.json) — the porting.html switch paragraph must carry the caller-supplied `CXX=`/`ASM=` requirement alongside `--target`; folded into this revision's doc sync, [review-s3-allowlist-arbitrary-exec](./tmp/reports/2026-10-14-indepassembly-return-site-translation-review-2026-10-14.json) — `Bash(python3 *)`/`Bash(make *)` are arbitrary-execution equivalents in a user-approved file; narrowing is the user's call, [review-s4-allowlist-narrower-than-needed](./tmp/reports/2026-10-14-indepassembly-return-site-translation-review-2026-10-14.json) — the absolute toolchain path form and bare `rm` are denied (observed live); surfaced to the user, [review-s5-pre-existing-arch-eval](./tmp/reports/2026-10-14-indepassembly-return-site-translation-review-2026-10-14.json) — pre-existing `$(ARCH)` evaluation at `Makefile:251` for the cheri path; noted for the successor |

## Plan Metadata

```json
{
  "plan_id": "2026-10-14-indepassembly-return-site-translation",
  "steps": ["Step 1: Translate assembly_return_site in lib/riscv64/indepassembly.asm", "Step 2: Translate assembly_return_site in lib/aarch64/indepassembly.asm", "Step 3: Translate assembly_return_site in lib/cheri_riscv64/indepassembly.asm", "Step 4: Add the Morello purecap variant and its Makefile selection", "Step 5: Validate natively and independently inspect the translated objects (tester)"],
  "coder_files": ["./lib/riscv64/indepassembly.asm", "./lib/aarch64/indepassembly.asm", "./lib/cheri_riscv64/indepassembly.asm", "./lib/aarch64/indepassembly_morello.asm", "./Makefile"],
  "tester_files": ["./tmp/verification/2026-10-14-indepassembly-return-site-translation-native.sh", "./tmp/verification/2026-10-14-indepassembly-return-site-translation-inspect.sh"],
  "doc_files": ["./doc/porting.html", "./CLAUDE.md"],
  "verification_scripts": ["./tmp/verification/2026-10-14-indepassembly-return-site-translation-step4.sh"],
  "repo_mode": "Public",
  "document_overrides": []
}
```

## Audit Accumulation

| Counter | Value | Last Updated |
|---------|-------|--------------|
| issues_resolved_since_audit | 2 | 2026-10-14 |
| steps_changed_since_audit | 0 | 2026-10-14 |
| files_changed_since_audit | 0 | 2026-10-14 |

## Documentation

Documentation owned by the planner: `./doc/porting.html` (variant sentence — see Guidance for Planner). No `README.md` change: the `disable_independent_assembly` switch stays documented and valid. `doc/test-catalog.html` is unaffected (no record/program change); `script/check_catalog.py` stays green.

## Final Results

**Status: COMPLETED** — 2026-10-14

**What was built.** Fix-forward translation of `assembly_return_site` in the three non-x86 POSIX independent-assembly sources — `lib/riscv64/indepassembly.asm`, `lib/aarch64/indepassembly.asm`, `lib/cheri_riscv64/indepassembly.asm` — whose bodies had carried untranslated x86-64 code since commit 789916e (2024-11-05), making every non-x86 port unbuildable for ~23 months. Plus the newly exposed Morello purecap variant `lib/aarch64/indepassembly_morello.asm` and its Makefile selection, which also completes the `enable_aarch64_morello` flag set (`--target=aarch64-unknown-freebsd` on `CXXFLAGS`, `OBJECT_CXXFLAGS` and the new `ASMFLAGS`; without it every documented Morello invocation was unbuildable — the audit's High finding).

**Files changed.** Coder: the five files above. Planner (this revision's doc sync): `./doc/porting.html`, `./CLAUDE.md`. Environment: `./.claude/settings.json` (new; user-approved 2026-10-14 permissions allowlist added to unblock unattended rounds — see History).

**Steps.** All five completed. Step 4's fallback (4e) did not fire — the primary purecap candidate bodies were accepted, exactly as the planner's probe predicted.

**Test results.** Tester verdict `SUCCESS`, 3/3, no blocking failures ([report](./tmp/reports/2026-10-14-indepassembly-return-site-translation-tester-2026-10-14-2.json)): all four files independently re-assembled and disassembled with their pinned toolchains and target triples; native `./run-test exhausted-run` healthy — `cfi-return-to-peer-asmfunc` and `cfi-return-to-parent-non-call-site-by-asmfunc` both recorded result 0 with not-run counts `0`/`0`, identical to the pre-change reference stamp; repo catalog guard green (246/246 records, 103/103 programs); plus the previous round's 38/38 content checks carried forward by reference.

**Coder report.** `build_result: pass` ([report](./tmp/reports/2026-10-14-indepassembly-return-site-translation-coder-2026-10-14-2.json)). Round 1 exited without a report and its work was independently verified by the planner before the report-only retry (History).

**Review verdict.** 0 Critical, 0 Warning, 5 Suggestions — non-blocking, ready to commit ([report](./tmp/reports/2026-10-14-indepassembly-return-site-translation-review-2026-10-14.json)). Two Suggestions were folded into this revision (the porting.html `CXX=`/`ASM=` requirement; the link-stage handoff caveat below); the rest are recorded in the History Dismissed cell.

**Issues resolved (2).** `indepassembly-return-site-untranslated` — the bench-side deferral, retired by `/update-and-commit` Step 9.6; `morello-purecap-indepassembly-bodies` — no separate report file, closed by this plan as the plan required. No deferred issue remains open in this plan.

**Known caveats.**
1. **Coverage boundary (as designed).** In-guest cross-arch execution is unverified: the riscv64/aarch64 lanes still run with `disable_independent_assembly=yes` plus the env-side `skip` injection until the successor env plan re-runs them after bumping `BENCH_COMMIT`; the morello and cheri_riscv64 lanes have no executing emulator before the C1 CHERI bring-up. Object-level and native-adjacent evidence only.
2. **Link stage unexercised for the three non-x86 objects** (reviewer S1). Every gate in this plan is assemble + `objdump`; the successor env plan's lane runs and the C1 acceptance checklist must carry link-stage verification explicitly so it is not lost at handoff.
3. **Pre-existing `$(ARCH)` evaluation quirk** at `Makefile:251` for the `enable_riscv64_cheri` path (reviewer S5): `ARCH` is forced after that line's immediate expansion, masked today only because every real caller passes `ARCH=` on the command line. Out of scope here; noted for the successor.
4. **Temporary scripts retained** — the three `./tmp/verification/2026-10-14-indepassembly-return-site-translation-*.{sh,py}` files stay as this plan's machine-local (gitignored) evidence record rather than being deleted at completion.
5. **`.claude/settings.json` breadth** (reviewer S3/S4): `Bash(python3 *)`/`Bash(make *)` are arbitrary-execution equivalents, while the absolute CHERI toolchain path form and bare `rm` are not covered. The file is explicitly user-approved; narrowing or extending is the user's call.

## Cross-References to Prior Plans

| Prior Plan | Deferred Issue | Resolution |
|------------|---------------|------------|
| 2026-10-11-riscv64-nemu-linux-platform | `[indepassembly-return-site-untranslated]` (its Issue Log row, status Open, found 2026-10-11) | This plan translated all three `assembly_return_site` bodies and added the Morello variant, so the deferral that plan's Step 4 cross-build depended on is closed bench-side. That plan's own remaining work (corpus mode, image recipe, guest harness) is untouched; the env-side follow-ups (skip-injection removal, `disable_independent_assembly` pin drop, `BENCH_COMMIT` bump, lane re-runs) are its successor env plan. |
| 2026-10-12-scheduler-skip-and-build-status | "the affected arches stay covered by the opt-out plus build-failure auto-recording until the `indepassembly-return-site-untranslated` deferral closes" (Known Caveats item 2) | The deferral closes with this plan: the opt-out (`disable_independent_assembly`) and the skip injection are now removable rather than load-bearing, which is exactly the successor env plan's step. The auto-recording of build failures remains valid scheduler behaviour. |