#!/usr/bin/env python3
"""Two-way guard: doc/test-catalog.html and configure.json agree.

Reads the catalog structurally -- it collects the text of every element
carrying class="rec" (a record name) or class="prog" (a program name),
whatever the tag -- and asserts set equality against configure.json:

    {rec elements}  == set(configure.json keys)
    {prog elements} == {program field, else the record key}

Set equality, not containment. A record added to configure.json but absent
from the catalog fails, and so does a record removed from configure.json but
left behind in the catalog (the phantom-row case a one-directional check
silently passes).

Substring matching is deliberately NOT used: 22 record names are proper
substrings of other record names (cfi-call-fake-vtable is a prefix of
cfi-call-fake-vtable-arg-num) and 154 program names are substrings of record
names, so a page that dropped a row could still textually "contain" its name
inside a sibling's.

The class names are reserved for names only. No other element in doc/ uses
them -- in particular doc/content.html's summary table carries plain cells,
so a count can never be read as a record name.

Usage (runs from any working directory):
    python3 script/check_catalog.py [path-to-catalog-page]

The page defaults to <repo>/doc/test-catalog.html; the repo root is
resolved from this script's own location, not the current directory.

Exit 0 on pass, 1 on failure, 2 on internal error. Stdlib only. Idempotent.
"""

import json
import os
import sys
from html.parser import HTMLParser

REPO_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CONFIG = os.path.join(REPO_ROOT, "configure.json")
DEFAULT_CATALOG = os.path.join(REPO_ROOT, "doc", "test-catalog.html")

BUCKETS = ("rec", "prog")


class CellCollector(HTMLParser):
    """Collect the text of every element carrying class 'rec' or 'prog'.

    Tag-agnostic: the catalog may use <td class="prog"> for a column and
    <span class="rec"> for inline record lists. Text is attributed to the
    innermost tracked element only, so accidental nesting cannot
    double-count.
    """

    def __init__(self):
        super().__init__(convert_charrefs=True)
        self.buckets = {b: [] for b in BUCKETS}
        self._stack = []          # [tag, bucket, buffer] for tracked elements
        self.malformed = []       # entries holding anything but one bare name

    def handle_starttag(self, tag, attrs):
        classes = (dict(attrs).get("class") or "").split()
        bucket = next((c for c in classes if c in BUCKETS), None)
        if bucket is not None:
            self._stack.append([tag, bucket, []])

    def handle_startendtag(self, tag, attrs):
        self.handle_starttag(tag, attrs)
        self.handle_endtag(tag)

    def handle_endtag(self, tag):
        if self._stack and self._stack[-1][0] == tag:
            _, bucket, buf = self._stack.pop()
            text = "".join(buf).strip()
            self.buckets[bucket].append(text)
            if len(text.split()) != 1:
                self.malformed.append((bucket, text[:80]))

    def handle_data(self, data):
        if self._stack:
            self._stack[-1][2].append(data)


def fail(msg):
    print(msg, file=sys.stderr)
    sys.exit(1)


def main():
    catalog = sys.argv[1] if len(sys.argv) > 1 else DEFAULT_CATALOG

    for path in (CONFIG, catalog):
        if not os.path.isfile(path):
            print("internal error: %s not found" % path, file=sys.stderr)
            sys.exit(2)

    with open(CONFIG, encoding="utf-8") as fh:
        config = json.load(fh)

    parser = CellCollector()
    with open(catalog, encoding="utf-8") as fh:
        parser.feed(fh.read())

    # run-test.cpp case_parser(): the program is the explicit 'program' field,
    # otherwise the record key itself.
    want_recs = set(config)
    want_progs = {v.get("program", k) for k, v in config.items()}
    got_recs = set(parser.buckets["rec"])
    got_progs = set(parser.buckets["prog"])

    problems = []
    if parser.malformed:
        problems.append("%d cell(s) do not hold exactly one bare name, e.g. %r"
                        % (len(parser.malformed), parser.malformed[0]))
    for label, want, got in (("record", want_recs, got_recs),
                             ("program", want_progs, got_progs)):
        missing = sorted(want - got)
        extra = sorted(got - want)
        if missing:
            problems.append("%s set: %d missing name(s): %s"
                            % (label, len(missing), ", ".join(missing)))
        if extra:
            problems.append("%s set: %d extra name(s): %s"
                            % (label, len(extra), ", ".join(extra)))

    if problems:
        print("FAIL: catalog/config disagreement —", file=sys.stderr)
        for p in problems:
            print("  - " + p, file=sys.stderr)
        sys.exit(1)

    print("catalog set-equality OK: %d/%d records and %d/%d programs match "
          "configure.json in both directions (no missing, no extra)"
          % (len(got_recs), len(want_recs), len(got_progs), len(want_progs)))
    return 0


if __name__ == "__main__":
    sys.exit(main())
