#!/usr/bin/env bash
#
# cpu-sec-bench platform "riscv64-nemu-linux": run the benchmark inside a
# booted Linux guest under XiangShan NEMU (system mode, riscv64).
#
# Sourced by the platform's scripts.  Everything tunable lives here, except
# the image recipe's own pins (workload-builder / OpenSBI / LibCheckpointAlpha
# commits, kernel and busybox tarballs), which live next to their use in
# build-image.sh.
#
# Toolchain defaults are the cpu-sec-test-env riscv64 lane's pins, so the NEMU
# guest corpus and the qemu riscv64 lane's corpus share one compiler.  The
# workspace toolchain is the documented override -- set all four knobs
# together, e.g.
#
#   CXX=riscv64-unknown-linux-gnu-g++ \
#   ASM=riscv64-unknown-linux-gnu-as \
#   OBJDUMP=riscv64-unknown-linux-gnu-objdump \
#   CROSS_COMPILE=riscv64-unknown-linux-gnu- \
#   platform/riscv64-nemu-linux/build-corpus.sh
#
# The corpus build and the run harness both honour these from the environment.

PLATFORM_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BENCH_DIR="$(cd "$PLATFORM_DIR/../.." && pwd)"

# cross toolchain (the scheduler's child makes receive these through the
# environment; see the Makefile's `?=` assignments)
CXX="${CXX:-riscv64-linux-gnu-g++-13}"
CC="${CC:-riscv64-linux-gnu-gcc-13}"
ASM="${ASM:-riscv64-linux-gnu-as}"
OBJDUMP="${OBJDUMP:-riscv64-linux-gnu-objdump}"
CROSS_COMPILE="${CROSS_COMPILE:-riscv64-linux-gnu-}"
# only used to read PT_INTERP when staging the guest's runtime libraries
READELF="${READELF:-${CROSS_COMPILE}readelf}"

# the NEMU binary that boots the guest image
NEMU_BIN="${NEMU_BIN:-/home/wsong83/proj/xiangshan/NEMU/build/riscv64-nemu-interpreter}"

# platform work tree (gitignored): out/<runset>/{bench,lib,fw_payload.bin,...}
OUT="${OUT:-$PLATFORM_DIR/out}"

# which records go into the image: "all" or a record-class prefix
# (acc, cfi, cpi, mss, mts)
RUNSET="${RUNSET:-acc}"

# host-side wall-clock limit around a guest run, in seconds; the guest has no
# per-case timeout (deferred by decision), so this is the only guard
NEMU_TIMEOUT="${NEMU_TIMEOUT:-1800}"

# parallel jobs for the kernel/busybox builds
JOBS="${JOBS:-$(nproc)}"

# fixed build timestamp for the kernel/busybox builds: keeps the guest image
# byte-reproducible given the same pinned inputs
SOURCE_DATE_EPOCH="${SOURCE_DATE_EPOCH:-1700000000}"