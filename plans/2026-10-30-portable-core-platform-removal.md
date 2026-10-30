# Plan: Make the benchmark a portable core — delete the embedded venue, move the CHERI build skips into the registry
**Plan ID:** 2026-10-30-portable-core-platform-removal
**Created:** 2026-10-30

## Issue Log

| Issue | Status | Found | Resolved | Resolved By |
|-------|--------|-------|----------|-------------|

No deferred issues are adopted by this plan. The eight open bench deferrals (`argv-conv-bounds`, `cheri-evidence-unavailable`, `dangling-require-names`, `internal-md-drift`, `makefile-arch-parse-order-residual`, `mss-rv-uninitialised`, `nonalloc-redzone-distance-written-before-match`, `windows-msvc-objdump-passthrough-verification`) are unrelated and stay untouched.

## Guidance for Coder

**Files to modify:**

- `./Makefile`
- `./configure.json`
- `platform/` (whole tree deleted — see Step 3)
- `./.gitignore`

**Constraints:**

- This is the bench repo (Public). Commit house style matches `f9e4ba7`: imperative subject, body bullets, `Tested:` and `Plan: ./plans/<plan-id>.md` footers, no attribution trailers — consistent with every prior bench commit.
- Never edit `plans/` (the archived design record, including `plans/content.json`), `INTERNAL.md` (outside the rounds' write scope), or `doc/**` / `README.md` / `CLAUDE.md` (planner-owned; see Guidance for Planner).
- No lane runs, no submodule operations, no result-code minting. The registry edit adds only `skip` / `skip-reason` — the exit-code contract is untouched.
- Run `make clean` before leaving the tree if `make` was invoked, so generated artifacts are never mistaken for committed ones.

**Step-by-step with verification:** the numbered steps live in `## Proposed Changes` below. `(auto)` checks are self-verified; `(scripted)` checks run the provided script under `./tmp/verification/`.

## Guidance for Tester

**Tests to create:**

- **Temporary** (all under `./tmp/verification/2026-10-30-portable-core-platform-removal/`, all Python, deleted after plan completion):
  - `skip-matrix.py` — pure-data check simulating BOTH scheduler behaviors: direct skips (`run-test.cpp:305-313`: skipped iff the record's `skip` list contains the baked `CSB_TARGET_ARCH`) AND the transitive require closure (a skipped prerequisite's 1025 satisfies no dependent group → dependent ends 1024). Assert the exact per-token outcomes: `x86_64`/`aarch64`/`riscv64` → zero of either; `aarch64-morello` → direct {`cfi-call-mid-func`}, indirect {`cfi-call-instruction-in-rodata`, `-in-data`, `-in-stack`, `-in-heap`}; `cheri_riscv64` → direct {`acc-get-frame-size`, `mss-write-by-stack-pointer`}, indirect {`cfi-return-to-parent-same-call-site`, `-diffargs`, `cfi-return-without-call`}. Malformed JSON exits 2, never a traceback.
  - `registry-shape.py` — diff-shape check (exactly the three records of Step 2 gained exactly `skip` + `skip-reason`; no other record or key changed; `expect-results` untouched) PLUS a vocabulary guard: every `skip` token belongs to the closed set `{x86_64, aarch64, aarch64-morello, riscv64, cheri_riscv64}` — a typo'd token silently matches nothing and would surface only as a 1025-with-make-status indistinguishable from a build regression. (An *empty* `CSB_TARGET_ARCH` stays legitimate by design — a hand-built scheduler's skips are inert, `run-test.cpp:309` — so the guard lives on the registry side, never on the make side.)
  - `token-baking.py` — runs the three `make -nB` probes of Step 1 via subprocess and asserts each fixed-string token (`CSB_TARGET_ARCH="\"<expected>\""`) appears exactly once.
  - `reference-clean.py` — two modes. **Tester-round mode** (default, after the coder): `git ls-files platform` is empty; `.gitignore` no longer names `platform`; and the tracked `platform/` hits outside `plans/` equal exactly the expected post-coder set {`CLAUDE.md:59`, `CLAUDE.md:101`, `CLAUDE.md:221`, `README.md:74`, `.claude/mega-audit-files.json:3`} — the planner-owned files whose sync comes after this round. **Strict mode** (`--strict`, the final post-planner acceptance gate before commit): `git grep -n "platform/"` over tracked files returns hits **only** under `plans/` (the archive is history and must not be edited).

**Tests to investigate for retirement:**

- Planner candidates: none — this plan removes no test, case or record; `script/check_catalog.py` assertions are unaffected (name set and count sentences unchanged).
- Tester may identify additional candidates during test work.

**Retirement procedure:** for each candidate, verify the test exercises code being removed or changed. If obsolete: delete and note in session report. If still valid: document why in session report.

**Scope note:** this cycle runs NO lanes and NO guest. The end-to-end proof that corpus staging plus the declared skips still reproduce the standing 246-record distributions belongs to the successor environment cycle (pin bump, venue absorption, staging swap, re-stamp of riscv64 + riscv64-nemu, comparison gate).

## Summary

**Problem.** The bench repo bills itself as a portable security test core, but it carries an embedded venue: `platform/riscv64-nemu-linux/` (10 tracked files — staging, image assembly, guest boot package, NEMU pins, venue README) exists to run the suite in a booted Linux guest under XiangShan NEMU. Meanwhile the knowledge "these three programs cannot build on the CHERI cross-toolchains" lives **outside** the bench, in the environment repo's `scripts/common/derive_registry.py` `SKIP_TABLE`, applied by rewriting a derived registry at deploy time. The user has ruled the target architecture: the benchmark is the portable core (sources, registry, support library, runner, documented contracts); everything platform-special belongs to the environment repo. An eventual in-guest compiler under NEMU (the SD-card rootfs route) would make the embedded venue obsolete anyway — it is temporary, and it goes now.

**Approach.** Three moves, one commit:

1. Delete `platform/` outright. Its generic content needs no bench-side successor: the corpus-mode split-build protocol it consumed is already documented as a runner feature in `doc/architecture.html`, and the successor environment cycle re-homes the venue's boot/export package into the env lane that already drives it (`scripts/riscv64_nemu/`).
2. Move the three build-skip declarations into the tracked `configure.json` as real `skip` / `skip-reason` rows — the scheduler's native skip machinery (`run-test.cpp:305-313`, recorded as 1025 "not run (skipped for this platform)", preceding build and run) then serves every venue without a derived registry. Critically, a skip is what keeps corpus mode's run phase from fail-fasting on a never-built unit: a skipped record is neither snapshotted nor executed.
3. Fix the one token collision that makes row (2) possible for Morello: the Morello lane builds with bench `ARCH=aarch64` + `enable_aarch64_morello=yes` (the flag affects the toolchain flags at `Makefile:402-408` and the independent-assembly object selection at `Makefile:265-269` — neither touches the arch token), so today its scheduler bakes `CSB_TARGET_ARCH="aarch64"` — indistinguishable from the healthy pa/bti/mte lanes. A tracked `skip: ["aarch64"]` would wrongly skip `cfi-call-mid-func` on all four healthy aarch64 lanes. The Makefile now bakes a distinct `aarch64-morello` token when the flag is set (spelling already precedented: 20 registry expect-results texts say "seen on aarch64-morello"). `cheri_riscv64` needs no such fix — `enable_riscv64_cheri` already forces `ARCH=cheri_riscv64`.

**Skip cascade (transitive, asserted — not introduced).** A skip propagates through the `require` graph: a skipped prerequisite's 1025 satisfies no dependent group, so its dependents end 1024 (prerequisite unsatisfied). On `aarch64-morello`, `cfi-call-mid-func` is the sole OR-arm of an AND group in `cfi-call-instruction-in-rodata`/`-in-data`/`-in-stack`/`-in-heap` — four indirect 1024s. On `cheri_riscv64`, `mss-write-by-stack-pointer` gates `cfi-return-to-parent-same-call-site` + `-diffargs` (variant 1) and `acc-get-frame-size` gates `cfi-return-without-call` (variant 0) — three indirect 1024s. This is not new behavior: the env's derived-registry skip already produces exactly these outcomes on the CHERI lanes today. The registry now states it declaratively, the docs must not claim "only three records affected", and the tester's matrix asserts the full closure.

**The invariant that survives both repos:** venue staging must *drive the pinned runner's own corpus mode* (`./run-test corpus make-only exhausted-run` / `./run-test no-make corpus exhausted-run`); it must never recompute corpus keys itself. That is what prevents the derive-registry duplication from re-forming once the env repo owns all orchestration. The protocol is bench documentation, not bench code.

**Alternatives considered and rejected:**

- *Generalize in place (`platform/common/` + venue knobs):* keeps venue code and a second pin location inside the portable core for a venue the env repo already drives end-to-end — symmetry with zero functional gain. Rejected by the user's portable-core ruling.
- *Keep `platform/` until an in-guest compiler lands:* the directory is removable now with a two-hour breakage window (below) instead of an indefinite carry; the compiler route is a separate future project and stacks cleanly on this end state.
- *Leave skips in the env's derived registry:* keeps the last fragment of deploy-time registry rewriting alive, exactly the mechanism this architecture retires.

**Out of scope:** the successor environment cycle (pin bump; absorb `platform/riscv64-nemu-linux/`'s guest package into `scripts/riscv64_nemu/`; swap riscv64 + CHERI staging to runner-driven corpus mode; delete `derive_registry.py` + `build_corpus.sh`; move the NEMU pin into env `env.sh`; re-stamps + comparison gate); CHERI lane bring-up; the in-guest-compiler route; the eight open deferrals.

## Repo Mode

Public (detected via `detect_repo_mode.py`, 2026-10-30). Commit messages in the house style of `f9e4ba7`; archival to the tracked `./plans/` directory by `/update-and-commit`.

## Document Overrides

| Document | Rule Overridden | Reason |
|----------|----------------|--------|
| `CLAUDE.md` (## Subagents) | The coder lane boundary enumerates "the test-case sources … plus `lib/`, `scheduler/`, `script/` and the `Makefile`" — `platform/` and `.gitignore` are outside it, yet Steps 3-4 assign both to the coder | Steps 3-4 are the user-approved venue deletion (ruling of 2026-10-30) and its dead-ignore-rule cleanup: repo-config removal with no product-code content. The venue README inside `platform/` is deleted with the tree; its successor text lives in the planner's doc sync |

Beyond this declared departure, zero overrides: every touched claim (`CLAUDE.md` Structure/Commands/Architecture lines naming `platform/` and "the shipped registry declares none"; `README.md` lines 61-76 and 106; `doc/architecture.html` skip-field text; `doc/porting.html` skip guidance) is updated to match the new reality through the normal keep-in-sync path, not overridden.

## Rollback

- **Bench side:** `git revert <commit>` on the bench repo restores `platform/`, the Makefile line and the three registry rows byte-for-byte — the whole change is one commit and there is no data migration.
- **Env side:** re-pin `BENCH_COMMIT` in `env.sh` to the last pre-deletion bench commit; `derive_registry.py` and its `SKIP_TABLE` still exist at that pin, so the CHERI lanes' staging semantics return unchanged.
- **Half-landed successor cycle:** if some env lanes already swapped to runner-driven staging, the bench revert plus the env re-pin restores the old staging for every lane at once; any venue package already absorbed under `scripts/riscv64_nemu/` is inert until referenced and may stay or revert with the env commit.
- The user performs all reverts (push control); the consistent resting pair is env `ef6d7dc` ↔ bench `f9e4ba7`.

## Risks

| Risk | Impact | Mitigation |
|------|--------|------------|
| Cross-repo breakage window: the env lane's `scripts/riscv64_nemu/prepare.sh` + `run.sh` drive `benchmark/platform/riscv64-nemu-linux/…` read-only and break at this commit | The env repo's NEMU lane cannot deploy until its successor cycle lands | Land the two cycles back-to-back; the user holds the bench push until the env cycle is ready. The env plan absorbs the venue package (coder reads the deleted files from the pre-deletion pin via `git show`) |
| The env **Morello** lane breaks at the pin bump by token mismatch — independently of the `platform/` deletion | `derive_registry.py` emits `skip: ["aarch64"]` (`out["skip"] = [args.lane]`, derive_registry.py:177; the lane passes `--lane aarch64 --env-lane aarch64-morello`) while the guest scheduler built with `enable_aarch64_morello=yes` now bakes `aarch64-morello` — the skip stops matching, the intentionally-unstaged unit spawns ENOENT in the guest (`raw 2`, undeclared) and the lane's gate fails | Successor env cycle: delete `derive_registry.py`/`build_corpus.sh` in the **same commit** as the pin bump (or, interim, emit the bench token `aarch64-morello`); include a Morello token/skip-matrix check in its verification; until then the Morello lane's acceptance at the new pin is expected-fail and must not be read as a regression of this plan |
| The three skip rows are a permanent-looking registry contract | If a future CHERI/Morello toolchain pin fixes the underlying build failures, the rows become stale | Each `skip-reason` states the technical cause (undeclared `GET_SP_LOC`; inline-asm constraint `'C'`; LLVM `blockaddress` backend crash), so retirement is checkable — same pattern as the aarch64 99-declaration cycle. Docs describe the rows as current platform facts |
| The `aarch64-morello` token is new vocabulary | Anything in the **bench repo** matching on `CSB_TARGET_ARCH` strings outside `run-test.cpp:305-313` would misbehave | Verified single consumption site within this repo (bake site `Makefile:426`, compare site `run-test.cpp:311` only); default (non-Morello) bakes are byte-identical to today. The env-side hazard is the separate Morello row above |
| Future guest-side CHERI runners must bake matching tokens | A mismatched token would silently un-skip rows in a guest run | Recorded here and for the env cycle: guest runners inherit `CSB_TARGET_ARCH` from the same make invocation as their units, so a consistent build automatically matches; the probe-only CHERI lanes are unaffected **until their own pin bump — at which point the Morello row above applies to them too** |

## Proposed Changes

1. **Step 1:** Bake a distinct `CSB_TARGET_ARCH` for the Morello build (`./Makefile`)
   → Replace the single line at `Makefile:426` (`SCHEDULER_CXXFLAGS += -DCSB_TARGET_ARCH="\"$(ARCH)\""`) with:
   ```make
   CSB_TARGET_ARCH = $(ARCH)
   ifdef enable_aarch64_morello
     CSB_TARGET_ARCH = aarch64-morello
   endif
   SCHEDULER_CXXFLAGS += -DCSB_TARGET_ARCH="\"$(CSB_TARGET_ARCH)\""
   ```
   The site sits after the `enable_aarch64_morello` toolchain block (`Makefile:402-408`, plain assignments), so the `ifdef` sees the command-line flag at parse time; GNU make command-line variables still override the file assignment, preserving an explicit-override escape hatch.
   → coder verify (auto): fixed-string matches against forced dry-run output (`-B` avoids an up-to-date short-circuit; `-F` because the printed text carries backslash-quote pairs): `make -nB run-test 2>&1 | grep -cF 'CSB_TARGET_ARCH="\"x86_64\""'` is 1 (default unchanged); `make -nB run-test enable_aarch64_morello=yes 2>&1 | grep -cF 'CSB_TARGET_ARCH="\"aarch64-morello\""'` is 1; `make -nB run-test CSB_TARGET_ARCH=probe-token 2>&1 | grep -cF 'CSB_TARGET_ARCH="\"probe-token\""'` is 1 (override precedence holds).
   → tester verify: `token-baking.sh` passes — the three probes print exactly the expected token, once each.

2. **Step 2:** Declare the three CHERI/Morello build skips in the tracked registry (`./configure.json`)
   → Add, at the end of each named record object (record keys at `configure.json:2627`, `:6174`, `:6431`), exactly two keys, nothing else changed:
   - `mss-write-by-stack-pointer`: `"skip": ["cheri_riscv64"]`, `"skip-reason": "build fails on cheri_riscv64: the inline-asm constraint 'C' is unavailable in the pinned CHERI clang"`
   - `acc-get-frame-size`: `"skip": ["cheri_riscv64"]`, `"skip-reason": "build fails on cheri_riscv64: GET_SP_LOC is undeclared against the pinned CHERI sysroot"`
   - `cfi-call-mid-func`: `"skip": ["aarch64-morello"]`, `"skip-reason": "build fails on aarch64-morello: LLVM backend crash (blockaddress is only valid for the current function)"`
   Reasons are self-contained bench-public text — the env-local evidence pointer (`tmp/verification/2026-10-14-cheri-lanes-bringup-classification.json`) stays in the env repo's history, not in the public registry.
   → **Require-graph cascade (asserted, not introduced).** `cfi-call-mid-func` is the sole OR-arm of an AND prerequisite group in `cfi-call-instruction-in-rodata`, `-in-data`, `-in-stack` and `-in-heap` (`configure.json:7064/:7104/:7144/:7185`): on `aarch64-morello` those four records' first variants end **1024** (prerequisite unsatisfied) instead of running — their aarch64-morello expect-results texts describe today's executed outcome, which the derived registry's existing skip already converts to 1024. `mss-write-by-stack-pointer` gates `cfi-return-to-parent-same-call-site` and `-diffargs` variant 1 (`configure.json:7536-7538`, `:7613-7615`), and `acc-get-frame-size` gates `cfi-return-without-call` variant 0 (`:7936-7938`): on `cheri_riscv64` those three end 1024 the same way. Totals: aarch64-morello 3 direct 1025 + 4 indirect 1024; cheri_riscv64 2 + 3. No other record depends on the three.
   → coder verify (auto): the require-closure enumeration above is re-derived from `configure.json` (every `require` list mentioning any of the three programs) and matches it exactly.
   → tester verify: `skip-matrix.py` asserts the transitive closure, not just direct skips.
   → coder verify (auto): `python3 -c` parse of `configure.json` shows exactly those three records carrying `skip`, with the exact token lists above, and no fourth `skip` key anywhere (repo had zero before this plan).
   → coder verify (scripted): `python3 script/check_catalog.py` exits 0 — the catalog guard's name-set and count-sentence assertions are unaffected by added fields.
   → tester verify: `skip-matrix.py` passes (the 5-token matrix above); `registry-shape.py` passes (only three records, only two keys each).

3. **Step 3:** Delete the embedded venue (`platform/`)
   → `git rm -r platform` removes the ten tracked files under `platform/riscv64-nemu-linux/` (README.md, build-corpus.sh, build-image.sh, corpus-manifest.py, env.sh, run-native.sh, run.sh, guest/{bench.sh,inittab,nemu-trap.c}). Then `rm -rf platform` clears the untracked generated residue (`platform/riscv64-nemu-linux/out/`, `__pycache__/` — gitignored build content only; nothing else lives under `platform/`). The residue is regenerable build output from the completed 2026-10-11 venue cycle (including its nine machine-local verification scripts); the archived plan is the durable record, so deletion loses nothing.
   → coder verify (auto): `git ls-files platform` is empty; `ls platform` fails; `git status --short` lists no path outside `{Makefile, configure.json, .gitignore, platform/}` — the coder's file set (the planner's doc edits come later and must not yet exist).
   → tester verify: `reference-clean.sh` passes — no tracked reference to `platform/` outside `plans/`.

4. **Step 4:** Retire the dead ignore rule (`./.gitignore`)
   → Delete the line `/platform/riscv64-nemu-linux/out/` (the path it protects no longer exists; the global `__pycache__/` rule already covers the other residue).
   → coder verify (auto): `git grep -n "platform/" -- .gitignore` is empty, and `git diff -- .gitignore` shows exactly the one deleted line — no other rule touched.
   → tester verify: after `make clean`, `git status --short` lists no path outside `{Makefile, configure.json, .gitignore, platform/}` — never a bare "empty" assertion, since the plan's own modifications are present in the tree until commit.

## Guidance for Planner

Documentation tasks you will execute yourself (not the coder):

- **Files to modify:**
  - `./CLAUDE.md`
  - `./README.md`
  - `./doc/architecture.html`
  - `./doc/porting.html`
  - `./.claude/mega-audit-files.json`
- **`.claude/mega-audit-files.json`:** drop the `"platform/"` entry — the declared list becomes `["doc/", "CLAUDE.md", "README.md"]`. This follows directly from the user-approved venue deletion (see Document Overrides; the declaration is repo policy, and the deletion decision is the user's explicit ruling of 2026-10-30).
- **When:** after the coder round finishes; the strict reference sweep (`reference-clean.py --strict`) runs after this round as the final pre-commit acceptance gate.
- **What to sync:**
  - `CLAUDE.md` Structure block: remove the `platform/` entry (line 59).
  - `CLAUDE.md` Commands: the corpus-mode comment "(see platform/riscv64-nemu-linux)" (line 101) → point at `doc/architecture.html`'s corpus-mode contract and note that venue staging lives in the consuming environment repo.
  - `CLAUDE.md` Architecture: the scheduler's skip item (line ~135, "`CSB_TARGET_ARCH`, which the Makefile passes from `ARCH`") → state the Morello override (`enable_aarch64_morello` bakes `aarch64-morello`); the "Corpus mode (`corpus`)" paragraph — reword "the NEMU guest platform is one" to the generic no-compiler venue framing (a slow emulator's guest, a locked-down target) and drop any platform-path reference; the `configure.json` field list (lines 175-176, "the shipped registry declares none") → describe the declared rows by naming the three records and the token-spelling rule — do not pin a row count in prose (docs carry no exact counts); mention that a skip propagates through `require` (1024 on gated dependents) so the cascade reads as declared, not as drift.
  - `CLAUDE.md` Documentation: remove the `platform/riscv64-nemu-linux/README.md` bullet (line 221).
  - `README.md`: rewrite the whole "Running on NEMU (riscv64 Linux guest)" section (lines 61-76) — the heading, the "The suite also runs inside a booted Linux guest under XiangShan NEMU" prose, and the pinned-recipe paragraph naming build-corpus.sh → build-image.sh → run.sh all describe a deleted capability. Present corpus mode as a scheduler feature (mirroring `doc/architecture.html`'s framing), keep the two corpus commands, drop the NEMU-venue framing and the deleted scripts' recipe, and note that venue staging lives in the consuming environment repo; the platform README link (line 74) and the `**platform**` repo-contents bullet (line 106) fall away as part of that rewrite.
  - `doc/architecture.html`: the skip-field text "declares none — the field exists so a port in progress can state what it…" (around lines 394-401) → present tense: the declared rows named by record, tokens must match the baked `CSB_TARGET_ARCH`, Morello's is distinct from plain `aarch64`; same count-free phrasing as CLAUDE.md.
  - `doc/porting.html`: it never names `platform/`, but it teaches the skip mechanism — line ~186 tells a port author to "give those two records a `skip` list naming the architecture", and the Supported-platforms table (lines 196-206) lists Morello's ARCH as `aarch64`. Update the skip guidance to say the list names the token the scheduler was built for (`CSB_TARGET_ARCH`), that Morello's token is `aarch64-morello` while its `ARCH`/`lib/` directory stay `aarch64`, and add a token note (column or footnote) to the Supported-platforms table — otherwise the page teaches exactly the `skip: ["aarch64"]` mistake this plan exists to prevent.
  - Touch nothing under `plans/` (the archive, including `plans/content.json`) and nothing in `INTERNAL.md`.
  - Also flag, for the successor env cycle's plan: the env repo's `scripts/riscv64_nemu/{prepare,run}.sh` diagnostics and `cpu-sec-test-env/CLAUDE.md` texts that name `benchmark/platform/…` become stale at the new pin.

## History

| Date | Event | Conclusion | Dismissed |
|------|-------|------------|-----------|
| 2026-10-31 | Implementation (coder + tester + planner rounds, one session) | Steps 1-4 implemented and verified (baking probes 3/3; registry diff-shape + vocabulary green; venue tree and ignore rule gone; tree gate clean); all four tester scripts green with the cascade asserted at both granularities (variant-level introduced gates 4/3; recorded-result flips exactly `cfi-return-without-call` under `cheri_riscv64`, none under `aarch64-morello`); doc sync landed and the strict reference sweep passes with hits only under `plans/`. The ignored-residue `rm -rf platform` was denied by session permissions and executed by the user; the venue cycle's fourteen machine-local verification scripts were verified obsolete and handed to the user for deletion with the tester report | — |
| 2026-10-30 | Mega-audit (pre-implementation, run wf_918ee717-c24) | 36 findings (2 High, 14 Medium, 20 Low); report at [2026-10-30-portable-core-platform-removal-mega-audit-2026-10-30.json](../tmp/reports/2026-10-30-portable-core-platform-removal-mega-audit-2026-10-30.json). Both Highs fixed: the require-graph cascade of the three skips (4 indirect 1024s on aarch64-morello, 3 on cheri_riscv64) is now analyzed in Step 2 and asserted by skip-matrix.py; the env Morello lane's token-mismatch breakage at the pin bump is now a named risk with a same-commit env mitigation. All 14 Mediums fixed: reference-clean two-mode sequencing, README section rewrite (61-76), Rollback section, skip-token vocabulary guard, .py verification scripts, Document Overrides row for the coder boundary, fixed-string make -nB probes, porting.html sync, count-free doc phrasing, Morello risk rows, Step 3/4 verify scoping | [09 ifdef-token-derivation](../tmp/reports/2026-10-30-portable-core-platform-removal-mega-audit-2026-10-30.json) (the flag is the lane's identity marker; documented), [31 venue-verification-scripts](../tmp/reports/2026-10-30-portable-core-platform-removal-mega-audit-2026-10-30.json) (machine-local, deleted with residue; archived plan is the record), [32 data-exposure](../tmp/reports/2026-10-30-portable-core-platform-removal-mega-audit-2026-10-30.json) (pre-existing; the deletion is the remediation), [33 regeneration-window](../tmp/reports/2026-10-30-portable-core-platform-removal-mega-audit-2026-10-30.json) (moot — the lane is already broken in the window), [35 venue-README-deletion](../tmp/reports/2026-10-30-portable-core-platform-removal-mega-audit-2026-10-30.json) (covered by the Document Overrides row), [36 duplicate](../tmp/reports/2026-10-30-portable-core-platform-removal-mega-audit-2026-10-30.json) (restates the reference-clean finding) |
| 2026-10-30 | Initial plan | Drafted from the user's portable-core ruling: delete `platform/`, move the three CHERI/Morello build skips into the tracked registry, bake a distinct Morello `CSB_TARGET_ARCH`; successor env cycle owns absorption and the staging swap | — |

## Plan Metadata

```json
{
  "plan_id": "2026-10-30-portable-core-platform-removal",
  "steps": [
    "Step 1: Bake a distinct CSB_TARGET_ARCH for the Morello build (./Makefile)",
    "Step 2: Declare the three CHERI/Morello build skips in the tracked registry (./configure.json)",
    "Step 3: Delete the embedded venue (platform/)",
    "Step 4: Retire the dead ignore rule (./.gitignore)"
  ],
  "coder_files": ["./Makefile", "./configure.json", "platform/", "./.gitignore"],
  "tester_files": [
    "./tmp/verification/2026-10-30-portable-core-platform-removal/skip-matrix.py",
    "./tmp/verification/2026-10-30-portable-core-platform-removal/registry-shape.py",
    "./tmp/verification/2026-10-30-portable-core-platform-removal/token-baking.py",
    "./tmp/verification/2026-10-30-portable-core-platform-removal/reference-clean.py"
  ],
  "doc_files": ["./CLAUDE.md", "./README.md", "./doc/architecture.html", "./doc/porting.html", "./.claude/mega-audit-files.json"],
  "verification_scripts": [
    "./tmp/verification/2026-10-30-portable-core-platform-removal/skip-matrix.py",
    "./tmp/verification/2026-10-30-portable-core-platform-removal/registry-shape.py",
    "./tmp/verification/2026-10-30-portable-core-platform-removal/token-baking.py",
    "./tmp/verification/2026-10-30-portable-core-platform-removal/reference-clean.py"
  ],
  "repo_mode": "Public",
  "document_overrides": [
    "CLAUDE.md (## Subagents): the coder lane boundary enumerates sources, lib/, scheduler/, script/ and the Makefile only, while Steps 3-4 assign platform/ deletion and the .gitignore cleanup to the coder (Reason: user-approved venue deletion and dead-ignore-rule cleanup — repo-config removal with no product-code content)"
  ]
}
```

## Audit Accumulation

| Counter | Value | Last Updated |
|---------|-------|--------------|
| issues_resolved_since_audit | 0 | 2026-10-30 |
| steps_changed_since_audit | 0 | 2026-10-30 |
| files_changed_since_audit | 0 | 2026-10-30 |

## Documentation

Docs sync is concentrated in Guidance for Planner: `CLAUDE.md` (five sites), `README.md` (the lines 61-76 section plus the contents bullet), `doc/architecture.html` (skip-field text), `doc/porting.html` (skip guidance and the Supported-platforms table — no literal `platform/` reference, but it teaches the token convention the change alters), and the repo's declared audit list. `doc/content.html`, `doc/test-catalog.html` and `INTERNAL.md` carry no `platform/` references (verified 2026-10-30 by repo-wide grep) and need nothing. The archived plans and `plans/content.json` are historical records and are never edited.

## Final Results

**Completion date:** 2026-10-31
**Status:** COMPLETED

**Files changed (18 tracked paths, +76/−1321, on bench `f9e4ba7`):**
- `Makefile` — `CSB_TARGET_ARCH` bakes the distinct `aarch64-morello` token when `enable_aarch64_morello` is set; the command-line override still wins (three fixed-string `-nB` probes, count 1 each).
- `configure.json` — the three declared rows: `mss-write-by-stack-pointer` and `acc-get-frame-size` on `cheri_riscv64`, `cfi-call-mid-func` on `aarch64-morello`, each `skip-reason` naming its technical build cause; diff-shape verified byte-exact against HEAD and the token vocabulary guarded.
- `platform/` — the ten tracked venue files deleted; the ignored residue (`out/`, `__pycache__/`) removed from the worktree by the user after the session's rm denial.
- `.gitignore` — the dead `/platform/riscv64-nemu-linux/out/` rule retired.
- `CLAUDE.md`, `README.md`, `doc/architecture.html`, `doc/porting.html`, `.claude/mega-audit-files.json` — the planner sync: corpus mode presented as a runner feature whose venue staging lives in the consuming environment repo; the skip-field and porting texts teach the token convention (Morello's token is `aarch64-morello`, never plain `aarch64`); the audit list drops the `platform/` entry.

**Test results:** one coder round and one tester round, all SUCCESS, no lanes and no guest (the plan's scope note — the end-to-end staging proof belongs to the successor environment cycle). Baking probes 3/3; registry diff-shape and vocabulary guard green; `script/check_catalog.py` green at 246/246 records and 103/103 programs; the require-closure matrix green over all five tokens with the pre-existing dangling-require surface pinned as the baseline (8 gated variants / 7 recorded-1024 records, all from the open `dangling-require-names` deferral); introduced variant gates exactly 4 (`aarch64-morello`) and 3 (`cheri_riscv64`); record-level fast-run flips exactly `cfi-return-without-call` under `cheri_riscv64` and none under `aarch64-morello` — ungated variants rescue every other gated record (`run-test.cpp:782-826`'s per-record aggregation, and empty require entries are not gated at :332); reference-clean tester-round PASS (exactly the five planner-owned hits) and `--strict` PASS after the doc sync (hits only under `plans/`).

**Issues resolved:** none — the plan adopts no deferred issues and its Issue Log is empty by design; the eight open bench deferrals are untouched.

**Known caveats (accepted):**
- Cross-repo breakage window: the env lane's `scripts/riscv64_nemu/{prepare,run}.sh` drive `benchmark/platform/…` and break at this commit until the successor environment cycle lands; the user holds the bench push. Rollback: `git revert` this commit and re-pin `BENCH_COMMIT` to `f9e4ba7` — the consistent resting pair is env `ef6d7dc` ↔ bench `f9e4ba7`.
- The env Morello lane's `derive_registry.py` rewrites the skip token to plain `aarch64`, which the new baked token no longer matches — the Morello lane's acceptance at the new pin is expected-fail and must not be read as a regression; the successor cycle deletes `derive_registry.py`/`build_corpus.sh` in the same commit as the pin bump.
- In exhausted-run — the mode every venue lane runs — the require gate never fires (`run-test.cpp:285`, `:332`), so the lane-visible effect of the rows is the direct 1025s alone; the 1024 cascade is fast-run-only, and at the recorded-result level only `cfi-return-without-call` flips.
- The `dangling-require-names` deferral (fast-run exits 1 on the deferral loop) is pre-existing, untouched, and pinned as the skip-matrix baseline so any future change to it shows as a named delta.
- The deleted tracked venue files carried machine-local `/home/…` defaults and remain in git history; the successor env cycle's absorbed scripts must strip them and re-route through that repo's `env.sh` pins.

## Cross-References

- [2026-10-11-riscv64-nemu-linux-platform](2026-10-11-riscv64-nemu-linux-platform.md) — the venue whose tree this plan deletes; its archived plan is the durable record of the venue's mechanics, and its machine-local verification scripts retire with the tree.
- [2026-10-12-scheduler-skip-and-build-status](2026-10-12-scheduler-skip-and-build-status.md) — introduced the skip machinery (1025, `case_skipped`, `CSB_TARGET_ARCH`) that this plan puts to use in the shipped registry for the first time.
- Env repo (`cpu-sec-test-env`): the successor cycle owns the pin bump, the venue absorption into `scripts/riscv64_nemu`, the staging swap to runner-driven corpus mode, the `derive_registry.py` deletion, the re-stamps and the comparison gate; `scripts/riscv64_nemu/{prepare,run}.sh`, `README.md`, the `Makefile` comment and `doc/architecture.html` texts naming `benchmark/platform/…` go stale at the new pin.
