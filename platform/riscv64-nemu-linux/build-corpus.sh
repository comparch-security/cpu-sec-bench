#!/usr/bin/env bash
#
# cpu-sec-bench platform "riscv64-nemu-linux": build the benchmark corpus for
# the NEMU guest and stage it (with the riscv64 scheduler and the runtime
# libraries) under out/<runset>/.
#
#   platform/riscv64-nemu-linux/build-corpus.sh              # RUNSET=acc
#   RUNSET=all platform/riscv64-nemu-linux/build-corpus.sh
#
# The corpus is built in a scratch copy of the bench tree at
# out/build/<runset>/tree, outside the published out/<runset>/ venue directory.
# The tracked work tree is never written and never cleaned, so the platform
# cannot leave a `-dirty` marker on it and a killed run cannot leak a registry.
#
# Pipeline:
#   1. derive the configure.json subset for RUNSET into out/<runset>/
#   2. copy the work tree into the scratch tree (excluding .git, tmp, test,
#      trace-log and the venue's own out/), `make clean` it so no generated
#      artifact is inherited, and place the derived registry there
#   3. host scheduler build
#   4. corpus build: the host scheduler drives `make -B` for every
#      (program, macro-map) unit through the cross toolchain and snapshots
#      each build as test/<program>__<key>      (`corpus make-only`)
#   5. cross-build the riscv64 scheduler
#   6. stage out/<runset>/bench/ (scheduler, configure.json, corpus files,
#      libcfi.so, sys_info) and out/<runset>/lib/ (every NEEDED library and
#      the PT_INTERP loader, resolved from the cross toolchain)
#   7. verify the staged corpus with corpus-manifest.py; manifest.json is
#      published only after every check passes
#
# Both scheduler invocations append `exhausted-run`: with a filtered subset a
# plain run aborts on requires that cross the subset boundary, and both
# phases then enumerate the same argument kind (`default_args` first), so the
# units built and the units run agree by construction.
#
# Provenance (BENCH_COMMIT / BENCH_TREE) is read from the untouched work tree
# before anything is copied or derived.

set -euo pipefail

PLATFORM_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$PLATFORM_DIR/env.sh"

BENCH_DIR="$(cd "$PLATFORM_DIR/../.." && pwd)"
RUNSET="${RUNSET:-acc}"
# RUNSET becomes a path component under $OUT, so it may not be anything else.
case "$RUNSET" in
    ""|*[!A-Za-z0-9_-]*)
        echo "ERROR: RUNSET must match [A-Za-z0-9_-]+ (got '$RUNSET')" >&2
        exit 1 ;;
esac
DEST="$OUT/$RUNSET"
TREE="$OUT/build/$RUNSET/tree"

# Provenance from the untouched work tree, captured before any copy or build:
# the scratch tree's own state must never be reported as the bench tree's.
BENCH_COMMIT="$(git -C "$BENCH_DIR" rev-parse HEAD)"
BENCH_TREE="$(git -C "$BENCH_DIR" describe --always --dirty)"

# One build per venue directory at a time: the staging area and the scratch
# tree are shared mutable state, and a second concurrent run would interleave
# writes into both.  Fail fast, naming the holder.
mkdir -p "$DEST"
exec 9>>"$DEST/.lock"
if ! flock -n 9; then
    echo "ERROR: another corpus build holds $DEST/.lock (holder: $(cat "$DEST/.lock" 2>/dev/null || echo unknown))" >&2
    exit 1
fi
printf '%s\n' "$$" > "$DEST/.lock"

rm -rf "$DEST/bench" "$DEST/lib"   # no stale staging from an earlier RUNSET may survive
rm -f "$DEST/manifest.json"        # and a stale manifest must not satisfy build-image.sh's guard
mkdir -p "$DEST/bench/test" "$DEST/bench/lib/common" "$DEST/lib"

echo "== [1/7] derive configure.json subset (RUNSET=$RUNSET)"
python3 - "$RUNSET" "$DEST/configure.json" "$BENCH_DIR/configure.json" <<'PY'
import json, sys
runset, dest, source = sys.argv[1], sys.argv[2], sys.argv[3]
cfg = json.load(open(source))
sub = cfg if runset == "all" else {k: v for k, v in cfg.items() if k.startswith(runset + "-")}
if not sub:
    sys.exit("RUNSET=%s selects no records" % runset)
with open(dest, "w") as f:
    json.dump(sub, f, indent=4)
print("records: %d" % len(sub))
PY

echo "== [2/7] scratch tree ($TREE)"
rm -rf "$TREE"
mkdir -p "$TREE"
tar -C "$BENCH_DIR" \
    --exclude=./.git --exclude=./tmp --exclude=./test --exclude=./trace-log \
    --exclude=./platform/riscv64-nemu-linux/out \
    -cf - . | tar -C "$TREE" -xf -
# Nothing generated may be inherited from the work tree: an earlier run's
# run-test, objects or libcfi.so in the copy would be staged as if this run had
# built them (the build products are gitignored, so a clean `git status` says
# nothing about them).  The clean recipe is `-rm -rf ...` -- the leading dash
# makes make ignore a failing `rm` -- so its exit status cannot carry the
# guarantee; assert the generated roots are actually gone instead.
make -C "$TREE" clean
# The clean list is ARCH-scoped (lib/$(ARCH)/...), so a work tree cross-built
# for another lane keeps lib/riscv64/*.o and lib/aarch64/*.o across this
# host-ARCH clean -- exactly the objects the riscv64 corpus build would reuse.
# Remove them directly; the assertion below stays the observable guarantee.
rm -f "$TREE"/lib/*/assembly.o "$TREE"/lib/*/indepassembly*.o
[ ! -e "$TREE/run-test" ] || { echo "ERROR: inherited generated artifact $TREE/run-test survived the scratch tree's clean" >&2; exit 1; }
[ ! -e "$TREE/lib/common/libcfi.so" ] || { echo "ERROR: inherited generated artifact $TREE/lib/common/libcfi.so survived the scratch tree's clean" >&2; exit 1; }
stale_obj="$(find "$TREE/lib" -name '*.o' -print -quit)"
[ -z "$stale_obj" ] || { echo "ERROR: inherited generated artifact $stale_obj survived the scratch tree's clean" >&2; exit 1; }
cp "$DEST/configure.json" "$TREE/configure.json"

cd "$TREE"

echo "== [3/7] host scheduler build"
# The host leg must not inherit a cross toolchain.  The documented override
# (env.sh) exports CXX/ASM/OBJDUMP, and the Makefile's `?=` honours the
# environment, so a bare `make` here would cross-compile the host scheduler --
# which stage [4/7] then could not execute on this machine.  `env -u` restores
# the Makefile's own defaults for this one invocation.
env -u CXX -u ASM -u OBJDUMP make >/dev/null

echo "== [4/7] corpus build (ARCH=riscv64, scheduler-driven, forced -B)"
env ARCH=riscv64 CXX="$CXX" ASM="$ASM" OBJDUMP="$OBJDUMP" \
    ./run-test corpus make-only exhausted-run 2>&1 | tee "$DEST/corpus-build.log"

if grep -q '"make-status"' results.json; then
    echo "WARNING: the corpus build could not build some records:" >&2
    python3 -c 'import json;[print("  " + k) for k,v in json.load(open("results.json")).items() if "make-status" in v]' >&2
    echo "the manifest step below will fail on the missing corpus files" >&2
fi

echo "== [5/7] cross-build the riscv64 scheduler"
make -B run-test ARCH=riscv64 CXX="$CXX" ASM="$ASM" OBJDUMP="$OBJDUMP" >"$DEST/scheduler-crossbuild.log" 2>&1

echo "== [6/7] stage bench/ and lib/"
install -m 0755 "$TREE/run-test" "$DEST/bench/run-test"
cp "$DEST/configure.json" "$DEST/bench/configure.json"
cp -p "$TREE"/test/*__* "$DEST/bench/test/"
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
# program, and the scratch tree's `make clean` guarantees no leftover from an
# earlier run.)
subset_has_program() { # program -> 0 when the derived subset contains it
    python3 -c 'import json,sys; d=json.load(open(sys.argv[1])); sys.exit(0 if any(v.get("program",k)==sys.argv[2] for k,v in d.items()) else 1)' \
        "$DEST/configure.json" "$1"
}
for f in test/acc-check-ASLR:acc-check-ASLR test/acc-read-func-func-opcode.tmp:acc-read-func; do
    file="${f%%:*}"; prog="${f##*:}"
    if [ -f "$TREE/$file" ]; then
        cp -p "$TREE/$file" "$DEST/bench/test/"
    elif subset_has_program "$prog"; then
        echo "ERROR: run-time support file $file is missing after the corpus build (needed by $prog)" >&2
        exit 1
    fi
done
if [ -f "$TREE/lib/common/libcfi.so" ]; then
    cp -p "$TREE/lib/common/libcfi.so" "$DEST/bench/lib/common/libcfi.so"
fi

# The guest's own system description: report_gen() prefixes results.dat with
# this file, so it must describe the venue the results came from -- not the
# host that built them (the Makefile's own test/sys_info.txt is the host's).
# BENCH_COMMIT/BENCH_TREE were captured from the untouched work tree above: the
# scratch tree this build ran in is not provenance the guest should see, and a
# `-dirty` suffix here means the corpus really did come from an uncommitted
# bench tree.
{
    echo "Venue: cpu-sec-bench inside a booted Linux guest under XiangShan NEMU (system mode, riscv64)"
    echo "Corpus compiler: $("$CXX" --version | head -n1)"
    echo "Corpus objdump: $("$OBJDUMP" --version | head -n1)"
    echo "Bench commit: $BENCH_COMMIT"
    echo "Bench tree: $BENCH_TREE"
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

echo "== [7/7] verify the staged corpus (manifest)"
python3 "$PLATFORM_DIR/corpus-manifest.py" \
    --bench-dir "$DEST/bench" \
    --configure "$DEST/configure.json" \
    --runset "$RUNSET" \
    --bench-commit "$BENCH_COMMIT" \
    --bench-tree "$BENCH_TREE" \
    --toolchain "$("$CXX" --version | head -n1)" \
    --objdump "$("$OBJDUMP" --version | head -n1)" \
    --out "$DEST/manifest.json"

echo "== done: $DEST"
