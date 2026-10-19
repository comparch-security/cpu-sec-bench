# Plan: Platform-correct opcode extraction — route `acc-read-func`'s side file through `$(OBJDUMP)`, add the cheri_riscv64 branch, unblock the four aarch64 lanes and the cheri_riscv64 deploy
**Project:** /home/wsong83/proj/xiangshan/cpu-sec-test-env/benchmark
**Plan ID:** 2026-10-19-func-opcode-extraction-platform-fix
**Created:** 2026-10-19

## Issue Log

Rows ordered newest-first.

| Issue | Status | Found | Resolved | Resolved By |
|-------|--------|-------|----------|-------------|
| [bench-expectation-gap-cfi-fakefunc-255](../reports/defer-issue-bench-expectation-gap-cfi-fakefunc-255.json) — pre-existing registry expectation gap: `cfi-return-to-parent-wrong-call-site-fakefunc` returns 255, undeclared in its `expect-results` (gcc 15.2.0 / binutils 2.46); unchanged by this plan and already tracked open by the env-side report `2026-10-12-test-env-all-archs-issue-review-lines-bench-expectation-gaps.json`. **Deferred** — this local report points at that authoritative tracker rather than double-filing the workstream | Open (deferred) | 2026-10-19 | — | — |
| [makefile-arch-parse-order-residual](../reports/defer-issue-makefile-arch-parse-order-residual.json) — pre-existing Makefile parse-order defect: `enable_riscv64_cheri` / `enable_aarch64_morello` force `ARCH` after earlier ARCH consumers (`independent_assembly`, `rubbish`, the `x86_64` option blocks) are evaluated; this plan folds the flag into the extractor selection only. **Deferred** — fixing the rest changes which objects a flag-only invocation builds and needs its own verification | Open (deferred) | 2026-10-19 | — | — |
| [handoff-reference-leg-post-install](../reports/2026-10-19-func-opcode-extraction-platform-fix-handoff-reference-leg-post-install.json) — the harness check scripts' reference leg invokes the *installed* cross extractor with a bare `objdump`, which the new explicit-knob requirement refuses, so the 15/15 + 6/6 acceptance is not reproducible post-install. Fix: the reference leg reads pristine pre-install bodies via `git show HEAD:script/...`, and the post-install word is validated independently. Fixed: the reference leg reads `git show HEAD:script/...` in both check scripts (re-run green: 21/21 and 6/6), and the tester's post-install fixtures validate the installed state independently | Resolved | 2026-10-19 | 2026-10-19 | tester (independent post-install fixtures) + planner (harness re-run) |
| [cheri-extractor-stale-header](../reports/2026-10-19-func-opcode-extraction-platform-fix-cheri-extractor-stale-header.json) — the newly installed `script/get_cheri_riscv64_func_inst.sh` carries its staging header ("STAGED DRAFT", "write-denied") as a tracked comment; replace with the plain xcheri explanation (comment-only). **Applied** in coder round 2 — the 4-line staging header is replaced by a 2-line pointer, `bash -n` is clean, and no `STAGED DRAFT`/`write-denied` marker remains in `script/`, `Makefile`, `acc/`, `lib/` or `scheduler/` — the tester's re-run confirms the change is behaviourally inert (every word identical, suite 246/246) | Resolved | 2026-10-19 | 2026-10-19 | tester |
| [cheri-riscv64-needs-cheri-capable-objdump](../reports/defer-issue-cheri-riscv64-needs-cheri-capable-objdump.json) — cross-repo (filed here, `target_repo: cpu-sec-test-env`): the env must pass the CTSRD SDK's `llvm-objdump` as `OBJDUMP` for the cheri lane (`scripts/common/build_corpus.sh` cheri case + `scripts/cheri_riscv64/prepare.sh:120`); until then the cheri corpus build stops loudly at the extraction, naming the tool it tried | Open | 2026-10-19 | — | — |
| [windows-msvc-objdump-passthrough-verification](../reports/defer-issue-windows-msvc-objdump-passthrough-verification.json) — local to this repo: the Windows/MSVC half of the `$(OBJDUMP)` pass-through (the MSVC recipe line and `script/get_x64_func_inst.bat`) cannot be exercised from this host; verification deferred at the user's request | Open | 2026-10-19 | — | — |
| [aarch64-func-opcode-extraction-host-objdump](../../../tmp/reports/defer-issue-aarch64-func-opcode-extraction-host-objdump.json) — cross-repo (report filed in the env repo, `target_repo: benchmark`): the extraction ran with the host objdump; the sweep's hard error turned it into 1025 on all four aarch64 lanes; env residual = why the per-lane shim did not take effect. **Deferred (hold)** — the report lives in the env repo and is retired cross-repo by the successor env cycle, after its lane re-run at this plan's commit (see Guidance for Planner) | Open (deferred — env cycle) | 2026-10-19 | — | — |
| [cheri-riscv64-opcode-extraction-deploy-abort](../../../tmp/reports/defer-issue-cheri-riscv64-opcode-extraction-deploy-abort.json) — cross-repo: `cheri_riscv64` matches no extractor branch (Makefile:237-241) → x64 fallback on an xcheri binary → hard error → deploy aborts; no cheri_riscv64 stamp can exist at pin `c8c6000`. **Deferred (hold)** — the report lives in the env repo and is retired cross-repo by the successor env cycle, after its lane re-run at this plan's commit (see Guidance for Planner) | Open (deferred — env cycle) | 2026-10-19 | — | — |

## Guidance for Coder

**Files to modify:**

- `./Makefile`
- `./acc/read-func.cpp` (the parse check — user-authorized scope expansion, already applied)
- `./script/get_x64_func_inst.sh`
- `./script/get_aarch64_func_inst.sh`
- `./script/get_riscv64_func_inst.sh`
- `./script/get_cheri_riscv64_func_inst.sh` (new, per the Step 2 (a) resolution)
- `./script/get_x64_func_inst.bat` (pattern-parity only; Windows verification deferred — see the Issue Log)

**Do NOT:** weaken the sweep's hardening (the exactly-one-address check, the empty-extraction hard error — they are what surfaced both defects and must stay); change any extractor script's argument contract (`<binary> <func_name> <n_bytes> <out_file>`); touch test-case sources (exception: the `acc/read-func.cpp` parse check — user-authorized, already applied), `configure.json`, `lib/`, or the `.gen` record semantics beyond the extraction dependency; touch `doc/**` or `CLAUDE.md` (planner-owned); edit the env repo (the CHERI-objdump supply now travels as the cross-repo deferral in the Issue Log).

**Step-by-step with verification:** `(auto)` checks are mechanical commands whose output is the check. Steps 1–2 are coder-owned with a planner decision gate at Step 2; Steps 3–4 tester-owned; Step 5 planner-owned.

## Guidance for Tester

**Tests to create:**

- **Temporary:** `./tmp/verification/2026-10-19-func-opcode-extraction-platform-fix-fixtures.sh` — per-platform end-to-end extraction proof, runnable fully in-repo on this host (the env's toolchains are present): build `test/acc-read-func` (and the `-func-opcode.tmp` side file) with (i) x86_64 native, (ii) `ARCH=aarch64 CXX=aarch64-linux-gnu-g++-13 ASM=aarch64-linux-gnu-as OBJDUMP=aarch64-linux-gnu-objdump`, (iii) `ARCH=riscv64` with the riscv64 cross tools — each time asserting the side file is exactly the helper's real first-8-bytes word (non-empty, one 16-hex-digit value) and that the extraction **names the tool it used** (the new audit line). Assert the negative control: with `OBJDUMP=/nonexistent-objdump` the extraction fails loudly naming the tool, never silently. Validate each word against a value computed **independently of the installed extractor** (the raw disassembly byte column reversed by the fixture's own parser), never against a pre-install body. Add the two further negative controls: `OBJDUMP` unset on a cross target fails loudly naming the knob, and `byte_nums=64` — more bytes than the region holds — fails loudly with the width message.
- **Temporary:** `./tmp/verification/2026-10-19-func-opcode-extraction-platform-fix-cheri.sh` — the Step 2 **(a)** proof (the gate resolved to (a); (b) is dismissed): the CHERI-capable extraction produces the correct word from the xcheri binary — equal to the independent reversal of `helper`'s first 8 code bytes, and cross-checked against the plain-riscv64 mechanism — run with the tool the env will actually supply: `OBJDUMP=<env-root>/build/cheri/output/sdk/bin/riscv64cheri-objdump` (the SDK wrapper over `llvm-objdump`; byte-identical output to bare `llvm-objdump`, per the env planner's note), not a bare `llvm-objdump`. Include the loud-failure controls (the env's GNU riscv64 objdump, the host x86_64 objdump, a nonexistent tool) and the aarch64-morello combination as an unaffected-control (`ARCH=aarch64 enable_aarch64_morello=yes` extraction still correct).
- **Tests to update:** none machine-local from prior cycles require re-pointing here (no stamps change); re-run the sweep's `...-func-opcode.sh` fixture suite as a regression — the collision fixture (`assembly_helper` + `helper`) must still extract exactly once.

**Tests to investigate for retirement:** No test obsolescence identified.

**Boundary to state in the tester report:** the four aarch64 lanes' and the cheri_riscv64 lane's **real-lane** confirmation is the env successor's re-bump + re-run cycle (named below); this plan's evidence is build/extraction level on this host.

## Summary

**Problem.** Pinning the sweep's `c8c6000` exposed two latent extraction-path defects it deliberately hardened against: (1) the extractor scripts call **bare `objdump`**, so on the four aarch64 lanes the extraction resolved to the host x86_64 objdump (`can't disassemble for architecture UNKNOWN!`), and the new hard error turned `acc-read-func` into a build failure (`1025`) where the old pin's lenient path had accidentally produced the correct word; (2) the extractor selection (Makefile:237-241) knows only `aarch64` and `riscv64`, so `cheri_riscv64` falls back to the **x64 extractor against an xcheri binary** (`unsupported relocation type 0xc1`) and the corpus build now aborts — no `cheri_riscv64` stamp can exist at the pin. Everything else at the pin is confirmed green: riscv64 (`acc-read-func` 0, the declared SIGPROT/`mss` codes observed), the aarch64-morello lane, and the full native set.

**Approach.** One principled bench change fixes both: the extraction stops resolving tools by PATH and honors the platform knob the bench already publishes — **`$(OBJDUMP)`** (set at command-line strength by every env lane's MAKEFLAGS today; `plan 2026-10-19-pin-bump-lane-confirmation`'s result analysis: `tmp/reports/2026-10-19-pin-bump-lane-confirmation-result-analysis.md`). The Makefile passes it into the extractor invocation and the scripts use `${OBJDUMP:-objdump}`. For `cheri_riscv64`, the selection gains its own branch with an explicit, recorded decision on the side file:

- **Option (a) — resolved at the Step 2 decision gate.** `script/get_cheri_riscv64_func_inst.sh` performs a CHERI-capable extraction (the CTSRD SDK's `riscv64cheri-objdump`, a wrapper over `llvm-objdump`, supplied through the published `OBJDUMP` knob), so "Adding an instruction set" step 4 in `doc/porting.html` keeps its expectation that each port provides an extractor.
- **Option (b) — dismissed at the gate, not a live alternative.** It would have changed the cheri verdict from the declared 290 to 2 (`acc/read-func.cpp` returns 2 in `main` before `helper()` when the side file is absent, and the capability fault arises only inside `helper()`), i.e. it would have turned the defense into a failure report. It is recorded here only as the rejected option.

The plan is **not** identical under the two options — (b) would drop `script/get_cheri_riscv64_func_inst.sh`, sever the `.gen`/`.tmp` dependency, and change Steps 3–5 — which is part of why the gate settled on (a).

**Env residual (successor cycle, named):** why the per-lane shim did not take effect for those four runs remains open (the deferral's env question). The bench fix makes the shim irrelevant for extraction, so the successor env cycle only needs: an **audit line** (each lane logs the resolved `objdump --version`/path at run start — the shim's application becomes observable), a bounded investigation of the shim miss, then the re-bump + re-run of all seven lanes to a uniform pin. This cycle's own acceptance is in-repo build/extraction evidence.

**Out of scope:** the return-code-delivery plan (bench; the next comparability workstream) and the NEMU venue plan (`2026-10-11-riscv64-nemu-linux-platform.md`; the final-aim workstream); C2/C3 (env); the env lane scripts entirely.

## Repo Mode

Public

## Document Overrides

| Document | Rule Overridden | Reason |
|----------|----------------|--------|

## Risks

| Risk | Impact | Mitigation |
|------|--------|------------|
| Routing through `$(OBJDUMP)` changes the default path for callers who never set it | silent x86_64 extraction behavior change | `${OBJDUMP:-objdump}` keeps the bare default identical when unset; the fixture suite covers x86_64 native before/after |
| A wrong-but-well-formed side-file word on CHERI is invisible at run time (the capability fault precedes the comparison, so the verdict is 290 whatever the word says) | a bad extraction could look like the declared defense | the extractor asserts exact width and hex shape before writing the side file; Step 3's CHERI fixture recomputes the word independently. The run-time insensitivity itself is **declared**, not claimed fixed — the value's correctness is carried at extraction time |
| The extractor invokes an externally supplied tool twice per extraction with no bound of its own | a hung tool stalls the build and then the scheduler (`run-test.cpp`'s `run_cmd` has no timeout) | every objdump call goes through a bounded runner (`timeout 30` where coreutils is present). Residual: a stock macOS without `timeout` falls back to an unbounded call — declared in Step 1 |
| The `cheri_riscv64` branch is selected before `enable_riscv64_cheri` forces `ARCH` (Makefile parse order) | a flag-only invocation silently picks the x64 extractor — the exact defect this plan fixes | Step 2 folds the flag into the selection and the verify dry-runs the flag-only invocation |
| The Windows `.bat` counterpart diverges from its `.sh` siblings | drift | pattern-parity edit + grep check only; no Windows test path exists from this host (as recorded by the sweep) |

## Proposed Changes

1. **Step 1:** Route the extraction through the published knob: the `Makefile` opcode rule passes `$(OBJDUMP)` into the `$(func-opcode-gen)` invocation, and the extractors consume it. x86_64 keeps `${OBJDUMP:-objdump}` (bare `objdump` *is* the native tool there); the three cross-target extractors require the knob explicitly and fail loudly when it is unset, because a PATH-resolved `objdump` on those targets is the host's own tool — the defect this plan fixes. Keeping every argument, the anchored symbol match, the exactly-one-address check and the hard no-opcode error intact, each script additionally: echoes the resolved tool and its `--version` (the audit line the successor env cycle consumes); runs every objdump call through a bounded runner (`timeout 30` where coreutils is present, unbounded otherwise — the stock-macOS residual is declared); names the resolved tool in its hard-error messages; and asserts the extracted word is exactly `2*byte_nums` lowercase hex digits before writing the side file (the consumer parses any width, so a partial listing from a tool that exits 0 would otherwise be accepted silently). The same pattern edit applies to `get_x64_func_inst.bat` where applicable.
   → coder verify (auto): `grep -n 'OBJDUMP' script/get_*_func_inst.sh Makefile` shows the routing; with `OBJDUMP=/nonexistent-objdump` an extraction exits non-zero naming the tool; with `OBJDUMP` unset the three cross extractors fail loudly naming the knob while the x86_64 one still extracts; an extraction asked for more bytes than the region holds (`byte_nums=64`) fails loudly with the width message instead of writing a short word; with the correct cross objdump it extracts exactly one word
2. **Step 2:** Add the `cheri_riscv64` branch to the extractor selection and implement the gate's decision — **resolved to (a)**, (b) dismissed (see Summary). The branch must key on the **effective** ARCH: `enable_riscv64_cheri` forces `ARCH := cheri_riscv64` further down the Makefile, *after* this selection is evaluated, so a flag-only invocation would otherwise pick the x64 extractor against an xcheri binary — fold the flag in at the selection (`func-opcode-arch := $(if $(enable_riscv64_cheri),cheri_riscv64,$(ARCH))`, selecting on that). `script/get_cheri_riscv64_func_inst.sh` performs the CHERI-capable extraction through the published `OBJDUMP` knob and fails loudly when that tool cannot decode the binary.
   → coder verify (auto): `make -n -B enable_riscv64_cheri=yes test/acc-read-func.gen` shows `script/get_cheri_riscv64_func_inst.sh` as the extraction step with **no** `ARCH=` on the command line; `make -n -B ARCH=cheri_riscv64 enable_riscv64_cheri=yes test/acc-read-func.gen` shows the same; `make -n -B test/acc-read-func.gen` still shows the x64 extractor; `git diff Makefile` touches only the selection block and the side-file rule; the aarch64/riscv64 selections are byte-identical to before
3. **Step 3:** Prove the fix end-to-end in-repo (tester; fixtures in Guidance for Tester): the three per-platform extractions produce the correct word, each validated against a value computed **independently of the installed extractor**, the negative controls fail loudly (nonexistent tool; `OBJDUMP` unset on a cross target; an extraction asking for more bytes than the region holds — the width guard), and the CHERI extraction under option (a) is proven with the env's SDK toolchain paths, the word recomputed independently of the extractor. Declare the boundary: on CHERI the case's verdict does not depend on the side-file value (the capability fault precedes the comparison), so the extraction layer — the width/shape assertion plus the independent recomputation — carries the correctness proof there.
   → tester verify: fixtures pass; the sweep's collision fixture stays green as a regression; the aarch64-morello combination extracts correctly
4. **Step 4:** Validation summary + handoff (tester): record the exact tool+version each platform's extraction used (the audit lines), the guard evidence (each negative control, including the width-guard case), the CHERI side-file declaration (including the run-time-insensitivity boundary), and the successor-cycle requirement (env re-bump + seven-lane re-run + the shim audit); state that real-lane confirmation is the successor's.
   → tester verify: the report carries the tool table and the boundary statement; nothing claims lane-level green
5. **Step 5:** Docs + retirement (planner): `doc/porting.html` — note that the opcode extraction honors the `OBJDUMP` knob (the "Adding an operating system" step 2 knob), and, under "Adding an instruction set" step 4, that the `cheri_riscv64` port requires a **CHERI-capable objdump** (the CTSRD SDK's `riscv64cheri-objdump`, a wrapper over `llvm-objdump`) with the extractor failing loudly on a tool that cannot decode the binary; the option-(b) "side file not produced" declaration **no longer applies** (the gate resolved to (a)). The `acc/read-func.cpp` parse check changes no result-code contract (code 1 was already this case's predefined error number), so no `README.md` / `doc/architecture.html` edit. Keep `benchmark/CLAUDE.md`'s "Helper scripts" paragraph true: it enumerates the extractors by name, so `get_cheri_riscv64_func_inst.sh` joins that list (and the sentence notes the extraction consumes the `$(OBJDUMP)` knob). **Hold the retirement of both cross-repo deferrals**, and record the protocol explicitly: the confirmation they wait on is the successor env cycle's lane re-run at this plan's commit; per `report-schemas.md`'s cross-repo protocol the deletion itself is executed by the consuming repo's `/update-and-commit` Step 9.9c (verify-then-delete in the origin repo), while the env planner's note directs the env cycle to perform it — the deviation is recorded deliberately, and whichever session executes the deletion is the one whose Issue Log carries the resolution. No second plan is opened for it. The hold is recorded in `## Final Results`.
   → Evidence: porting.html diff exists; the CLAUDE.md helper-scripts list names all four extractors; the explicit hold and its protocol note are recorded in Final Results

## Guidance for Planner

Documentation tasks you will execute yourself (not the coder):

- **Files to modify:**
  - `./doc/porting.html`
  - `./CLAUDE.md` (the "Helper scripts" paragraph enumerates the extractors by name)
- **When:** after Step 3 (before final revision).
- **What to sync:** the OBJDUMP-knob routing sentence (both keep-in-sync sections: "Adding an instruction set" step 4 — the per-ISA extractor now consumes `$(OBJDUMP)`; "Adding an operating system" step 2 — the knob now covers extraction too); under "Adding an instruction set" step 4, the `cheri_riscv64` tool requirement (a CHERI-capable objdump — the SDK's `riscv64cheri-objdump` — with the extractor failing loudly otherwise) and the guard family (exact-width assertion, cross targets require the knob explicitly); `./CLAUDE.md`'s "Helper scripts" paragraph gains `script/get_cheri_riscv64_func_inst.sh` in its extractor list and notes the knob routing. Do **not** write the option-(b) "side file not produced" declaration: the gate resolved to (a). Explicitly **no change required** (recorded so the omission is deliberate): `./README.md` (it enumerates directories and result codes, none of which this plan touches) and `./doc/architecture.html` (its extraction reference is the generic `get-code-offset` field description; the result-code contract and `configure.json`'s field set are untouched). Verify no existing porting note contradicts.
- **Decision gate (Step 2):** **resolved to (a)** at the gate; (b) was dismissed because it changes the cheri verdict 290 → 2 (see Immediate Actions and History).
- **Deferral retirement — HOLD (required), with the protocol recorded:** the two env-filed reports (`defer-issue-aarch64-func-opcode-extraction-host-objdump.json`, `defer-issue-cheri-riscv64-opcode-extraction-deploy-abort.json`) stay **Open** through this plan's commit — the confirmation they wait on is the successor env cycle's lane re-run at this plan's commit. The env planner's note (`./tmp/reports/2026-10-19-func-opcode-extraction-platform-fix-env-planner-note.md`) directs the **env cycle** to retire them; `report-schemas.md`'s cross-repo protocol instead has the **consuming repo's** `/update-and-commit` Step 9.9c perform the verify-then-delete in the origin repo. That deviation is recorded deliberately: neither side may retire them before the lane confirmation exists, and whichever session executes the deletion is the one whose Issue Log carries the resolution — no second plan is opened for it. Record the hold and this note in `## Final Results`; mark the plan's Issue Log rows Resolved only once the cycle reports.
- **Successor (name it in Final Results):** env cycle (id `2026-10-19-cheri-lane-cheri-capable-objdump`, drafted env-side in parallel) = supply the CHERI-capable objdump from both env spots (a `find_sdk_objdump` mirroring `find_sdk_clang`; the tool `<env-root>/build/cheri/output/sdk/bin/riscv64cheri-objdump`), fix the two stale "GNU format reads FreeBSD ELF" comments, add the per-lane audit line, investigate the shim miss, bump `BENCH_COMMIT` to **this plan's commit**, re-run `cheri_riscv64` + the four aarch64 lanes (aarch64 hosted by the user — the session sandbox hides `binfmt_misc`), then retire the two env-filed reports. `riscv64` and `aarch64-morello` are **not** re-run (they are green at `c8c6000`).

## History

| Date | Event | Conclusion | Dismissed |
|------|-------|------------|-----------|
| 2026-10-19 | Initial plan | Extraction-path platform fix drafted from the two pin-bump deferrals; one decision (cheri extraction vs skip, default skip-with-declaration) and one env successor named. | — |
| 2026-10-19 | Revision (operating session) | Step 2's gate resolved to option (a) — the default (b) would change the cheri verdict 290 → 2; `acc/read-func.cpp` parse check user-authorized and applied; `Makefile` routing applied; five `script/` bodies staged and verified for the coder round; Windows/MSVC verification and the env's CHERI-objdump supply deferred (two Issue Log rows). | Plan's (b) default and its "capability-mode early exit before the side file" rationale |
| 2026-10-19 | Env planner's note received and folded in | Step 5's retirement is now a required **hold** (the successor env cycle retires the two env-filed reports cross-repo — its note is on file); the successor's scope corrected (cheri + the four aarch64 lanes; riscv64 and aarch64-morello not re-run); Step 5's porting.html text and the Step 3 CHERI fixture's tool (`riscv64cheri-objdump`) adapted to the note | — |
| 2026-10-19 | Mega-audit (pre-implementation) | 35 findings — 1 High, 16 Medium, 18 Low; every High + Medium fixed in this revision: the effective-ARCH fold so a flag-only invocation reaches the cheri extractor, Step 2's verify retargeted to `test/acc-read-func.gen`, the exec-bit acceptance check plus the recorded user action, the cross extractors' explicit-knob requirement, the bounded objdump runner, tool-naming in the hard errors, the exact width/shape assertion before the side file is written, the separator-independent CHERI byte parse, the `main` `argc` guard, the Summary/Risks option-(b) cleanup, the retirement-protocol reconciliation, and the CLAUDE.md / README.md / architecture.html doc accounting. Report: [mega-audit-2026-10-19](../reports/2026-10-19-func-opcode-extraction-platform-fix-mega-audit-2026-10-19.json) | Lows acknowledged, no fix required: [ma-4](../reports/2026-10-19-func-opcode-extraction-platform-fix-mega-audit-2026-10-19.json), [ma-5](../reports/2026-10-19-func-opcode-extraction-platform-fix-mega-audit-2026-10-19.json), [ma-8](../reports/2026-10-19-func-opcode-extraction-platform-fix-mega-audit-2026-10-19.json), [ma-9](../reports/2026-10-19-func-opcode-extraction-platform-fix-mega-audit-2026-10-19.json), [ma-10](../reports/2026-10-19-func-opcode-extraction-platform-fix-mega-audit-2026-10-19.json), [ma-11](../reports/2026-10-19-func-opcode-extraction-platform-fix-mega-audit-2026-10-19.json), [ma-12](../reports/2026-10-19-func-opcode-extraction-platform-fix-mega-audit-2026-10-19.json), [ma-13](../reports/2026-10-19-func-opcode-extraction-platform-fix-mega-audit-2026-10-19.json), [ma-17](../reports/2026-10-19-func-opcode-extraction-platform-fix-mega-audit-2026-10-19.json), [ma-18](../reports/2026-10-19-func-opcode-extraction-platform-fix-mega-audit-2026-10-19.json), [ma-19](../reports/2026-10-19-func-opcode-extraction-platform-fix-mega-audit-2026-10-19.json), [ma-20](../reports/2026-10-19-func-opcode-extraction-platform-fix-mega-audit-2026-10-19.json), [ma-21](../reports/2026-10-19-func-opcode-extraction-platform-fix-mega-audit-2026-10-19.json), [ma-27](../reports/2026-10-19-func-opcode-extraction-platform-fix-mega-audit-2026-10-19.json), [ma-28](../reports/2026-10-19-func-opcode-extraction-platform-fix-mega-audit-2026-10-19.json), [ma-29](../reports/2026-10-19-func-opcode-extraction-platform-fix-mega-audit-2026-10-19.json), [ma-31](../reports/2026-10-19-func-opcode-extraction-platform-fix-mega-audit-2026-10-19.json), [ma-32](../reports/2026-10-19-func-opcode-extraction-platform-fix-mega-audit-2026-10-19.json) |

| 2026-10-19 | Spec revision (/update-plan) — post-coder round | Coder round `build_result: pass` ([report](../reports/2026-10-19-func-opcode-extraction-platform-fix-coder-2026-10-19.json)): five bodies installed, effective-ARCH fold applied, `argc` guard added; the three filed issues classified — the stale staging header and the harness reference leg are Fix Planned (Immediate Actions), the pre-existing Makefile parse-order residual is deferred ([defer-issue-makefile-arch-parse-order-residual](../reports/defer-issue-makefile-arch-parse-order-residual.json)); Step 3 and Guidance for Tester now require independent word validation on all four platforms | — |

| 2026-10-19 | Spec revision (/update-plan) — post-tester round | Tester report **SUCCESS** ([report](../reports/2026-10-19-func-opcode-extraction-platform-fix-tester-2026-10-19.json)): 7/7 fixture checks, full suite 246/246 (0 skips, 0 build failures), `acc-read-func` 1 → 0 is the only result-code change against the 2026-10-13 reference, all four platform words validated by a recomputation independent of the installed extractor (plus the aarch64-morello control); `handoff-reference-leg-post-install` is **Resolved** (HEAD-sourced reference leg + the tester's post-install fixtures), and the tester's one abnormal result — the pre-existing 255 expectation gap — is deferred as [bench-expectation-gap-cfi-fakefunc-255](../reports/defer-issue-bench-expectation-gap-cfi-fakefunc-255.json); the handoff's stale CHERI constant is corrected | — |

| 2026-10-19 | Spec revision (/update-plan) — post-coder round 2 | Comment-only fix applied: the stale staging header is gone from `script/get_cheri_riscv64_func_inst.sh` (2-line pointer replaces it; `bash -n` clean; no remaining `STAGED DRAFT`/`write-denied` markers across `script/`, `Makefile`, `acc/`, `lib/`, `scheduler/`); no issues filed ([report](../reports/2026-10-19-func-opcode-extraction-platform-fix-coder-2026-10-19-2.json)). `cheri-extractor-stale-header` stays Fix Planned pending the tester's behavioral re-confirmation | — |

| 2026-10-19 | Spec revision (/update-plan) — post-tester round 2 — finalize path | Tester re-run **SUCCESS** ([report](../reports/2026-10-19-func-opcode-extraction-platform-fix-tester-2026-10-19-2.json)): the comment-only header fix is behaviourally inert — 7/7 fixture checks, every word identical to the prior run, all negative controls loud, the sweep collision fixture 36/36, the suite at 246/246 (0 skips, 0 build failures), only `acc-read-func` 1 → 0 against the 2026-10-13 reference. `cheri-extractor-stale-header` **Resolved**; the two env-filed rows re-marked as deferred (hold); no in-scope issues remain — the Phase 6 final review is next | — |

| 2026-10-19 | Review (Phase 6) | Adversarial code-quality review complete: verdict **Issues found — non-blocking** — 4 findings, all Suggestion band, no Critical and no Warning; every changed file read in full, the extraction logic traced correct on all four platforms, the four installed `.sh` bodies and the `.bat` byte-compared against the staged drafts (diff empty), the effective-ARCH fold and the `acc/read-func.cpp` guards verified, the planner's doc-structure Evidence re-run to PASS ([report](../reports/2026-10-19-func-opcode-extraction-platform-fix-review-2026-10-19.json)). `## Final Results` written: **COMPLETED**, with both local deferrals, the two held env-filed reports and the retirement-protocol note recorded | Four Suggestions acknowledged: (1) the cross extractors' explicit-knob guard never fires on the make path — the Makefile's `OBJDUMP := objdump` default supplies the host tool; outcome stays safe (loud failure via the rc check + width assertion, never a wrong word), so it is an intent/documentation gap; (2) the side-file rule re-extracts on an in-place `OBJDUMP` switch only under `-B` (the scheduler path uses `make -B`; same shape as the pre-existing `dump` target); (3) the ~28-line objdump preamble is duplicated across the four extractors — factoring it into a sourced helper is a candidate for a future cleanup plan, not this one; (4) the empty `## Final Results` placeholder cited by Step 5's Evidence line — **addressed here**: the section is now written, satisfying the citation. Hunches: the flag-only `enable_aarch64_morello` residual (**already** the open `makefile-arch-parse-order-residual` deferral), extractors not removing the output on failure (mega-audit Low, acknowledged), the CHERI wrapper's identity being env-side, the `$^` positional-argument fragility, and the lanes' dependency on the env successor (the declared boundary) |

## Plan Metadata

```json
{
  "plan_id": "2026-10-19-func-opcode-extraction-platform-fix",
  "steps": ["Step 1: Route the extraction through $(OBJDUMP)", "Step 2: Add the cheri_riscv64 extraction branch (decision gate)", "Step 3: Prove the fix end-to-end in-repo (tester)", "Step 4: Record the tool table and the successor handoff (tester)", "Step 5: Sync porting.html and dispose the deferrals (planner)"],
  "coder_files": ["./Makefile", "./acc/read-func.cpp", "./script/get_x64_func_inst.sh", "./script/get_aarch64_func_inst.sh", "./script/get_riscv64_func_inst.sh", "./script/get_cheri_riscv64_func_inst.sh", "./script/get_x64_func_inst.bat"],
  "tester_files": ["./tmp/verification/2026-10-19-func-opcode-extraction-platform-fix-fixtures.sh", "./tmp/verification/2026-10-19-func-opcode-extraction-platform-fix-cheri.sh"],
  "doc_files": ["./doc/porting.html", "./CLAUDE.md"],
  "verification_scripts": [],
  "repo_mode": "Public",
  "document_overrides": []
}
```

## Audit Accumulation

| Counter | Value | Last Updated |
|---------|-------|--------------|
| issues_resolved_since_audit | 2 | 2026-10-19 |
| steps_changed_since_audit | 1 | 2026-10-19 |
| files_changed_since_audit | 0 | 2026-10-19 |

## Documentation

Documentation owned by the planner: `./doc/porting.html` — **applied this session**: the
"Adding an instruction set" step 4 now says the extractor consumes `OBJDUMP` and records the
`cheri_riscv64` tool requirement (a CHERI-capable objdump, the SDK's `riscv64cheri-objdump`), and
"Adding an operating system" step 2 notes the knob drives the extraction too; the page's
`last-modified` meta and visible date are bumped to 2026-10-19. Gate evidence:
`doc_structure_check.py --mode public` → PASS, 5 files, no violations. `./CLAUDE.md` also gains
`script/get_cheri_riscv64_func_inst.sh` in its "Helper scripts" extractor list (mega-audit finding
#33). No `README.md` or `doc/architecture.html` change — recorded as deliberate, since neither
enumerates the extractors, the record count, or the result-code contract (mega-audit findings #34,
#35). The two cross-repo reports' retirement is **held**, with the protocol note from Guidance for
Planner, for the successor env cycle.

## Final Results

**Status:** COMPLETED
**Completed:** 2026-10-19

### What was implemented

The `acc-read-func` opcode extraction now follows the platform, on both defect fronts the pin bump exposed:

1. **`$(OBJDUMP)` routing.** The `test/acc-read-func-func-opcode.tmp` rule passes the published knob into the extractor invocation (`OBJDUMP="$(OBJDUMP)" $^ helper 8 $@`; the Windows branch mirrors it with `set "OBJDUMP=$(OBJDUMP)"`). The POSIX extractors consume it: x86_64 keeps `${OBJDUMP:-objdump}` (bare `objdump` *is* the native tool there); aarch64/riscv64/cheri_riscv64 require the knob explicitly and fail loudly naming it — a PATH-resolved `objdump` on those targets is the host's own tool, the defect that turned `acc-read-func` into `1025` on all four aarch64 lanes.
2. **`cheri_riscv64` extraction branch.** The selection gains its own branch keyed on the **effective** ARCH (`func-opcode-arch := $(if $(enable_riscv64_cheri),cheri_riscv64,$(ARCH))`, so a flag-only invocation reaches it) and runs the new `script/get_cheri_riscv64_func_inst.sh` — the CHERI-capable extraction resolved at the Step 2 gate as **option (a)**. Option (b) was dismissed: it would have changed the case's declared 290 verdict into 2.
3. **Extractor hardening.** Bounded objdump runner (`timeout 30` where coreutils is present; the stock-macOS residual is declared), rc-checked symbol-table/disassembly calls (a tool that exits non-zero while printing a mis-decoded listing must not feed it into the parse), tool-naming hard errors, an exact-width/lowercase-hex assertion on the extracted word before the side file is written, and an audit line echoing the resolved tool + version for the successor cycle. The CHERI byte-column parse is separator-independent (llvm-objdump's memory-order bytes reversed to the GNU word-value convention the consumer expects). `acc/read-func.cpp` gains the parse check (unparseable/empty side file → 1, before `helper()`, so a failed extraction can never be masked as the declared CAP fault) and the `main` `argc` guard (no argument → 2, instead of dereferencing a NULL `argv[1]`). **No result-code contract change**: 1 and 2 were already this case's predefined codes.

### Files changed

| File | Change |
|---|---|
| `Makefile` | `OBJDUMP` pass-through in the side-file rule (POSIX + Windows branches); `cheri_riscv64` extractor selection keyed on the effective ARCH |
| `acc/read-func.cpp` | parse check (empty/malformed side file → 1) + `argc` guard (→ 2) |
| `script/get_x64_func_inst.sh` | knob consumption, bounded runner, rc checks, width assertion, audit line |
| `script/get_aarch64_func_inst.sh` | same + explicit-knob requirement |
| `script/get_riscv64_func_inst.sh` | same + explicit-knob requirement |
| `script/get_cheri_riscv64_func_inst.sh` | **new** — CHERI-capable extraction (llvm-objdump byte column → little-endian word), explicit-knob requirement |
| `script/get_x64_func_inst.bat` | `%OBJDUMP%` (default `dumpbin.exe`) + tool echo; pattern parity only — Windows verification deferred |
| `doc/porting.html` | step 4 (instruction set): the extractor consumes `OBJDUMP` + the `cheri_riscv64` tool requirement; step 2 (operating system): the knob drives extraction too |
| `CLAUDE.md` | "Helper scripts" paragraph names all four extractors, the knob routing and the width assertion |

### Test results

Tester round 1 ([report](../reports/2026-10-19-func-opcode-extraction-platform-fix-tester-2026-10-19.json)) — **SUCCESS**: 7/7 fixture checks; the four platform words validated by a recomputation independent of the installed extractor (x86_64 `35058b48fa1e0ff3`, aarch64 `910b800090000000`, riscv64 `0006869300000697`, cheri_riscv64 `df05250f00007517`, plus the aarch64-morello unaffected control); negative controls loud (nonexistent tool, knob unset on a cross target, `byte_nums=64` width guard); full suite **246/246** (0 skips, 0 build failures), `acc-read-func` 1 → 0 the only result-code change against the 2026-10-13 x86_64 reference.

Tester round 2 ([report](../reports/2026-10-19-func-opcode-extraction-platform-fix-tester-2026-10-19-2.json)) — **SUCCESS** (after the comment-only header fix): 7/7 checks, every word identical to round 1, all negative controls loud, the sweep's collision fixture 36/36, suite 246/246.

**Boundary:** the four aarch64 lanes' and the cheri_riscv64 lane's **real-lane** confirmation is the successor env cycle's; this plan's evidence is build/extraction level on this host. On CHERI the case's run-time verdict does not depend on the side-file value (the capability fault precedes the comparison), so the extraction layer — width/shape assertion plus independent recomputation — carries the correctness proof there; that run-time insensitivity is **declared**, not claimed fixed.

### Issues resolved

- `handoff-reference-leg-post-install` — **Resolved**: the harness check scripts' reference leg now reads the pristine pre-install bodies via `git show HEAD:script/...` (re-run green, 21/21 and 6/6), and the tester's post-install fixtures validate the installed state independently of the harness.
- `cheri-extractor-stale-header` — **Resolved**: coder round 2 replaced the staging header with a 2-line pointer (comment-only); the tester's re-run confirms it is behaviourally inert.

### Deferred issues (open, by design)

Each is a *completed* disposition — correctly identified and correctly scoped out — not unfinished work.

- **`makefile-arch-parse-order-residual`** — report: `./tmp/reports/defer-issue-makefile-arch-parse-order-residual.json`. Pre-existing Makefile parse-order defect: `enable_riscv64_cheri` / `enable_aarch64_morello` force `ARCH` *after* earlier ARCH consumers are evaluated; this plan folds the flag into the extractor selection only. Fixing the rest changes which objects a flag-only invocation builds and needs its own verification. (The reviewer's flag-only `enable_aarch64_morello=yes` hunch lands here — already covered.)
- **`bench-expectation-gap-cfi-fakefunc-255`** — report: `./tmp/reports/defer-issue-bench-expectation-gap-cfi-fakefunc-255.json`. Pre-existing registry expectation gap (`cfi-return-to-parent-wrong-call-site-fakefunc` → 255, undeclared in its `expect-results`); unchanged by this plan, and the authoritative tracker is the env-side `2026-10-12-test-env-all-archs-issue-review-lines-bench-expectation-gaps.json`. The local report keeps the deferral visible in this repo without double-filing.
- **`windows-msvc-objdump-passthrough-verification`** — report: `./tmp/reports/defer-issue-windows-msvc-objdump-passthrough-verification.json`. The Windows/MSVC half of the pass-through (`script/get_x64_func_inst.bat` + the MSVC recipe line) has no test path from this host; deferred at the user's request. Retires on a Windows/MSVC run of `make test/acc-read-func.gen` (or the scheduler-driven build) showing dumpbin invoked through `%OBJDUMP%`, producing the correct 16-hex-digit word, with the `dumpbin.exe` default holding when unset.
- **`cheri-riscv64-needs-cheri-capable-objdump`** — cross-repo (filed here, `target_repo: cpu-sec-test-env`): the env must pass the CHERI-capable objdump as `OBJDUMP` for the cheri lane (both env spots); until then the cheri corpus build stops loudly at the extraction, naming the tool it tried.

### Retirement hold (recorded, deliberate)

The two env-filed cross-repo reports — `defer-issue-aarch64-func-opcode-extraction-host-objdump.json` and `defer-issue-cheri-riscv64-opcode-extraction-deploy-abort.json` — stay **Open** through this plan's commit: the confirmation they wait on is the successor env cycle's lane re-run **at this plan's commit**.

**Protocol note (recorded deviation).** `report-schemas.md`'s cross-repo protocol has the *consuming* repo's `/update-and-commit` Step 9.9c perform the verify-then-delete in the origin repo; the env planner's note (`./tmp/reports/2026-10-19-func-opcode-extraction-platform-fix-env-planner-note.md`) instead directs the **env cycle** to retire them. Neither side may retire them before the lane confirmation exists; the deviation is recorded deliberately, and whichever session executes the deletion is the one whose Issue Log carries the resolution. No second plan is opened for it. Consequently this plan's `/update-and-commit` **retires nothing** — both held reports stay on disk.

### Review

Phase 6 adversarial review ([report](../reports/2026-10-19-func-opcode-extraction-platform-fix-review-2026-10-19.json)) — verdict **Issues found — non-blocking**: 4 findings, all Suggestion band, no Critical and no Warning. The reviewer read every changed file in full, byte-compared the installed bodies against the staged drafts, traced all four platform paths (correct), and re-ran the planner's doc-structure Evidence to PASS; its residual risk sits with the declared env successor, not this diff. Dispositions: the knob-guard/intent gap and the `-B`-only re-extraction on a knob switch are acknowledged (both outcomes stay safe: loud failure, never a wrong word); the cross-extractor preamble duplication is noted as a future cleanup candidate; and the empty `## Final Results` placeholder a Step 5 Evidence line cited is **satisfied by this section**.

### Successor

Env cycle **`2026-10-19-cheri-lane-cheri-capable-objdump`** (drafted env-side in parallel): supply the CHERI-capable objdump from both env spots (a `find_sdk_objdump` mirroring `find_sdk_clang`; the tool `<env-root>/build/cheri/output/sdk/bin/riscv64cheri-objdump`), fix the two stale "GNU format reads FreeBSD ELF" comments, add the per-lane objdump audit line, investigate the shim miss, bump `BENCH_COMMIT` to **this plan's commit**, re-run `cheri_riscv64` + the four aarch64 lanes (aarch64 hosted by the user — the session sandbox hides `binfmt_misc`), then retire the two env-filed reports. `riscv64` and `aarch64-morello` are **not** re-run (green at `c8c6000`).