Running cpu-sec-bench on a booted Linux under NEMU (riscv64)
========================

This platform runs the benchmark inside a Linux guest on **XiangShan NEMU** (system mode,
riscv64): the test corpus is cross-compiled on the host, baked into a bootable image together
with the riscv64 scheduler, and the scheduler then runs the prebuilt binaries inside the guest.
It exists because the NEMU venue has no compiler and no writable network, and because a guest
initramfs is transient — so the bench's usual "compile each variant at run time" flow cannot
work there. The scheduler's `corpus` mode is what makes the split possible; see
[doc/architecture.html](../../doc/architecture.html).

The image follows the classic NEMU layout assembled by
[workload-builder](https://github.com/OpenXiangShan/workload-builder):

```
0x80000000 +0     gcpt (LibCheckpointAlpha startup, <= 1 MiB)
0x80100000 +1 MiB OpenSBI fw_jump
0x80200000 +2 MiB device tree (xiangshan: 128 MiB DRAM, 1 hart)
0x80400000 +4 MiB Linux Image
...        MB-aligned  initramfs (rootfs.cpio, uncompressed newc)
```

### Usage

Three commands, in order — each writes into `out/` and the second needs the first:

~~~bash
  platform/riscv64-nemu-linux/build-corpus.sh     # cross-build, stage, verify the corpus
  platform/riscv64-nemu-linux/build-image.sh      # assemble out/<runset>/fw_payload.bin
  platform/riscv64-nemu-linux/run.sh              # boot the image, extract the results
~~~

`RUNSET` selects which records go into the image — `acc` (the default, 22 records: the smoke
image), any single record class (`cfi`, `cpi`, `mss`, `mts`) or `all` (246 records):

~~~bash
  RUNSET=all platform/riscv64-nemu-linux/build-corpus.sh
  RUNSET=all platform/riscv64-nemu-linux/build-image.sh
  RUNSET=all platform/riscv64-nemu-linux/run.sh
~~~

The subset is applied by deriving a `configure.json` subset (records copied verbatim) into
`out/<runset>/` and, for the duration of the build, swapping it over the work tree's copy — the
scheduler always reads `configure.json` from its cwd — with an exit trap restoring the original.
A completed run never changes the committed registry, but the swap is visible while one is in
flight, and a hard kill (SIGKILL) inside that window would leave the subset in place: that
crash-safety gap is recorded as a deferred issue
(`tmp/reports/defer-issue-corpus-build-registry-swap-crash-safety.json` on the machine that
produced this platform — the `tmp/` tree is machine-local and never committed), together with
the concurrency hazard the same shared state carries; the remedy for both is to build in a
scratch tree instead of swapping the tracked registry.

A fourth script produces the host-native reference run — the same record subset compiled and
run directly on this workstation, which is what the guest's result distribution is compared
against (the port-verification method of [doc/porting.html](../../doc/porting.html)):

~~~bash
  RUNSET=all platform/riscv64-nemu-linux/run-native.sh
~~~

Everything tunable lives in [env.sh](env.sh): the cross toolchain (`CXX=riscv64-linux-gnu-g++-13`,
`CC=riscv64-linux-gnu-gcc-13`, `ASM`, `OBJDUMP`, `CROSS_COMPILE=riscv64-linux-gnu-` — the
versioned pins the `cpu-sec-test-env` riscv64 lane uses, so both riscv64 corpora share one
compiler), the NEMU binary (`NEMU_BIN`, defaulting to this workstation's
`/home/wsong83/proj/xiangshan/NEMU/build/riscv64-nemu-interpreter`, so override it anywhere
else), the work tree (`OUT`), the record set (`RUNSET`), the host-side wall-clock guard
(`NEMU_TIMEOUT`, 1800 s), and the fixed build timestamp (`SOURCE_DATE_EPOCH`, `1700000000`) that
keeps the image byte-reproducible. The workspace's
own `riscv64-unknown-linux-gnu-*` toolchain is the documented override — set `CXX`, `ASM`,
`OBJDUMP` and `CROSS_COMPILE` together.

### What each script does

**`build-corpus.sh`** (host, bench work tree, terminated by `make clean`):

1. builds the host scheduler;
2. derives the `RUNSET` subset of `configure.json` into `out/<runset>/`;
3. runs `./run-test corpus make-only exhausted-run` with `ARCH=riscv64` — the scheduler drives
   one forced-`-B` `make` per `(program, macro-map)` unit through the cross toolchain and
   snapshots each build as `test/<program>__<key>`;
4. cross-builds the riscv64 scheduler;
5. stages `out/<runset>/bench/` (scheduler, subset registry, corpus files, `lib/common/libcfi.so`,
   a guest-side `test/sys_info.txt`) and `out/<runset>/lib/` (the transitive `NEEDED` closure
   plus the `PT_INTERP` loader, resolved through the cross compiler's own search path);
6. verifies the staged tree with [corpus-manifest.py](corpus-manifest.py) — expected unit set
   equals the staged set, per-program keys collision-free, longest corpus path within the
   scheduler's 63-character budget — and writes `out/<runset>/manifest.json`.

Both scheduler invocations append `exhausted-run`: with a filtered subset a plain run aborts on
`require` edges that cross the subset boundary, and both phases then enumerate the same
argument kind, so the units built and the units run agree by construction.

**`build-image.sh`** (host): clones/checks out the pinned inputs into `out/src/`, builds a
static busybox and the kernel `Image` (cached per kernel version), builds gcpt and OpenSBI,
assembles the rootfs tree (busybox `_install`, `/init`, the guest overlay from
[guest/](guest/), `/bench/` from the staging, `/lib/` from the library closure, the built
`nemu-trap`), packs it with `cpio`, and hands the pieces to workload-builder's
`build-firmware-linux.sh`, which produces `fw_payload.bin` and the DTB that carries the
initramfs placement. `out/<runset>/meta.json` then records the image and rootfs sha256, the
NEMU binary's sha256, the bench commit, the corpus compiler and every input pin.

**`run.sh`** (host): boots the image as `timeout $NEMU_TIMEOUT $NEMU_BIN -b out/<runset>/fw_payload.bin`;
the guest console — including the `===CSB-...===` blocks that [guest/bench.sh](guest/bench.sh)
prints around the result files — lands in `runs/<ts>/boot.log`, and the results are carved out
of it between the markers. The guest hands its exit code to NEMU through
[guest/nemu-trap.c](guest/nemu-trap.c) (`.word 0x0005006b` with the code in `a0`), so the
harness can assert that the guest's own exit code, NEMU's exit status and its `HIT GOOD TRAP` /
`HIT BAD TRAP` line all agree. A timeout (exit 124) fails the run loudly instead of publishing
a partial one.

### Output layout

```
out/src/                 pinned checkouts (workload-builder, opensbi, LibCheckpointAlpha) + downloads
out/<runset>/            the venue: bench/ lib/ configure.json manifest.json meta.json
                         rootfs.cpio fw_payload.bin corpus-build.log scheduler-crossbuild.log
out/<runset>/runs/<ts>/  one boot: boot.log results.json variables.json results.dat meta.json
out/native/              the host-native reference run (+ tree/, its scratch copy of the bench)
out/busybox-1.36.1/  out/linux-6.18.56/  out/kernel/  out/gcpt/  out/opensbi/  out/rootfs/
```

`out/` is gitignored and machine-local.

### Pinned inputs

| Input | Pin |
|---|---|
| workload-builder | `b7c321dec6ad6f4c78abcaefc722b59b4043503c` |
| OpenSBI (v1.9) | `cbf9f6734dd85a982c63e3cb5db7ffe09da839ca` |
| LibCheckpointAlpha | `5169da768d18f4682a489eaabe7c071df6d6ef88` |
| Linux | 6.18.56, sha256 `4633bbda2ab95eaf2a7af4041236451d6759c72e6946265531af4b3aacefbf89` |
| busybox | 1.36.1, sha256 `b8cc24c9574d809e7279c3be349795c5d5ceb6fdf19ca709f80cde50e47de314` |
| NEMU | the workspace's `NEMU/build/riscv64-nemu-interpreter` (OpenXiangShan/NEMU `master` @ `ae1ec3cd`); its sha256 is recorded per run, not checked |

The kernel is 6.18.56 rather than the older 6.1.83 the bench's first NEMU experiments used, and
the reason is load-bearing: the guest userspace is built by the cross toolchain whose glibc is
RVA23U64 vector-using code, and kernel-side userspace vector support only exists from Linux 6.5
(`arch/riscv/kernel/vector.c`, `CONFIG_RISCV_ISA_V`). On 6.1.83 every guest process dies with
SIGILL on its first vector instruction — a static busybox dies in a glibc `memset` at init, and
a dynamic binary dies even earlier, inside `ld-linux` itself. The recipe therefore pins
`CONFIG_RISCV_ISA_V=y` and `CONFIG_RISCV_ISA_V_DEFAULT_ENABLE=y` into the config and asserts
both against the built `.config`; the `cpu-sec-test-env` riscv64 lane pins the same kernel for
the same reason. The kernel and busybox tarballs are seeded from that repo's
`build/downloads/` when present, so only the three git checkouts need network on a first build.

### Known caveats

- **No per-case timeout.** The scheduler has none and the guest gets none; the host-side
  `timeout` around NEMU is the only guard, so one hanging case costs the whole run. (Deferred by
  decision; the `run-test continue` bug that would have let a run resume was fixed at bench
  `c8c6000`.)
- **Results travel over the console.** This venue has no virtio-9p or shared filesystem: the
  guest prints its result files between `===CSB-...===` markers and the host carves them out of
  `boot.log`, which is kept as the raw evidence. A guest that hangs before printing them yields
  a boot log and a failed run, not a partial result file.
- **128 MiB of guest RAM, one hart.** Measured against the shipped artifacts: kernel `Image`
  14.1 MB, initramfs 9.4 MB (`acc`) / 13.7 MB (`all`), so the assembled images are 29.3 MB and
  33.6 MB — well inside the budget, which workload-builder's own DTS check also enforces.
- **The guest mounts `/proc` and `/sys` itself** (`guest/inittab`). The bench's support library
  reads `/proc/self/maps` to find a region's base address, so without `/proc` the stack-region
  ASLR cases misreport — measured: code `0` where the host-native reference scores `1`.
- **Two run-time support files ride along with the corpus.** `test/acc-check-ASLR` (the case
  re-invokes itself through a hard-coded relative path) and `test/acc-read-func-func-opcode.tmp`
  (the opcode side file the case reads) are not `(program, macro-map)` units, so they are staged
  explicitly. Without them those cases exit their "known failure point" code `2` inside the guest.
  Both serve acc programs, so a class runset that selects neither is not required to produce them.
- **The corpus is built by the host toolchain, not the guest's** — the guest has no compiler, by
  design. A toolchain bump therefore invalidates the corpus and the image together.
- **Not yet a lane.** This platform is standalone; wiring it into the `cpu-sec-test-env`
  dispatcher is a separate cycle, which consumes `out/<runset>/runs/<ts>/` as its input.