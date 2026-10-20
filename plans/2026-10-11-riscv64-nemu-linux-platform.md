# Plan: Run cpu-sec-bench on a booted Linux under NEMU (riscv64-nemu-linux platform, MVP)

**Project:** /home/wsong83/proj/xiangshan/cpu-sec-test-env/benchmark
**Plan ID:** 2026-10-11-riscv64-nemu-linux-platform
**Created:** 2026-10-11

## Issue Log

Rows ordered newest-first (latest issues at top). New issues are prepended; resolved issues stay in place with updated status.

| Issue | Status | Found | Resolved | Resolved By |
|-------|--------|-------|----------|-------------|
| [corpus-build-registry-swap-crash-safety](./tmp/reports/defer-issue-corpus-build-registry-swap-crash-safety.json) | Open | 2026-10-20 | — | — |
| [indepassembly-return-site-untranslated](./tmp/reports/defer-issue-indepassembly-return-site-untranslated.json) | Resolved | 2026-10-11 | 2026-10-14 | plan 2026-10-14-indepassembly-return-site-translation — all three `assembly_return_site` bodies translated (riscv64/aarch64/cheri_riscv64) and the Morello purecap variant added, so this plan's Step 4 corpus cross-build is no longer blocked bench-side; this plan's own work (corpus mode, image recipe, guest harness) is untouched, and the env-side follow-ups (skip-injection removal, `disable_independent_assembly` pin drop, `BENCH_COMMIT` bump, lane re-runs) go to its successor env plan. |

## Guidance for Coder

**Files to modify:** (source only — no docs, no tests, no `configure.json`, no `lib/`, no test-case sources)

- `./Makefile`
- `./scheduler/run-test.cpp`
- `./platform/riscv64-nemu-linux/env.sh` (new)
- `./platform/riscv64-nemu-linux/build-corpus.sh` (new)
- `./platform/riscv64-nemu-linux/corpus-manifest.py` (new)
- `./platform/riscv64-nemu-linux/build-image.sh` (new)
- `./platform/riscv64-nemu-linux/run.sh` (new)
- `./platform/riscv64-nemu-linux/run-native.sh` (new)
- `./platform/riscv64-nemu-linux/guest/inittab` (new)
- `./platform/riscv64-nemu-linux/guest/bench.sh` (new)
- `./platform/riscv64-nemu-linux/guest/nemu-trap.c` (new)
- `./.gitignore`

**Step-by-step with verification:** each step below carries coder checks tagged `(auto)` or `(scripted)`. Coder self-verifies `(auto)` checks and runs the provided script at `./tmp/verification/2026-10-11-riscv64-nemu-linux-platform-stepN.py` for `(scripted)` checks. Steps 7–8 are tester-owned and carry `→ tester verify:` lines instead.

**Hard boundaries:** the result-code contract in `doc/architecture.html` is unchanged. `configure.json`, test-case sources (`acc/ cfi/ cpi/ mss/ mts/`), `lib/` and `script/` are **not** touched. Native (non-corpus) scheduler behavior must be byte-for-byte unchanged in semantics — the corpus mode must be strictly additive behind its new option word. Run `make clean` before leaving the tree whenever a step ran `make` or `./run-test`, per the repo's own rule.

**Long-running steps:** Steps 5 and 6 download/build a Linux kernel and boot NEMU; each may exceed a single Bash call's 10-minute cap and must be run with `run_in_background: true` and monitored, or split into sub-commands.

## Guidance for Tester

**Tests to create:**

- **Permanent:** none. The repo has no automated test framework; standing one up is out of scope for this plan. The committed platform scripts (`platform/riscv64-nemu-linux/*`) are themselves the durable, re-runnable verification vehicle, and the planner's step scripts under `./tmp/verification/` are re-runnable by anyone.
- **Temporary:** mark all of the following as `(temporary)` and delete them after the plan completes (they live under `./tmp/`):
  - `scheduler-corpus-matrix` — behavioral matrix for the new `corpus` option, executed independently of the coder's step scripts: (a) with a filtered configure.json of the 22 `acc` records, `./run-test corpus make-only` builds and creates `test/<program>__<key>` files; every created file executes (`test/<program>__<key>` run manually exits with a sane code and not `ENOEXEC`/`EACCES`); (b) `./run-test no-make corpus exhausted-run` records all 22 records in `results.json`; (c) **negative control:** delete one corpus file, re-create a fresh `results.json`, re-run → the scheduler exits non-zero and names the missing file on stderr; (d) **parity control:** a non-corpus `./run-test exhausted-run` run of the same filtered subset produces the same set of record keys in `results.json` with the same result interpretation (individual verdicts may legitimately differ run-to-run under ASLR; assert key-set equality and the absence of new anomaly codes, not verdict equality). `make clean` afterwards; `git status --short` clean.
  - `image-layout` — after `build-image.sh RUNSET=acc`: independently verify the `fw_payload.bin` layout by scanning magics/bytes (FDT magic near offset 2 MiB, OpenSBI bytes at 1 MiB, kernel `Image` bytes at 4 MiB — compare via `dd if=… bs=1M skip=4 count=1 | cmp - Image`, cpio magic `070701` at the MB-aligned initramfs offset) and by parsing the embedded cpio to a file list (must contain `/bench/run-test`, `/bench/configure.json`, `/etc/inittab`, `/lib/ld-linux-riscv64-lp64d.so.1`). Assert total size < 128 MiB (guest RAM).
  - `guest-e2e-acc` — boot the `acc` image on the pinned NEMU binary: NEMU exits 0, `boot.log` contains OpenSBI banner, kernel banner, the `===CSB-…===` markers, and `HIT GOOD TRAP`; the extracted `results.json` contains all 22 acc records; `meta.json` records image sha256, NEMU sha256, toolchain version, bench commit.
  - `guest-e2e-full` — the same end-to-end for the full 246-record image (`RUNSET=all`), plus completeness accounting: number of records in `results.json` == 246, classification counts per `expect-results` semantics summarized.
  - `native-reference-compare` — run `run-native.sh` (the suite compiled and run natively on the host, no NEMU) and compare the per-category result distribution with the full NEMU run: tabulate counts of `result == 0` / expected codes / unexpected codes per category (`acc`, `mss`, `mts`, `cpi`, `cfi`) for both; every discrepancy cell is traced to a named record and reported (a discrepancy is a finding, not automatically a failure; the porting guide's own acceptance method is exactly this distribution comparison).

**Tests to investigate for retirement:**

- No test obsolescence identified. The repo has no test suite; no existing test exercises `scheduler/run-test.cpp`, the Makefile's toolchain selection, or any file this plan changes.

**Retirement procedure:** not applicable this round.

## Summary

**Problem.** The security suite must run on a booted Linux under XiangShan NEMU (system mode, riscv64). Today the bench runs only on native hosts: the scheduler compiles each variant of each test case *at run time* and executes it. The target venue (an NEMU guest) has no compiler and no writable network; and the guest is transient (initramfs, no persistence). The bench must therefore gain a **build-elsewhere / run-here** flow, and the repo must gain a **platform** that fabricates a bootable image containing the prebuilt corpus and the scheduler, runs it under NEMU, and extracts verifiable results.

**Approach.** Four layers, each pinned and additive:

```
HOST (x86_64)                                             GUEST (riscv64 Linux on NEMU, 128 MiB RAM)
+---------------------------------------------+           +----------------------------------------+
| cpu-sec-bench @ commit (unmodified tree)    |           | /  (initramfs, writable ramfs)         |
|   make run-test              (host build)   |           |  /init -> busybox init                 |
|   env ARCH=riscv64 CXX=riscv64-...-g++ \    |  bake     |  /etc/inittab: ::once:/bench/bench.sh  |
|       ./run-test corpus make-only           | --------> |  /bench/   run-test (riscv64)          |
|     - builds every (program,macro-map) unit |           |            configure.json              |
|       with forced -B, snapshots each as     |           |            test/<prog>__<key>  corpus  |
|       test/<program>__<key>                 |           |            lib/common/libcfi.so        |
|   (later: cross-build run-test; make clean) |           |  /lib/     glibc, libstdc++, libgcc_s  |
|                                             |           |  bench.sh: ./run-test no-make corpus \ |
| Image assembly (thin wrapper over pinned    |           |              exhausted-run             |
| workload-builder scripts @ b7c321d):        |           |            cat results.json/.dat       |
|   gcpt (LibCheckpointAlpha @ 5169da7)       |           |            nemu-trap $rc               |
|   OpenSBI v1.9 (cbf9f67 + wb config/patch)  |           +----------------------------------------+
|   Linux 6.18.56 (kernel.org, sha256-pinned, |                    | guest console = NEMU stderr
|       wb linux.config minus 1 initramfs line)|                    v
|   DTS: wb generator, name "xiangshan"       |           +----------------------------------------+
|   busybox 1.36.1 (static) + our rootfs      |           | 03-run.sh: timeout NEMU -b image       |
|   -> rootfs.cpio -> wb build-firmware-linux |           | extracts results between markers ->    |
|   -> fw_payload.bin  (dd fixed layout)      |           | runs/<ts>/{boot.log,results.json,      |
| $NEMU_BIN -b fw_payload.bin ----------------+           |   results.dat, meta.json}              |
+---------------------------------------------+           +----------------------------------------+
```

**Why a corpus feature in the scheduler (the core bench change).** The scheduler's documented two-phase options (`make-only` / `no-make`) are almost the cross-build story — but the binary at `test/<program>` is rebuilt **in place** whenever the record's `conf-macro` map changes, and 17 programs carry multiple macro variants (2026-10-20, corpus model: 145 distinct `(program, macro-map)` build units over 246 records / 103 programs, 25 distinct maps, at most 4 per program). A naive `make-only` + `no-make` split therefore runs the wrong binary for most variant records. The plan adds a `corpus` option: in build phase it forces `-B`, and snapshots every freshly built binary as `test/<program>__<key>` where `<key>` is a deterministic 8-hex FNV-1a-32 of the canonical macro-map string (`K=V` pairs, keys sorted, `,`-joined; empty map → `811c9dc5`); in run phase it executes the `<key>`-suffixed path and fails fast (`exit 1`, file named on stderr) if a corpus file is missing. Non-corpus behavior is untouched.

**Why 6.18.56 and workload-builder.** The first boot proof on this workstation's NEMU was Linux 6.1.83 + OpenSBI-family (Experiment F, `tmp/experiments/nemu-program-io/`, 2026-10-10: 21,464,788 instrs, 145 ms, `HIT GOOD TRAP` ret 0) — but the guest userspace this platform runs is built by the apt cross toolchain, whose glibc is RVA23U64 vector-using code, and kernel-side RISC-V userspace vector support only exists from Linux 6.5 (`arch/riscv/kernel/vector.c`, `CONFIG_RISCV_ISA_V`). On 6.1.83 every guest process dies with SIGILL on its first vector instruction — measured in the 2026-10-20 coder round: the static busybox dies in a glibc memset `vsetivli` at init, and a dynamic probe built with the same cross g++-13 dies inside ld-linux itself before any user code runs. The kernel pin is therefore 6.18.56, with `CONFIG_RISCV_ISA_V` and `CONFIG_RISCV_ISA_V_DEFAULT_ENABLE` pinned into the config and asserted against the built `.config` — exactly what the env repo's riscv64 lane does, for the same reason. workload-builder is the maintained, scripted, pipelined image producer; its `build-firmware-linux.sh`/`build-sbi.sh`/`build-gcpt.sh` are standalone bash with clean argument surfaces, so the platform wraps them at a pinned commit instead of reimplementing the dd-layout, DTS generation, and OpenSBI patch/config.

**Alternatives considered and rejected.**

- *Compiler inside the guest* — image +10s of MB, guest compile time ~10-50× native, and it diverges from the repo's prebuild-friendly design. Rejected.
- *Multi-pass staging with per-group filtered configure.json* (zero scheduler change) — duplicates the scheduler's build/variant knowledge in shell/python, multi-invocation semantics diverge from a single native run, fragile. Rejected; the corpus feature is smaller and upstreamable (it also serves any build-host/run-target split, e.g. CI).
- *Rewriting the image assembly ourselves* (vendoring wb scripts or hand dd) — drift and licensing/log maintenance; full-buildroot pipeline — heavy (Bootlin SDK, ~1-2 h first build) and unnecessary since we can feed `build-firmware-linux.sh` our own `Image`. Rejected.
- *Embedded initramfs (kernel CONFIG_INITRAMFS_SOURCE)* — forces kernel rebuild per corpus change and diverges from wb's assembly; the DTB `/chosen linux,initrd-start/end` path (which wb's assembler patches automatically) keeps cpio composition independent. Rejected.
- *Static linking of test cases* — does not fix the variant problem and diverges from native dynamic defaults; libs are ~7 MB, trivially affordable in the initramfs. Rejected.

**Acceptance criteria (definition of done).** (1) `make` still builds the scheduler natively; `./run-test exhausted-run` on the host is unaffected. (2) `RUNSET=acc` image boots on the pinned NEMU, scheduler runs in-guest, all 22 acc records reported, NEMU exits 0 with `HIT GOOD TRAP`. (3) Full `RUNSET=all` image: all 246 records reported; distribution compared against the host-native reference run (per `doc/porting.html`'s port-verification method) with every discrepancy traced or reported. (4) All provenance artifacts recorded (manifest with per-file sha256, image sha256, NEMU sha256, toolchain version, bench commit). (5) `make clean`-clean tree; no generated artifacts tracked.

**Explicitly out of scope:** SD-card/disk persistence (initramfs only), per-case timeout (the host-side `timeout` remains the mitigation; the `run-test continue` bug this line used to pair with it was fixed at bench `c8c6000`), the `cpu-sec-test-env` lane integration (this plan delivers the standalone bench platform — a successor env cycle consumes it as a lane, taking the platform's `runs/<ts>/` outputs as its input), checkpoints/SMP/multi-hart, musl or dynamic-linking variants, trace-log analysis, RTL/Verilator venue, any modification to NEMU or Linux trees (both consumed pinned and unmodified), upstreaming NEMU user mode.

## Repo Mode

Public

*Detected at plan creation. Determines: plan archival path, commit-message content, staged files.*

## Document Overrides

| Document | Rule Overridden | Reason |
|----------|----------------|--------|

## Risks

| Risk | Impact | Mitigation |
|------|--------|------------|
| OpenSBI v1.9 + this DTS has not been boot-verified on NEMU master@ae1ec3cd (wb CI validates assembly, not boot; the locally-proven image was the older ready-to-run build: OpenSBI v1.4 + Linux 6.1.83) | First boot fails or hangs | Verification is staged: the `acc` smoke image boots before any full run; the ready-to-run `linux.bin` remains the known-good control to A/B against if boot fails; fallback is pinning OpenSBI to the v1.4-era commit and re-running assembly. **Retired 2026-10-20:** the shipped OpenSBI v1.9 image booted on NEMU `ae1ec3cd` (out/acc/runs/20261020-122225, all 22 acc records, `HIT GOOD TRAP`), so no fallback was needed. |
| Kernel / cross-GCC era mismatch: the guest kernel is built by default `riscv64-linux-gnu-*` GCC 13, the same cross the env repo's riscv64 lane pins (the workspace `riscv64-unknown-linux-gnu-` GCC 16.1.0 stays as the documented override), and that toolchain's glibc is RVV-using — kernels before 6.5 have no userspace vector support, which is why the guest kernel is pinned to **6.18.56** rather than the original 6.1.83 | Build warnings escalated to errors; guest userspace SIGILLs | Build with `WERROR=0` (kernel-supported knob); the recipe pins `CONFIG_RISCV_ISA_V` + `CONFIG_RISCV_ISA_V_DEFAULT_ENABLE` into wb's version-tolerant fragment and asserts both against the built `.config`; measured 2026-10-20: 6.1.83 SIGILLs both a static busybox and a dynamic ld-linux, 6.18.56 boots the corpus clean |
| Guest RAM is 128 MiB (wb `xiangshan` DTS profile) | Image or runtime out of memory | Image budget asserted < 128 MiB by script; measured expectation ~20-30 MB (kernel ~3 MB + cpio ~15-25 MB); custom DTS memory bump is a documented fallback (wb custom-template-dir) |
| A malformed case hangs the guest; the scheduler has no per-case timeout | Full run wastes its wall-clock and aborts partial | Subset-first acceptance; host-side `timeout` around NEMU; harness reports `N/246` completeness and fails loudly; per-case timeout deferred by decision (documented) |
| Corpus filename key collisions or argv truncation (scheduler `arg_pool[32][64]`) | Wrong binary executed; buffer overflow | FNV-1a-32 key over ≤4 maps/program (2026-10-20 corpus model: 145 units, 25 distinct maps across 103 programs) has ~3×10⁻⁹ collision odds; manifest script asserts name uniqueness and expected-set equality; `arg_pool` is enlarged to `[32][256]` in the same step; longest current corpus path is 63 chars (asserted by the manifest script) |
| File-copy snapshot loses the executable bit | `posix_spawn` EACCES in guest | Corpus copy explicitly `chmod 0755` on POSIX (MSVC branch guarded); tester's matrix runs a corpus file manually to prove executability |
| Dynamic glibc from the cross toolchain vs the guest kernel (glibc/kernel minimum-version mismatch) — this row materialised as **userspace vector**: the cross glibc is RVV-using, so the guest kernel must be ≥ 6.5 | Guest processes fail to start (SIGILL on the first vector instruction) | Lib set derived from `readelf -d` NEEDED (not guessed) and staged to `/lib/`; kernel pin 6.18.56 with `CONFIG_RISCV_ISA_V`/`_DEFAULT_ENABLE` asserted against the built `.config`; smoke asserts scheduler and one corpus case run before the full pass |
| wb/OpenSBI/kernel fetches at build time (network) | Non-reproducible or flaky builds | Every input pinned: wb `b7c321d`, OpenSBI `cbf9f67` (= v1.9), LibCheckpointAlpha `5169da7`, kernel tarball sha256 `4633bbda2ab95eaf2a7af4041236451d6759c72e6946265531af4b3aacefbf89`, busybox sha256 `b8cc24c9574d809e7279c3be349795c5d5ceb6fdf19ca709f80cde50e47de314`; downloads cached under `out/` — in this workspace the pinned kernel 6.18.56 and busybox 1.36.1 tarballs already exist (sha256-matching) in the env repo's `build/downloads/` and can seed the cache, so only the three git clones need network on the first build; assembler is committed upstream scriptage, not ours |
| Full-suite guest wall-clock (246 records × 145 build units are host-side; guest runs are compute-only) | Acceptance run takes long | Estimate: guest ≈ 10⁹-10¹⁰ instrs ≈ minutes at the measured ~147 M instr/s; host timeout generous (step 6); tester splits evidence (acc first) |

## Proposed Changes

Each step includes verification checks tagged with confidence. `(auto)` = mechanical. `(scripted)` = deterministic multi-step, script at `./tmp/verification/2026-10-11-riscv64-nemu-linux-platform-stepN.py`.

1. **Step 1:** Make the toolchain selection overridable via the environment in `./Makefile` (POSIX branch): change `CXX := g++`, `ASM := as`, `OBJDUMP := objdump` (and the Darwin `CXX := clang++`) to `?=` forms. Command-line override (`make CXX=…`) continues to win; environment override becomes possible, which is the only way the scheduler's child `make` invocations (which receive no extra arguments) can be steered to the cross toolchain. Nothing else in the Makefile changes.
   → coder verify (auto): `grep -nE '^(CXX|ASM|OBJDUMP)\s*\?:?=' Makefile` shows the three `?=` assignments in the POSIX branch; `make -n run-test | head -1` still shows `g++`; `env CXX=/bin/echo make -n run-test` output starts with `/bin/echo`
   → coder verify (scripted): script at `./tmp/verification/2026-10-11-riscv64-nemu-linux-platform-step1.py` (asserts default invocation unchanged, env override effective, command-line override effective, MSVC branch untouched)
   → tester verify: covered by `scheduler-corpus-matrix` (native `make`/`./run-test` behavior unchanged)

2. **Step 2:** Add the corpus **build** side to `./scheduler/run-test.cpp`: (a) new option word `corpus` (set a global `bool corpus_run`), added to the `help` text; (b) enlarge `arg_pool` to `char arg_pool[32][256]`; (c) helper `corpus_key(const str_map_t&)`: canonical string = `KEY=VALUE` pairs from the record's macro map, keys sorted (the map is `std::map`, iteration order is sorted), `,`-joined, then FNV-1a-32 (init `0x811c9dc5`, prime `0x01000193`) printed as `%08x`; (d) in the make phase, when `corpus_run`: always take the `-B` branch (even for empty macro maps), and after a successful build copy `test/<program>` to `test/<program>__<key>` with a POSIX `chmod(…, 0755)` (MSVC branch guarded), printing `corpus: <src> -> <dst>`; (e) record `file-size` from the run-path file. Non-corpus behavior identical.
   → coder verify (auto): `./run-test help` lists `corpus`; `grep -n 'corpus_key\|arg_pool\[32\]\[256\]' scheduler/run-test.cpp` shows the helper and the enlarged pool; the FNV constants `0x811c9dc5` / `0x01000193` appear exactly once
   → coder verify (scripted): script at `./tmp/verification/2026-10-11-riscv64-nemu-linux-platform-step2.py` — builds the host scheduler, runs `./run-test corpus make-only` against a filtered `acc` configure.json written to a scratch copy of the repo root (or the repo root itself with cleanup), and asserts: every expected `test/<program>__<key>` exists with the key computed by an independent Python FNV-1a-32 implementation; each file's bytes equal the corresponding `test/<program>` build product byte-for-byte; each corpus file is mode `0755`; the captured stdout contains `-B` in every `make` line
   → tester verify: `scheduler-corpus-matrix` (a) and the name/uniqueness assertions

3. **Step 3:** Add the corpus **run** side to `./scheduler/run-test.cpp`: when `corpus_run` is set, the run command becomes the corpus path `test/<program>__<key>` (computed from the same helper); before spawning, `file_exist` the corpus path and on absence print `corpus file missing: <path>` to stderr and `exit(1)` (fail fast rather than recording a bogus result). Everything else — argument expansion, exit-status interpretation, `results.json`/`variables.json`/`results.dat` writing, `set-var` handling — unchanged.
   → coder verify (auto): `grep -n 'corpus file missing' scheduler/run-test.cpp` finds the guard; the run path expression is `cmd = corpus_run ? corpus_path : ("test/" + prog)`-shaped (read the two lines)
   → coder verify (scripted): script at `./tmp/verification/2026-10-11-riscv64-nemu-linux-platform-step3.py` — (a) happy path: build `acc` corpus, delete `results.json`, run `./run-test no-make corpus exhausted-run` → exit 0, `results.json` has the 22 acc record keys; (b) negative control: delete one `test/<prog>__<key>` file, delete `results.json`, re-run → exit 1, stderr names the missing file; (c) parity: a non-corpus `./run-test exhausted-run` of the same subset yields the identical record-key set (verdicts not compared — ASLR may vary them); run `make clean` afterwards
   → tester verify: `scheduler-corpus-matrix` (b), (c), (d)

4. **Step 4:** Create the platform corpus pipeline: `./platform/riscv64-nemu-linux/env.sh` (toolchain/NEMU/OUT paths from env with documented defaults; `CXX`/`ASM`/`OBJDUMP` defaults = the env repo's riscv64-lane pins — `riscv64-linux-gnu-g++-13`, `riscv64-linux-gnu-as`, `riscv64-linux-gnu-objdump` — so the NEMU and qemu riscv64 corpora share one compiler, with the workspace `riscv64-unknown-linux-gnu-` toolchain as the documented override; `NEMU_BIN` default `/home/wsong83/proj/xiangshan/NEMU/build/riscv64-nemu-interpreter`; `OUT` default `./platform/riscv64-nemu-linux/out`), `./platform/riscv64-nemu-linux/build-corpus.sh` (host `make run-test`; then `env ARCH=riscv64 CXX="$CXX" ASM="$ASM" OBJDUMP="$OBJDUMP" ./run-test corpus make-only exhausted-run` (`exhausted-run` is required with a filtered subset — without it a `require` crossing the subset boundary aborts the scheduler — and it keeps the build and run phases enumerating the same argument kind) — Step 1's `?=` forms carry the exported toolchain into the scheduler's child `make`s, and the `OBJDUMP` knob is **mandatory** on riscv64 since the 2026-10-19 extraction fix, bench `74195c4` (the extractor consumes it and fails loudly when unset; the old bare-`objdump` shim dir is obsolete and must not be reintroduced); then cross-build `make run-test ARCH=riscv64 CXX="$CXX"` and stage the riscv64 `run-test`; stage `lib/common/libcfi.so`, `configure.json`, the `test/*` corpus files plus the two run-time support files the corpus binaries reach (the plain `test/acc-check-ASLR`, which acc/check-ASLR.cpp re-invokes through a hard-coded relative path, and `test/acc-read-func-func-opcode.tmp`, the opcode side file acc/read-func.cpp reads), and `lib/` runtime libraries derived from `readelf -d` NEEDED over all staged riscv64 ELF files; write `test/sys_info.txt` describing the NEMU guest venue; finally `make clean`), `./platform/riscv64-nemu-linux/corpus-manifest.py` (stdlib-only; independent FNV-1a-32 reimplementation; enumerates expected `(program, macro-map)` units from `configure.json`, asserts expected==actual file set, per-program key uniqueness, and the ≤63-char path budget; writes `out/<runset>/manifest.json` with per-file sha256, macro maps, bench commit/toolchain provenance). `RUNSET=<class|all>` filtering is done by writing a derived configure.json subset (records verbatim) into `out/<runset>/configure.json` — never editing the tracked registry. Add `platform/riscv64-nemu-linux/out/` to `./.gitignore`.
   → coder verify (auto): the four files exist; `python3 platform/riscv64-nemu-linux/corpus-manifest.py --help` exits 0; `.gitignore` contains the `out/` line; `grep -n 'OBJDUMP' platform/riscv64-nemu-linux/build-corpus.sh` shows the knob export
   → coder verify (scripted): script at `./tmp/verification/2026-10-11-riscv64-nemu-linux-platform-step4.py` — runs `build-corpus.sh` with `RUNSET=acc`, then asserts: manifest reported 0 mismatches; every staged `bench/` binary and the staged `run-test` are ELF64 riscv64 (machine 243); the corpus-build log's extractor audit line names `riscv64-linux-gnu-objdump`; the staged `lib/` set covers every NEEDED entry of every staged binary; re-running the script is idempotent (same manifest); `git status --short` shows no generated files (clean-tree rule)
   → tester verify: `image-layout` (cpio content) and `guest-e2e-acc` (staged tree actually runs)

5. **Step 5:** Create the image builder: `./platform/riscv64-nemu-linux/build-image.sh` — clone/checkout pinned inputs (workload-builder `b7c321dec6ad6f4c78abcaefc722b59b4043503c`; OpenSBI `cbf9f6734dd85a982c63e3cb5db7ffe09da839ca` = v1.9; LibCheckpointAlpha `5169da768d18f4682a489eaabe7c071df6d6ef88`; kernel tarball `linux-6.18.56.tar.xz` verified against sha256 `4633bbda2ab95eaf2a7af4041236451d6759c72e6946265531af4b3aacefbf89`; busybox `busybox-1.36.1.tar.bz2` verified against sha256 `b8cc24c9574d809e7279c3be349795c5d5ceb6fdf19ca709f80cde50e47de314`); build in order: busybox (static defconfig), kernel (`ARCH=riscv CROSS_COMPILE=…` with wb's `br2-external/board/openxiangshan/nemu/linux.config` minus the one `CONFIG_INITRAMFS_SOURCE=` line plus the pinned `CONFIG_RISCV_ISA_V=y` / `CONFIG_RISCV_ISA_V_DEFAULT_ENABLE=y` pair, `olddefconfig`, `-j Image WERROR=0`, then both symbols asserted against the built `.config`; the Image is cached per kernel version and padded so that the assembler's MiB-aligned initramfs clears the kernel's runtime `_end` — unpadded it lands inside the kernel's reserved range and the kernel discards it), gcpt (`wb/scripts/build-gcpt.sh $wb/bootloader/LibCheckpointAlpha out/gcpt`, `DEFAULT_DTB=xiangshan`), OpenSBI (`wb/scripts/build-sbi.sh $wb/bootloader/opensbi out/opensbi`, `DEFAULT_DTB=xiangshan`); assemble the rootfs tree (busybox `_install` + `/init` symlink, `/etc/inittab`, `guest/bench.sh` at the inittab target, `/bench/` from step 4's staging, built `nemu-trap`, `test/sys_info.txt`); `find . | cpio -o -H newc --owner=0:0` → `out/<runset>/rootfs.cpio`; finally `CROSS_COMPILE=… DTC=dtc DEFAULT_DTB=xiangshan FIRMWARE_OUTPUT=out/<runset>/fw_payload.bin bash $wb/scripts/build-firmware-linux.sh out/gcpt/build/gcpt.bin out/opensbi out/generated-dts out/kernel/Image out/<runset>`; record sha256 + provenance into `out/<runset>/meta.json`. New guest overlay files: `guest/inittab` (`::sysinit:/bin/mount -t proc proc /proc`, `::sysinit:/bin/mount -t sysfs sysfs /sys`, then `::once:/bench/bench.sh` — the bench's support library reads `/proc/self/maps`), `guest/bench.sh` (prints `===CSB-RESULTS-JSON-BEGIN/END===` around `results.json`, same for `variables.json` and `results.dat`, runs `cd /bench && ./run-test no-make corpus exhausted-run`, captures `$?`, dumps, then `nemu-trap $rc`), `guest/nemu-trap.c` (argv[1]-as-exit-code, `asm volatile("mv a0, %0; .word 0x0005006b")` — encoding verified against NEMU `src/isa/riscv64/instr/decode.c:107` and workload-builder's `nemu-trap/main.c`; comment records both sources).
   → coder verify (auto): files exist; `grep -n '0x0005006b' guest/nemu-trap.c`; the three pinned commit shas appear in `build-image.sh`
   → coder verify (scripted): script at `./tmp/verification/2026-10-11-riscv64-nemu-linux-platform-step5.py` — runs `build-image.sh RUNSET=acc` (long; background) then asserts: `fw_payload.bin` exists, size < 128 MiB; byte-compare image at the OpenSBI (1 MiB), DTB (2 MiB, FDT magic), kernel (4 MiB, first MiB of `Image`) offsets; cpio at the MB-aligned offset parses and lists `/bench/run-test`, `/etc/inittab`, `/bench/test/<program>__<key>` entries; `meta.json` carries all pins
   → tester verify: `image-layout` (independent scan + cpio parse)

6. **Step 6:** Create the run harness: `./platform/riscv64-nemu-linux/run.sh` — `timeout` around `"$NEMU_BIN" -b out/<runset>/fw_payload.bin >boot.stdout 2>boot.stderr` (guest console = stderr, per the existing experiment scripts), extract the three marker bodies into `out/<runset>/runs/<timestamp>/{results.json,variables.json,results.dat,boot.log,meta.json}`, assert NEMU exit code == the `nemu-trap` code, and print a completeness line (`records in results.json: N/246`). Create `./platform/riscv64-nemu-linux/run-native.sh` — the same extraction-free runner for the host-native reference run (`./run-test exhausted-run` in a scratch tree seeded with the same derived configure.json), writing `results.dat` and `results.json` under `out/native/`.
   → coder verify (auto): both scripts exist and are executable-when-run-via-`bash`; `run.sh` references `HIT GOOD TRAP` and `records in results.json`
   → coder verify (scripted): script at `./tmp/verification/2026-10-11-riscv64-nemu-linux-platform-step6.py` — runs `run.sh RUNSET=acc` (background) after step 5's image, asserts: exit 0, `boot.log` contains OpenSBI banner + kernel banner + markers + `HIT GOOD TRAP`, extracted `results.json` contains exactly the acc record keys, `meta.json` has image/NEMU sha256 fields
   → tester verify: `guest-e2e-acc` (full independent rerun)

7. **Step 7 (tester):** Boot and verify the full suite: build `RUNSET=all` image (steps 4→5) and run it (step 6); assert all 246 records reported and tabulate the result distribution per category; re-run the `acc` image end-to-end once more for reproducibility of the pipeline (same image sha256 → same record key set; verdict drift, if any, quantified and reported).
   → tester verify: `guest-e2e-full` evidence; a discrepancy-free `results.json` record count of 246; every uncaught-signal (256-511) or unexpected code occurrence listed by record name with its `expect-results` annotation

8. **Step 8 (tester):** Produce the native-reference comparison: run `run-native.sh` for the full suite on the host, then build the per-category distribution table (NEMU-guest vs host-native) and classify every cell; report discrepancies by record with a one-line hypothesis each. Run the negative controls listed in the tester test list (missing-corpus-file, manifest mismatch by renaming a staged file, image-layout magic scan) and record raw evidence paths under `./tmp/`.
   → tester verify: `native-reference-compare` table present; negative controls exercised with captured non-zero exits/algorithmic failures; `make clean`-clean tree confirmed at the end

## Guidance for Planner

Documentation tasks you will execute yourself (not the coder):

- **Files to modify:**
- `./CLAUDE.md`
- `./README.md`
- `./doc/architecture.html`
- `./platform/riscv64-nemu-linux/README.md` (new)
- **`.claude/mega-audit-files.json`:** this plan creates a user-facing `platform/` tree. Adding `"platform/"` as a declared audit location is recommended but is repo policy — **confirm with the user before editing the file**; if not approved, leave it untouched.
- **When:** doc sync before coder start is limited to nothing; all four doc files are written/updated **after coder finishes** (so they describe the as-built scripts), except that `doc/architecture.html`'s scheduler section must match the final `run-test.cpp` exactly and its verification depends on coder step 3 being done.
- **What to sync:**
  - `doc/architecture.html` (declared "keep in sync with `scheduler/run-test.cpp`'s algorithm"): add the `corpus` option to the option description, document the corpus path contract (`test/<program>__<key>`, key = FNV-1a-32 over the sorted `K=V` map, empty map = `811c9dc5`), the forced-`-B` rule, and the missing-file fail-fast. Do not touch the result-code contract or the `configure.json` field set.
  - `README.md`: add a short "Running on NEMU (riscv64 Linux guest)" entry pointing at `platform/riscv64-nemu-linux/README.md`, plus the one-line corpus-mode usage.
  - `CLAUDE.md`: extend Commands (corpus-mode two-liner), Structure (the `platform/` tree), and the scheduler-feature description; no rule changes.
  - `platform/riscv64-nemu-linux/README.md`: the recipe reference — pinned inputs (wb `b7c321d`, OpenSBI v1.9 `cbf9f67`, LibCheckpointAlpha `5169da7`, Linux 6.18.56 + sha256, busybox 1.36.1 + sha256, NEMU @ae1ec3cd sha256-recorded), the three-command usage (`build-corpus.sh` → `build-image.sh` → `run.sh`), `RUNSET=acc|all` semantics, output layout, and the known caveats (no per-case timeout; 128 MiB guest RAM; console-only result extraction; the guest mounts `/proc` and `/sys` itself for the bench's `/proc/self/maps` readers; the corpus staging carries two run-time support files beyond the `test/<program>__<key>` set).

## History

One row per event (spec revision / mega-audit / review / implementation round). Conclusion = one-sentence outcome or what changed. Dismissed = compact dismissed-findings list for the reviewer — verbatim issue slugs linked as `[<slug>](<report-path>)`, no prose-only entries.

| Date | Event | Conclusion | Dismissed |
|------|-------|------------|-----------|
| 2026-10-11 | Initial plan | Researcher-authored from the 2026-10-10/11 NEMU Linux-route research (Experiment F + workload-builder recipe archaeology); MVP scope: corpus feature + pinned image recipe + in-guest run + native-reference acceptance. | — |
| 2026-10-20 | Spec revision (re-scope) | Currency re-scope against the 2026-10-19/20 bench world: Step 4 now passes the mandatory `$(OBJDUMP)` knob (defaults = the env repo's riscv64-lane pins, so the NEMU and qemu riscv64 corpora share one compiler) and drops the obsolete shim-dir workaround; counts re-verified (246 records / 103 programs; **145 build units / 25 distinct maps, ≤ 4 per program** — corrected at implementation: this row's re-scope figures of 146/37/≤5 were a miscount of the same, never-changed registry; longest corpus path still 63 chars); the out-of-scope line drops the `run-test continue` bug (fixed at `c8c6000`) and names the env-lane integration as the successor cycle; NEMU pin re-verified (`ae1ec3cd`, the system-mode binary present and current); the recipe's kernel/busybox tarballs are already cached sha256-matching in the env repo's downloads dir. | — |
| 2026-10-20 | Spec revision (coder-round outcome) | Guest kernel pin 6.1.83 → **6.18.56** (plus the pinned `CONFIG_RISCV_ISA_V` / `CONFIG_RISCV_ISA_V_DEFAULT_ENABLE` pair, asserted against the built `.config`) because the pinned cross toolchain's glibc is RVV-using and kernels before 6.5 have no userspace vector support — measured: 6.1.83 SIGILLs a static busybox at init and a dynamic ld-linux before any user code, 6.18.56 boots the corpus clean (all 22 acc records, `HIT GOOD TRAP`) and agrees 22/22 record-for-record with the host-native reference. Counts re-derived with the corpus model (145 units / 25 distinct maps / ≤ 4 per program; 17 multi-variant programs unchanged). Step 4/5 text records the as-built mechanics: `exhausted-run` in both corpus phases, the two run-time support files in the staging, the guest `/proc`+`/sys` mounts, the per-version kernel Image cache and its padding for the initramfs placement. The step-verification scripts under `./tmp/verification/` were authored during the coder round and now exist. Deviation ratified by the plan owner 2026-10-20 after a XiangShan-side audit found no kernel-version pin and no objection (report: `tmp/reports/2026-10-11-riscv64-nemu-linux-platform-coder-2026-10-20.json`). | — |
| 2026-10-20 | Implementation round (coder rounds 1–2, tester rounds 1–2) | Delivered: the scheduler's `corpus` mode (FNV-1a-32 naming contract, forced `-B`, snapshot-then-run, missing-file fail-fast) and the `platform/riscv64-nemu-linux` venue (corpus pipeline, pinned image recipe, run harness, native-reference runner, guest overlay), plus the planner doc sync. Tester round 2 **pass**: the full image boots to 246/246 records with `HIT GOOD TRAP` and 244/246 record-for-record agreement with the host-native reference (both differing records traced, neither caused by this plan), both runsets' images are byte-reproducible (`acc` `e9ba5f9c…`, `all` `0e50a9a8…`), and an independent full corpus rebuild reproduced the staged tree byte-for-byte (184/184 files), so the fixes moved archived mtimes and order only. Round 1's two blocking findings (F1 comma-bearing macro values in the manifest, F5 rootfs mtimes) are fixed and independently re-verified. | [testcase-return-code-delivery](./tmp/reports/defer-issue-testcase-return-code-delivery.json) (finding F2 — bench/registry-side, owned by that open issue, excluded from this plan's verdict and recorded rather than dropped), [corpus-build-registry-swap-crash-safety](./tmp/reports/defer-issue-corpus-build-registry-swap-crash-safety.json) (found in-round, deferred so the verified bytes stay verified) |
| 2026-10-20 | Phase 6 review + fix round (tester round 3) | Review verdict *Issues found — non-blocking* (4 Warning / 6 Suggestion, no Critical): per-class `RUNSET`s aborted at staging, the manifest's collision check could not fail, the host legs inherited the exported cross toolchain, and `run.sh` accepted an aborted guest as success. All four fixed plus the actionable Suggestions (README measurements, the swap wording, the contract page's injectivity assumption and `results.json`-shape claim, timeout diagnostics, a `bench_tree` provenance field), each with a proof. Tester round 3 **pass**: an `mts` class image built and booted end to end with zero verdict drift against the full run, the abort path exercised on a real broken image (run dir holds only `boot.log`), the forced-collision and host-leg controls green, and the acc/all boots re-confirmed at 22/22 and 246/246 with zero drift. | [corpus-build-registry-swap-crash-safety](./tmp/reports/defer-issue-corpus-build-registry-swap-crash-safety.json) (broadened to the concurrency half — review finding 7), [testcase-return-code-delivery](./tmp/reports/defer-issue-testcase-return-code-delivery.json) (F2, unchanged and out of scope) |

## Plan Metadata

```json
{
  "plan_id": "2026-10-11-riscv64-nemu-linux-platform",
  "steps": [
    "Step 1: Make toolchain selection environment-overridable in the Makefile",
    "Step 2: Add corpus build mode to the scheduler",
    "Step 3: Add corpus run mode and missing-file guard to the scheduler",
    "Step 4: Platform corpus pipeline (env.sh, build-corpus.sh, corpus-manifest.py)",
    "Step 5: Platform image builder (pinned recipe, guest overlay, nemu-trap)",
    "Step 6: Platform run harness and native-reference runner",
    "Step 7: (tester) Full-suite guest verification and distribution tabulation",
    "Step 8: (tester) Native-reference comparison and negative controls"
  ],
  "coder_files": [
    "./Makefile",
    "./scheduler/run-test.cpp",
    "./platform/riscv64-nemu-linux/env.sh",
    "./platform/riscv64-nemu-linux/build-corpus.sh",
    "./platform/riscv64-nemu-linux/corpus-manifest.py",
    "./platform/riscv64-nemu-linux/build-image.sh",
    "./platform/riscv64-nemu-linux/run.sh",
    "./platform/riscv64-nemu-linux/run-native.sh",
    "./platform/riscv64-nemu-linux/guest/inittab",
    "./platform/riscv64-nemu-linux/guest/bench.sh",
    "./platform/riscv64-nemu-linux/guest/nemu-trap.c",
    "./.gitignore"
  ],
  "tester_files": [
    "./tmp/verification/2026-10-11-riscv64-nemu-linux-platform-step1.py",
    "./tmp/verification/2026-10-11-riscv64-nemu-linux-platform-step2.py",
    "./tmp/verification/2026-10-11-riscv64-nemu-linux-platform-step3.py",
    "./tmp/verification/2026-10-11-riscv64-nemu-linux-platform-step4.py",
    "./tmp/verification/2026-10-11-riscv64-nemu-linux-platform-step5.py",
    "./tmp/verification/2026-10-11-riscv64-nemu-linux-platform-step6.py"
  ],
  "doc_files": [
    "./CLAUDE.md",
    "./README.md",
    "./doc/architecture.html",
    "./platform/riscv64-nemu-linux/README.md"
  ],
  "verification_scripts": [
    "./tmp/verification/2026-10-11-riscv64-nemu-linux-platform-step1.py",
    "./tmp/verification/2026-10-11-riscv64-nemu-linux-platform-step2.py",
    "./tmp/verification/2026-10-11-riscv64-nemu-linux-platform-step3.py",
    "./tmp/verification/2026-10-11-riscv64-nemu-linux-platform-step4.py",
    "./tmp/verification/2026-10-11-riscv64-nemu-linux-platform-step5.py",
    "./tmp/verification/2026-10-11-riscv64-nemu-linux-platform-step6.py"
  ],
  "repo_mode": "Public",
  "document_overrides": []
}
```

## Audit Accumulation

*Counters reset when mega-audit runs. Used by Phase 5 entry trigger.*

| Counter | Value | Last Updated |
|---------|-------|--------------|
| issues_resolved_since_audit | 0 | 2026-10-11 |
| steps_changed_since_audit | 0 | 2026-10-11 |
| files_changed_since_audit | 0 | 2026-10-11 |

## Documentation

Keep-in-sync duties this plan creates or touches (all owned by the planner, listed again in Guidance for Planner):

- `doc/architecture.html` — **keep in sync with `scheduler/run-test.cpp`'s algorithm**: the corpus option, naming contract, forced `-B`, fail-fast guard.
- `doc/test-catalog.html` — untouched: no record or program is added, removed, or renamed; `script/check_catalog.py` must keep passing (run it as a sanity gate after implementation).
- `doc/porting.html` — untouched: no new ISA or OS layer is added (riscv64/posix already exists); the platform is a *venue*, described in its own README.
- `README.md` — the NEMU-platform pointer and corpus-mode usage.
- `CLAUDE.md` — commands, structure, scheduler description.

## Final Results

**Status:** COMPLETED
**Completed:** 2026-10-20

### What was implemented

The benchmark can now run on a venue it could not run on before — a booted Linux guest under XiangShan NEMU — and the delivery has two halves: the scheduler feature that makes a build-elsewhere/run-here split possible, and the venue itself.

1. **Scheduler `corpus` mode (Steps 1–3).** A new option word splits the two phases across machines: the build phase forces `-B` for every record and snapshots each fresh build as `test/<program>__<key>` (`<key>` = FNV-1a-32 over the record's macro map rendered as sorted, comma-joined `K=V` pairs, `%08x`; empty map → `811c9dc5`), and the run phase executes that keyed path, exiting `1` and naming the file when it is missing rather than recording a result for a case that never ran. The snapshot is what keeps variant records honest: the in-place `test/<program>` is overwritten whenever the macro map changes, so a plain `make-only`/`no-make` split would run whichever variant was built last. `CXX`/`ASM`/`OBJDUMP` became `?=` so the environment can steer the child `make` invocations the scheduler spawns — the only path a cross toolchain has to them, since those children receive no extra arguments. Non-corpus behavior is unchanged, and that is the load-bearing invariant: the tester's parity control asserted that a non-corpus run of the same subset yields the same record-key set with no new anomaly codes, and seven archived runs across three image shas show zero per-record verdict drift.

2. **The `platform/riscv64-nemu-linux` venue (Steps 4–6).** `build-corpus.sh` builds the host scheduler, derives the `RUNSET` subset, drives the corpus build, cross-builds the riscv64 scheduler, stages `bench/` plus the `NEEDED`/`PT_INTERP` library closure resolved through the cross compiler, and gates on `corpus-manifest.py` (an independent FNV-1a-32 model that asserts the staged set equals the expected unit set, per-program key uniqueness, and the 63-character path budget). `build-image.sh` assembles the bootable image from pinned inputs — static busybox, the kernel `Image` with the vector symbols asserted against the built `.config`, gcpt, OpenSBI, the rootfs tree, `cpio` — pads the Image so the MiB-aligned initramfs clears the kernel's runtime `_end`, hands the pieces to workload-builder's assembler, and writes `meta.json`. `run.sh` boots under a host timeout and carves `results.json`/`variables.json`/`results.dat` out of the console log between `===CSB-...===` markers, cross-checking the guest's exit code, NEMU's exit status and its trap line. `run-native.sh` produces the host-native reference distribution in a scratch tree. The guest overlay mounts `/proc` and `/sys` before the bench runs (the support library reads `/proc/self/maps`, and without it the stack-region ASLR cases misreport — measured: guest `0` where native scores `1`) and hands its exit code to NEMU through `nemu-trap` (`.word 0x0005006b`, `a0` as the code).

3. **Kernel pin 6.1.83 → 6.18.56 (a ratified deviation).** The pinned cross toolchain's glibc is RVA23U64 vector-using code and kernel-side userspace vector support only exists from Linux 6.5; on 6.1.83 every guest process dies with SIGILL on its first vector instruction — measured: a static busybox dies in a glibc `memset` at init, and a dynamic binary dies inside `ld-linux` before any user code runs. The recipe pins `CONFIG_RISCV_ISA_V` and `CONFIG_RISCV_ISA_V_DEFAULT_ENABLE` into the config and asserts both against the built `.config`, exactly as the env repo's riscv64 lane does. Ratified by the plan owner 2026-10-20 after a XiangShan-side audit found no kernel-version pin and no objection.

4. **Planner doc sync.** `doc/architecture.html` gains the corpus-mode section (the scheduler source cites that page as carrying the same contract); `README.md` the NEMU entry, the corpus usage and the two stale spots (the quoted help block, the repo contents); `CLAUDE.md` the Commands, Structure and scheduler text; `platform/riscv64-nemu-linux/README.md` is new — the recipe reference, the pin table, the output layout and the venue's caveats.

### Files changed

| File | Change |
|---|---|
| `Makefile` | `CXX`/`ASM`/`OBJDUMP` (and Darwin's `CXX`) become `?=`, so the environment can steer the scheduler's child `make`s |
| `scheduler/run-test.cpp` | `corpus` option word; `corpus_key`/`corpus_bin_path`/`corpus_snapshot`; `arg_pool[32][256]`; forced `-B` in corpus builds; corpus run path with the missing-file fail-fast |
| `.gitignore` | `platform/riscv64-nemu-linux/out/` and `__pycache__/` |
| `platform/riscv64-nemu-linux/env.sh` | **new** — every knob: cross toolchain (the env repo's riscv64-lane pins by default), `NEMU_BIN`, `OUT`, `RUNSET`, `NEMU_TIMEOUT`, `JOBS`, `SOURCE_DATE_EPOCH` |
| `platform/riscv64-nemu-linux/build-corpus.sh` | **new** — host scheduler; derived subset; corpus build; cross scheduler; staging + library closure; manifest gate |
| `platform/riscv64-nemu-linux/corpus-manifest.py` | **new** — independent FNV-1a-32 model, staged-set/key/path-budget assertions, deterministic `manifest.json` |
| `platform/riscv64-nemu-linux/build-image.sh` | **new** — pinned fetch/checkout; busybox; kernel Image (+ vector-symbol assertion, per-version cache, padding); gcpt; OpenSBI; rootfs; cpio; `fw_payload.bin`; `meta.json` |
| `platform/riscv64-nemu-linux/run.sh` | **new** — boot under `timeout`, marker extraction, guest/NEMU/trap agreement, run `meta.json` |
| `platform/riscv64-nemu-linux/run-native.sh` | **new** — the host-native reference run in a scratch tree |
| `platform/riscv64-nemu-linux/guest/inittab` | **new** — `proc`/`sys` mounts, then `::once:/bench/bench.sh` |
| `platform/riscv64-nemu-linux/guest/bench.sh` | **new** — guest driver: run the corpus, print the result files between markers, `nemu-trap` the exit code |
| `platform/riscv64-nemu-linux/guest/nemu-trap.c` | **new** — the trap helper, with both instruction-source citations in its header |
| `platform/riscv64-nemu-linux/README.md` | **new** (planner) — recipe reference, pins, output layout, caveats |
| `doc/architecture.html` | corpus-mode section + the two scheduler-list cross-references (planner) |
| `README.md` | NEMU entry, corpus usage, `corpus` line in the quoted help block, `platform` in the repo contents (planner) |
| `CLAUDE.md` | Commands (corpus two-liner), Structure (`platform/`), scheduler description (planner) |

### Test results

Tester round 1 ([report](./tmp/reports/2026-10-11-riscv64-nemu-linux-platform-tester-2026-10-20.json)) — **fail**: two blocking coder-owned findings, F1 (`RUNSET=all` produced no `manifest.json`; `corpus-manifest.py` crashed re-splitting macro values that contain commas) and F5 (the image was not byte-reproducible — 11 of 454 cpio members carried wall-clock mtimes), plus three smaller items (D2 `meta.json` toolchain, D3 a garbled padding report line, D4 an unignored `__pycache__`). The coder fix round ([report](./tmp/reports/2026-10-11-riscv64-nemu-linux-platform-coder-2026-10-20-2.json)) fixed F1/F5/D2/D3/D4 and dispositioned F2 into its existing open issue.

Tester round 2 ([report](./tmp/reports/2026-10-11-riscv64-nemu-linux-platform-tester-2026-10-20-2.json)) — **pass**, re-run independently over the fixed bytes:

- `scheduler-corpus-matrix` 24/24 (unchanged script): 22 forced `-B` builds = the independent model, every snapshot `0755` and executable, the missing-file control exits 1 naming the file with the victim unrecorded, the non-corpus parity control green, and the tracked registry's sha unchanged.
- `image-layout` **22/22 for `acc` and 22/22 for `all`** (was 17/21 for `all`): the full runset's manifest now exists, its unit set equals the independent model (145 = 145), every cpio corpus member's sha256 equals the manifest's (145/145), and the manifest-mismatch control became a genuine exercise.
- `guest-e2e` 50/51 with three fresh tester boots — `acc` (`out/acc/runs/20261020-145013`), `all` (`out/all/runs/20261020-145017`), and the `repro` section (`out/acc/runs/20261020-145051`). The single red check is F2, excluded by scope and left visible in the raw logs.
- The F5 regression is green as a three-way agreement: on-disk `acc` image → build 1 → build 2 all `e9ba5f9c…`; the full image rebuilds to `0e50a9a8…` exactly; and seven archived runs spanning three image shas show identical histograms and zero per-record verdict differences, so the fixes moved archived mtimes and order only.
- `native-reference-compare` 6/7 (same exclusion): 246 vs 246 vs configured, key sets equal, **244/246 record-for-record agreement**, two differing records both traced — F2 (`267` guest vs `0` native, the open return-code issue) and `mts-access-after-reclaim-heap` (`0` vs `1`, both legal outcomes for that record) — and one anomaly shared by both venues (255, pre-existing).
- Side checks: an independent full `RUNSET=all` corpus rebuild reproduced the staged tree byte-for-byte (184/184 files), `script/check_catalog.py` exits 0, and the doc-structure gate passes on all five HTML pages.

Against the acceptance criteria: (1) `make` and `./run-test exhausted-run` on the host are unaffected (parity control, catalog guard); (2) the `acc` image boots with all 22 records and NEMU exit 0 with `HIT GOOD TRAP`; (3) the full image reports all 246 records with the distribution compared against the host-native reference and every discrepancy named and traced; (4) provenance is recorded for both runsets (per-file sha256 manifest, image and rootfs sha256, NEMU sha256, toolchain version, bench commit); (5) the tree is clean of generated artifacts and the image is byte-reproducible. Full-suite cost in the venue, from the tester's own full run (`out/all/runs/20261020-145017`): the entire 246-record suite in one guest boot — 2,515,823,171 guest instructions in 31.6 s of host time (NEMU's own statistic, ≈79.6 M instr/s).

A Phase 6 adversarial review then ran over the same diff ([report](./tmp/reports/2026-10-11-riscv64-nemu-linux-platform-review-2026-10-20.json)), found ten issues — none Critical, four Warnings that are real defects in the delivered scripts — and the fix round that followed was re-verified independently.

Tester round 3 ([report](./tmp/reports/2026-10-11-riscv64-nemu-linux-platform-tester-2026-10-20-3.json)) — **pass**, every fix verified by the tester's own constructions:

- **Class runset, end to end** (the fix round's headline): `RUNSET=mts build-corpus.sh` → `build-image.sh` → `run.sh`, all with the documented cross override exported — corpus exit 0 with a 14-unit/14-program manifest whose support set correctly omits every acc-only file, image `aa6613586ea5008a…` (29,105,152 B), boot exit 0, 16/16 records, `HIT GOOD TRAP` (`out/mts/runs/20261020-153328`); a purpose-written `class-runset-compare.py` (5/5) shows **zero verdict drift record-for-record** against the same records in the full run, with no new anomalies; the class image's layout audited independently (22/22).
- **The abort path on a real broken image**, not a synthetic one: one corpus file deleted from the acc staging, the image rebuilt and booted — `run.sh` failed, named the in-guest scheduler exit, and the run directory holds **only `boot.log`** (`out/acc/runs/20261020-153409`), where before the fix it exited 0 and published a partial run. Victim restored.
- **The collision check** forced with a stubbed hash in the tester's own copy: exits 1 naming 11 distinct canonical maps, while the unmodified script passes the real staged tree.
- **Host-leg isolation**, statically and at runtime: the exported override leaks into a bare `make` and not into the scripts' `env -u` form; `run-native.sh` completes with the override exported and the native reference under `out/native/` stayed byte-identical.
- **Re-confirmations**: fresh boots through the new `run.sh` — acc 22/22 (`out/acc/runs/20261020-153635`) and all 246/246 (`out/all/runs/20261020-153638`) — with key sets and histograms identical to round 2 and zero drift; `image-layout` 22/22 for both runsets; `check_catalog.py` exit 0; the tracked registry byte-unchanged (`2d2c3cbf91c4a202…`) after every build.

One expected consequence, recorded rather than smoothed over: exercising the override path re-staged the acc corpus, which refreshed the guest's `sys_info.txt` with the new `# Bench tree:` line, so the accepted acc image legitimately moved `e9ba5f9c…` → `da90bc25838a27f3…` (the header line is the only difference; two consecutive builds agree and it boots identically — 22/22, zero drift). The `all` image was not re-staged: it remains `0e50a9a8c50297e7…`, internally consistent with its own staging and manifest, and was booted fresh by the tester at 246/246. Both were built from the pre-commit tree, so the acc artifacts carry `bench_tree = 74195c4-dirty` while the `all` staging predates that field and carries the commit alone — the successor cycle's lane re-run is where the provenance becomes a committed commit.

### Issues resolved

- **F1 — `corpus-manifest.py` crashed on comma-bearing macro values** (blocking): the manifest now carries the record's `conf-macro` entry instead of re-splitting the canonical string, so values like `VAL_4U,VAL_2U,VAL_1U` round-trip; `RUNSET=all` exits 0 with a 145-unit manifest, and the canonical string — hence every FNV key and corpus file name — is unchanged.
- **F5 — the image was not byte-reproducible** (blocking): stage 5 now stamps every rootfs entry to `SOURCE_DATE_EPOCH` (`touch -h`) and feeds `cpio` an `LC_ALL=C`-sorted list; two consecutive builds agree for both runsets.
- **D2/D3/D4** — `meta.json` gained the top-level `toolchain` field, the padding line prints real hex, and `__pycache__/` is ignored (verified against `git ls-files`, so the broad pattern masks no tracked file).

### Deferred issues (open, by design)

Each is a *completed* disposition — correctly identified and correctly scoped out — not unfinished work.

- **`corpus-build-registry-swap-crash-safety`** — report: `./tmp/reports/defer-issue-corpus-build-registry-swap-crash-safety.json`. `build-corpus.sh` makes the derived registry visible to the scheduler by swapping it over the tracked `configure.json` and restoring it from an EXIT trap; a SIGKILL inside that window runs no trap, so for a class runset the work tree would be left holding a subset. Found in-round (the `M configure.json` seen during the tester's run was the swap in flight, restored six seconds later), deferred rather than fixed so the verified bytes stay verified; the remedy is to build in a scratch tree the way `run-native.sh` already does.
- **`testcase-return-code-delivery`** — report: `./tmp/reports/defer-issue-testcase-return-code-delivery.json`. Pre-existing and open before this plan, which did not file it: it owns finding F2 (`cfi-jump-func-ra-from-heap-memcpy-implicit-arith` = `267` on riscv64 against a declared `290`), a bench-side gvar-as-exit-status defect visible in every riscv64 venue. This plan appended a dated corroboration paragraph instead of filing a duplicate, and excluded F2 from its verdict while leaving both flagging checks red in the raw logs. Residual risk, recorded: until that issue is worked, an acceptance run of this platform must read the distribution table, not only the exit status.

### Review

Phase 6 adversarial review ([report](./tmp/reports/2026-10-11-riscv64-nemu-linux-platform-review-2026-10-20.json)) — verdict **Issues found — non-blocking**: 10 findings (4 Warning, 6 Suggestion), no Critical. The reviewer read every changed file in full, re-derived the corpus model from `configure.json` independently — the FNV-1a-32 contract agrees byte-for-byte between the C++ and the Python for all 145 units, comma-bearing values included — checked the image arithmetic against the real `System.map`, and sampled the coder/tester claims against the artifacts.

Every Warning and the actionable Suggestions were fixed in the follow-up round, each with a proof, then re-verified independently by the tester (round 3 above):

| Finding | Disposition |
|---|---|
| **W1** — per-class `RUNSET`s aborted at stage [5/6]: the support-file loop demanded acc-only files unconditionally | Fixed: a file is required only when the selected subset contains the program that reaches it; proven by building and booting an `mts` image |
| **W2** — the manifest's per-program collision check could never fail (the collection was keyed by the hash it was testing) | Fixed: distinct canonical maps are collected per key; proven with a forced collision (fires, names both maps) against the unmodified script's clean pass |
| **W3** — the host legs inherited an exported cross toolchain, cross-compiling the scheduler that stage [3/6] must then execute | Fixed: `env -u CXX -u ASM -u OBJDUMP` on both host legs; proven statically (`make -n`) and at runtime |
| **W4** — `run.sh` accepted an aborted guest as success and never checked completeness | Fixed: a non-zero guest exit or an incomplete `results.json` fails the run, a timeout is diagnosed before the markers are parsed, and nothing is published unless the run is proven complete; proven on a real broken image |
| **S5** — `build_plan` modelled argument containers as lists only, where the registry writes objects | Fixed: both shapes are counted, with the index-0 floor `arg_parser` implies |
| **S6** — the canonical rendering is not injective (a value containing `,KEY=` aliases two build units) | Documented as an explicit contract assumption in `doc/architecture.html`; escaping the rendering would change every corpus key and every staged name, so it is left to a successor rather than smuggled into this commit |
| **S7** — the registry swap and the staging mutate shared state with no lock | Recorded in the deferred issue, whose title, problem and fix now cover the concurrency half as well as the crash window |
| **S8** — the README's memory figures were stale pre-pin estimates | Fixed to the measured values (kernel Image 14,084,664 B; cpio 9,377,280 / 13,663,232 B; images 29,300,224 / 33,586,176 B) |
| **S9** — provenance recorded only a commit that does not contain the code that produced the artifacts | Fixed in part: `bench_tree` (`git describe --always --dirty`) is now recorded in `sys_info.txt`, the manifest and both `meta.json`s; rebuilding the accepted artifacts after the commit belongs to the successor cycle |
| **S10** — the timeout branch was unreachable behind the marker check, and its message interpolated an unexported variable | Fixed with W4; the timeout value is passed in explicitly |
| Hunches 3, 4, 5 (the `results.json`-shape claim, the registry "never edited" wording, a same-second run directory) | Fixed in `doc/architecture.html`, the platform README and `run.sh` |
| Hunches 1, 2 (workload-builder's truncate-then-`notrunc` assembly; the padding report when no padding is needed) | Not reachable with the current pins; recorded in the review report rather than acted on |

The review's own `areas_not_fully_verified` states the residual limits honestly: cold-cache image reproducibility (the tester's evidence was taken with warm kernel and busybox caches, so byte reproducibility is demonstrated for the rootfs, cpio and assembly stages), the MSVC branch (read, never run), and F2's attribution (owned by its open issue).

### Successor

The env-cycle integration the out-of-scope clause names — consuming `out/<runset>/runs/<ts>/` as a lane — plus the deferred scratch-tree redesign above. Both are separate cycles; this plan's commit retires nothing (`testcase-return-code-delivery` stays open for its own workstream, and the swap report stays open for its redesign).