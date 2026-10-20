#!/usr/bin/env bash
#
# cpu-sec-bench platform "riscv64-nemu-linux": build the benchmark corpus for
# the NEMU guest and stage it (with the riscv64 scheduler and the runtime
# libraries) under out/<runset>/.
#
#   platform/riscv64-nemu-linux/build-corpus.sh              # RUNSET=acc
#   RUNSET=all platform/riscv64-nemu-linux/build-corpus.sh
#
# Pipeline, all in the bench work tree (terminated by `make clean`):
#   1. host scheduler build
#   2. derive the configure.json subset for RUNSET into out/<runset>/
#   3. corpus build: the host scheduler drives `make -B` for every
#      (program, macro-map) unit through the cross toolchain and snapshots
#      each build as test/<program>__<key>      (`corpus make-only`)
#   4. cross-build the riscv64 scheduler
#   5. stage out/<runset>/bench/ (scheduler, configure.json, corpus files,
#      libcfi.so, sys_info) and out/<runset>/lib/ (every NEEDED library and
#      the PT_INTERP loader, resolved from the cross toolchain)
#   6. verify the staged corpus with corpus-manifest.py (writes manifest.json)
#
# Both scheduler invocations append `exhausted-run`: with a filtered subset a
# plain run aborts on requires that cross the subset boundary, and both
# phases then enumerate the same argument kind (`default_args` first), so the
# units built and the units run agree by construction.

set -euo pipefail

PLATFORM_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$PLATFORM_DIR/env.sh"

BENCH_DIR="$(cd "$PLATFORM_DIR/../.." && pwd)"
RUNSET="${RUNSET:-acc}"
DEST="$OUT/$RUNSET"
CFG="$BENCH_DIR/configure.json"
CFG_BACKUP="$DEST/configure.json.orig"

rm -rf "$DEST/bench" "$DEST/lib"   # no stale staging from an earlier RUNSET may survive
mkdir -p "$DEST/bench/test" "$DEST/bench/lib/common" "$DEST/lib"

cd "$BENCH_DIR"
# The scheduler always reads configure.json from its cwd: the RUNSET subset is
# swapped into the work tree for the duration of this script (the committed
# registry is never edited) and restored on exit, before `make clean`.
cp "$CFG" "$CFG_BACKUP"
trap 'cp "$CFG_BACKUP" "$CFG"; make clean >/dev/null 2>&1 || true' EXIT
make clean >/dev/null 2>&1 || true

echo "== [1/6] host scheduler build"
# The host leg must not inherit a cross toolchain.  The documented override
# (env.sh) exports CXX/ASM/OBJDUMP, and the Makefile's `?=` honours the
# environment, so a bare `make` here would cross-compile the host scheduler --
# which stage [3/6] then could not execute on this machine.  `env -u` restores
# the Makefile's own defaults for this one invocation.
env -u CXX -u ASM -u OBJDUMP make >/dev/null

echo "== [2/6] derive configure.json subset (RUNSET=$RUNSET)"
python3 - "$RUNSET" "$DEST/configure.json" <<'PY'
import json, sys
runset, dest = sys.argv[1], sys.argv[2]
cfg = json.load(open("configure.json"))
sub = cfg if runset == "all" else {k: v for k, v in cfg.items() if k.startswith(runset + "-")}
if not sub:
    sys.exit("RUNSET=%s selects no records" % runset)
with open(dest, "w") as f:
    json.dump(sub, f, indent=4)
print("records: %d" % len(sub))
PY
cp "$DEST/configure.json" "$CFG"

echo "== [3/6] corpus build (ARCH=riscv64, scheduler-driven, forced -B)"
env ARCH=riscv64 CXX="$CXX" ASM="$ASM" OBJDUMP="$OBJDUMP" \
    ./run-test corpus make-only exhausted-run 2>&1 | tee "$DEST/corpus-build.log"

if grep -q '"make-status"' results.json; then
    echo "WARNING: the corpus build could not build some records:" >&2
    python3 -c 'import json;[print("  " + k) for k,v in json.load(open("results.json")).items() if "make-status" in v]' >&2
    echo "the manifest step below will fail on the missing corpus files" >&2
fi

echo "== [4/6] cross-build the riscv64 scheduler"
make -B run-test ARCH=riscv64 CXX="$CXX" ASM="$ASM" OBJDUMP="$OBJDUMP" >"$DEST/scheduler-crossbuild.log" 2>&1

echo "== [5/6] stage bench/ and lib/"
install -m 0755 run-test "$DEST/bench/run-test"
cp "$DEST/configure.json" "$DEST/bench/configure.json"
cp -p test/*__* "$DEST/bench/test/"
# Run-time support files that are not <program>__<key> units, so the glob above
# cannot see them -- both are the bench's own run-time contract, found by the
# 2026-10-20 guest run (19 of the 22 acc verdicts matched the native host
# reference; all three differences were cases that exit their "known failure
# point" code 2 because one of these files was missing from the staged tree):
#   test/acc-check-ASLR                 -- acc/check-ASLR.cpp re-invokes the
#       program through the hard-coded relative path "test/acc-check-ASLR 1
#       <flag>" (system(), check-ASLR.cpp:51); with only the corpus file
#       staged the inner call finds nothing and the case exits 2 ("should
#       never run to here").  It is the only test in the bench that shells out
#       by a fixed path (grep -rn 'system(' over acc/ cfi/ cpi/ mss/ mts/).
#   test/acc-read-func-func-opcode.tmp  -- the build-time opcode side file
#       acc/read-func.cpp reads from its argv[1]; without it the case exits 2
#       ("no side file at all").
# Both files serve acc programs only, so a class runset that selects neither is
# not required to have built them: stage what exists, and demand a file only when
# the selected records are the ones that reach it.  (Requiring them outright made
# every per-class RUNSET abort here -- a cfi/cpi/mss/mts subset builds neither
# program, and the `make clean` above guarantees no leftover from an earlier run.)
subset_has_program() { # program -> 0 when the derived subset contains it
    python3 -c 'import json,sys; d=json.load(open(sys.argv[1])); sys.exit(0 if any(v.get("program",k)==sys.argv[2] for k,v in d.items()) else 1)' \
        "$DEST/configure.json" "$1"
}
for f in test/acc-check-ASLR:acc-check-ASLR test/acc-read-func-func-opcode.tmp:acc-read-func; do
    file="${f%%:*}"; prog="${f##*:}"
    if [ -f "$file" ]; then
        cp -p "$file" "$DEST/bench/test/"
    elif subset_has_program "$prog"; then
        echo "ERROR: run-time support file $file is missing after the corpus build (needed by $prog)" >&2
        exit 1
    fi
done
if [ -f lib/common/libcfi.so ]; then
    cp -p lib/common/libcfi.so "$DEST/bench/lib/common/libcfi.so"
fi

# The guest's own system description: report_gen() prefixes results.dat with
# this file, so it must describe the venue the results came from -- not the
# host that built them (the Makefile's own test/sys_info.txt is the host's).
{
    echo "Venue: cpu-sec-bench inside a booted Linux guest under XiangShan NEMU (system mode, riscv64)"
    echo "Corpus compiler: $("$CXX" --version | head -n1)"
    echo "Corpus objdump: $("$OBJDUMP" --version | head -n1)"
    echo "Bench commit: $(git -C "$BENCH_DIR" rev-parse HEAD)"
    # A `-dirty` suffix means the corpus came from an uncommitted tree: the commit
    # alone would not rebuild it.  Recorded so the provenance is not misleading.
    echo "Bench tree: $(git -C "$BENCH_DIR" describe --always --dirty)"
    echo "Platform: platform/riscv64-nemu-linux (RUNSET=$RUNSET)"
} > "$DEST/bench/test/sys_info.txt"

# Runtime libraries: closure over NEEDED plus the PT_INTERP loader, resolved
# through the cross compiler's own search path.  libcfi.so is NOT staged to
# /lib: the cpi/cfi binaries reach it through their relative RUNPATH
# (lib/common/) with the guest's cwd at /bench.
is_elf() { [ "$(head -c4 "$1" | od -An -tx1 | tr -d ' \n')" = "7f454c46" ]; }
declare -a queue=()
queue+=("$DEST/bench/run-test")
for f in "$DEST/bench/test/"*; do queue+=("$f"); done
[ -f "$DEST/bench/lib/common/libcfi.so" ] && queue+=("$DEST/bench/lib/common/libcfi.so")
declare -A staged_seen=()
while [ "${#queue[@]}" -gt 0 ]; do
    f="${queue[0]}"; queue=("${queue[@]:1}")
    [ -n "${staged_seen[$f]:-}" ] && continue
    staged_seen[$f]=1
    is_elf "$f" || continue
    interp="$("$READELF" -l "$f" | sed -n 's/.*Requesting program interpreter: \(.*\)\]/\1/p')"
    for soname in $interp $("$OBJDUMP" -p "$f" | awk '$1=="NEEDED"{print $2}'); do
        base="$(basename "$soname")"
        [ -e "$DEST/lib/$base" ] && continue
        [ -e "$DEST/bench/lib/common/$base" ] && continue   # relative-RUNPATH library
        src="$("$CXX" -print-file-name="$base")"
        if [ ! -f "$src" ]; then
            echo "ERROR: unresolved library $base (needed by $f)" >&2
            exit 1
        fi
        cp -pL "$src" "$DEST/lib/$base"
        queue+=("$DEST/lib/$base")
    done
done
echo "staged libs: $(ls "$DEST/lib" | tr '\n' ' ')"

echo "== [6/6] verify the staged corpus (manifest)"
python3 "$PLATFORM_DIR/corpus-manifest.py" \
    --bench-dir "$DEST/bench" \
    --configure "$DEST/configure.json" \
    --runset "$RUNSET" \
    --bench-commit "$(git -C "$BENCH_DIR" rev-parse HEAD)" \
    --bench-tree "$(git -C "$BENCH_DIR" describe --always --dirty)" \
    --toolchain "$("$CXX" --version | head -n1)" \
    --objdump "$("$OBJDUMP" --version | head -n1)" \
    --out "$DEST/manifest.json"

echo "== done: $DEST"