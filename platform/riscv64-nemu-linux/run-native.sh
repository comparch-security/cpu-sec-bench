#!/usr/bin/env bash
#
# cpu-sec-bench platform "riscv64-nemu-linux": the host-native reference run.
#
#   platform/riscv64-nemu-linux/run-native.sh              # RUNSET=acc
#   RUNSET=all platform/riscv64-nemu-linux/run-native.sh
#
# Compiles and runs the same record subset directly on this workstation with
# the host toolchain (no NEMU), in a scratch copy of the bench tree under
# out/native/tree seeded with the same derived configure.json the guest image
# was built from.  The result distribution is what the guest run is compared
# against, per doc/porting.html's port-verification method -- and the scratch
# scheduler is built from the same (working-tree) sources as the guest's, so
# the comparison isolates the venue, not the bench revision.
#
# Outputs: out/native/{results.json,results.dat,native-run.log} and the
# scratch tree itself.  The repo is never built or cleaned by this script.

set -euo pipefail

PLATFORM_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$PLATFORM_DIR/env.sh"

BENCH_DIR="$PLATFORM_DIR/../.."
RUNSET="${RUNSET:-acc}"
DEST="$OUT/$RUNSET"
NATIVE="$OUT/native"
TREE="$NATIVE/tree"

[ -f "$DEST/configure.json" ] || { echo "ERROR: no derived configure.json at $DEST; run build-corpus.sh first" >&2; exit 1; }

rm -rf "$TREE"
mkdir -p "$TREE" "$NATIVE"
tar -C "$BENCH_DIR" \
    --exclude=./.git --exclude=./tmp --exclude=./test --exclude=./trace-log \
    --exclude=./platform/riscv64-nemu-linux/out \
    -cf - . | tar -C "$TREE" -xf -
rm -f "$TREE/results.json" "$TREE/variables.json" "$TREE/results.dat"
cp "$DEST/configure.json" "$TREE/configure.json"

cd "$TREE"
echo "== host scheduler build (scratch tree)"
# host leg: it must not inherit the cross toolchain the documented override
# exports -- the Makefile's `?=` honours the environment, which would build a
# riscv64 scheduler this host cannot run.
env -u CXX -u ASM -u OBJDUMP make >/dev/null

echo "== native exhausted-run"
set +e
./run-test exhausted-run > "$NATIVE/native-run.log" 2>&1
rc=$?
set -e
if [ "$rc" -ne 0 ]; then
    echo "ERROR: run-test exited $rc (see $NATIVE/native-run.log)" >&2
    exit 1
fi
cp results.json "$NATIVE/results.json"
[ -f results.dat ] && cp results.dat "$NATIVE/results.dat"

python3 - "$NATIVE" "$DEST" <<'PY'
import json, os, sys
native, dest = sys.argv[1], sys.argv[2]
records = json.load(open(os.path.join(native, "results.json")))
total = len(json.load(open(os.path.join(dest, "configure.json"))))
print("records in results.json: %d/%d" % (len(records), total))
PY

echo "== done: $NATIVE"