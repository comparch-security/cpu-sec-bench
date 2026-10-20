#!/usr/bin/env python3
"""Verify a staged riscv64 NEMU-guest corpus and write its manifest.

Reimplements the scheduler's corpus naming contract independently of the C++
(see scheduler/run-test.cpp corpus_key()): the key is FNV-1a-32 over the
record's macro map rendered as `KEY=VALUE` pairs with keys sorted, `,`-joined
(empty map keys to 811c9dc5), and every build unit is test/<program>__<key>.

Checks, all failing loudly (exit 1) with the offending names on stderr:
  * the staged corpus file set equals the expected (program, macro-map) set
    -- derived from the *staged* configure.json exactly as the scheduler's
    build phase enumerates it (exhausted-run arg kind: default_args first)
  * no two macro maps of one program collide on a key
  * the longest corpus path stays within the scheduler's 63-char budget

The manifest (<out>) is deterministic: per-corpus-file sha256 plus the macro
maps, and per-support-file sha256 plus the bench-commit / toolchain
provenance handed in on the command line.  Re-running over the same staged
tree rewrites it byte-identically.

Usage: corpus-manifest.py --bench-dir DIR --configure JSON [--runset S]
                          [--bench-commit C] [--bench-tree B] [--toolchain T]
                          [--objdump O] --out FILE
"""
import argparse
import hashlib
import json
import os
import sys

SENTINEL_EMPTY_MAP_KEY = "811c9dc5"  # FNV-1a-32 of the empty canonical string
PATH_BUDGET = 63  # longest corpus path the scheduler's argument pool must take


def fnv1a32(s):
    """FNV-1a-32 over the canonical macro-map string, %08x."""
    h = 0x811c9dc5
    for b in s.encode():
        h ^= b
        h = (h * 0x01000193) & 0xFFFFFFFF
    return "%08x" % h


def canon(macro_map):
    return ",".join("%s=%s" % (k, macro_map[k]) for k in sorted(macro_map))


def build_plan(rec):
    """Macro maps in the order the scheduler's build phase builds them, as
    (canonical string, conf-macro entry) pairs.

    Mirrors run-tests()/case_parser() under `corpus make-only exhausted-run`:
    the argument kind is default_args when present, and a build happens for
    the first argument list and whenever the map changes from the last build.

    The entry dict travels alongside the canonical string because the joined
    string is not a faithful round-trip source: a macro VALUE may itself
    contain commas (the mss bare-array-pointer programs carry values such as
    'VAL_4U,VAL_2U,VAL_1U'), so re-splitting it would corrupt the map the
    manifest records.
    """
    args = rec.get("default_args", rec.get("arguments"))
    # The registry writes argument lists as JSON objects keyed "0".."N-1", and
    # the scheduler's arg_parser iterates their members, so both shapes have to
    # be counted here; an empty container still yields index 0, because
    # arg_parser always produces at least one argument list -- hence the floor.
    n = max(1, len(args)) if isinstance(args, (list, dict)) else 1
    cf = rec.get("conf-macro") or {}
    maps = []
    for ind in range(n):
        entry = cf.get(str(ind), {})
        if not isinstance(entry, dict):
            sys.exit("record %r: conf-macro[%d] is not a map" % (rec, ind))
        m = canon(entry)
        if not maps or maps[-1][0] != m:
            maps.append((m, entry))
    return maps


def sha256_file(path):
    h = hashlib.sha256()
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--bench-dir", required=True, help="staged bench tree (contains run-test, test/, configure.json)")
    ap.add_argument("--configure", required=True, help="the staged configure.json (the RUNSET subset)")
    ap.add_argument("--runset", default="", help="the RUNSET name this corpus was built for")
    ap.add_argument("--bench-commit", default="", help="cpu-sec-bench commit the corpus was built from")
    ap.add_argument("--bench-tree", default="", help="`git describe --always --dirty` of that tree; a -dirty suffix means the corpus came from uncommitted sources")
    ap.add_argument("--toolchain", default="", help="corpus compiler version line")
    ap.add_argument("--objdump", default="", help="corpus objdump version line")
    ap.add_argument("--out", required=True, help="manifest.json output path")
    args = ap.parse_args()

    proc = []
    def check(name, ok, detail=""):
        print("%s  %-58s %s" % ("PASS" if ok else "FAIL", name, detail))
        proc.append(ok)

    cfg = json.load(open(args.configure))
    expected = {}       # corpus file name -> (program, canonical map, macro entry)
    per_program = {}    # program -> {key: {canonical maps sharing that key}}
    for key, rec in cfg.items():
        prog = rec.get("program", key)
        for m, entry in build_plan(rec):
            name = "%s__%s" % (prog, fnv1a32(m))
            expected[name] = (prog, m, entry)
            per_program.setdefault(prog, {}).setdefault(fnv1a32(m), set()).add(m)

    test_dir = os.path.join(args.bench_dir, "test")
    actual = sorted(f for f in os.listdir(test_dir) if "__" in f)

    missing = sorted(set(expected) - set(actual))
    extra = sorted(set(actual) - set(expected))
    check("staged corpus set equals the expected unit set",
          not missing and not extra,
          "expected=%d actual=%d missing=%r extra=%r" %
          (len(expected), len(actual), missing[:3], extra[:3]))

    # A collision is one <key> carrying more than one distinct macro map: the later
    # build would overwrite the earlier snapshot, so a single file would serve two
    # build units.  Collecting the maps per key is what lets this check fail at
    # all -- keying the collection by the hash itself collapses the very
    # collision it is meant to detect.
    collisions = {p: {k: sorted(v) for k, v in km.items() if len(v) > 1}
                  for p, km in per_program.items()}
    collisions = {p: v for p, v in collisions.items() if v}
    check("per-program corpus keys are collision-free", not collisions, "collisions=%r" % collisions)

    longest = max((len("test/" + f) for f in actual), default=0)
    check("longest corpus path within the %d-char budget" % PATH_BUDGET,
          longest <= PATH_BUDGET, "longest=%d" % longest)

    corpus_files = []
    for name in sorted(expected):
        path = os.path.join(test_dir, name)
        prog, m, entry = expected[name]
        if not os.path.exists(path):
            continue
        corpus_files.append({
            "name": "test/" + name,
            "program": prog,
            "macro_map": entry,
            "canonical": m,
            "sha256": sha256_file(path),
        })

    support = []
    for root, _dirs, files in os.walk(args.bench_dir):
        for f in sorted(files):
            path = os.path.join(root, f)
            rel = os.path.relpath(path, args.bench_dir)
            if rel.startswith("test/") and "__" in os.path.basename(rel):
                continue
            support.append({"name": rel, "sha256": sha256_file(path)})
    support.sort(key=lambda e: e["name"])

    manifest = {
        "plan": "2026-10-11-riscv64-nemu-linux-platform",
        "platform": "riscv64-nemu-linux",
        "runset": args.runset,
        "bench_commit": args.bench_commit,
        "bench_tree": args.bench_tree,
        "toolchain": args.toolchain,
        "objdump": args.objdump,
        "records": len(cfg),
        "programs": len(per_program),
        "units": len(expected),
        "max_corpus_path_chars": longest,
        "corpus_files": corpus_files,
        "support_files": support,
    }
    with open(args.out, "w") as f:
        json.dump(manifest, f, indent=2, sort_keys=True)
        f.write("\n")

    print("manifest: %d units over %d programs, %d mismatches, longest path %d chars -> %s"
          % (len(expected), len(per_program), len(missing) + len(extra), longest, args.out))
    return 1 if not all(proc) else 0


if __name__ == "__main__":
    sys.exit(main())